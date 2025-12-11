#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>

volatile long encoderCount = 0;   // Cuentas del encoder
volatile int8_t encoderDir = 0;    // Último sentido de giro

// Parámetros de control PID
long setpoint = 0;                // Posición objetivo en cuentas
long previousError = 0;           // Error de la iteración anterior
long integral = 0;                // Acumulación del error (integral)
const long POSITION_TOLERANCE = 5; // Margen de error aceptable (cuentas)
  const float Kp = 1.75;             // Ganancia proporcional (ajústala en pruebas)
const float Ki = 0.009;             // Ganancia integral (ajústala en pruebas)
const float Kd = 1.3;             // Ganancia derivativa (ajústala en pruebas)
const uint8_t PWM_MAX = 200;       // Máximo valor de PWM (8 bits)

// --- Prototipos ---
void motorStop(void);
void motorForward(void);
void motorBackward(void);
void setPWM(uint8_t duty);
long getEncoderCount(void);
void readSetpointFromSerial(void);

// -------------------- SETUP --------------------
void setup(void)
{
  cli(); // Deshabilita interrupciones globales mientras configuramos

  // ----- GPIO -----
  // Encoder: C1 -> PD2(INT0), C2 -> PD3
  DDRD &= ~((1 << DDD2) | (1 << DDD3));       // Entradas
  PORTD |= (1 << PORTD2) | (1 << PORTD3);     // Pull-up internos

  // Driver de motor:
  // D7 (PD7) -> IN1, D8 (PB0) -> IN2, D9 (PB1/OC1A) -> ENA (PWM)
  DDRD |= (1 << DDD7);                        // IN1 salida
  DDRB |= (1 << DDB0) | (1 << DDB1);          // IN2 y OC1A salidas

  // Apagar motor al inicio
  motorStop();
  setPWM(0);

  // ----- Interrupción externa INT0 para el encoder (canal C1) -----
  // Modo: flanco de subida (ISC01=1, ISC00=1)
  EICRA |= (1 << ISC01) | (1 << ISC00);
  // Habilitar INT0
  EIMSK |= (1 << INT0);
  // Limpiar posible bandera pendiente
  EIFR  |= (1 << INTF0);

  // ----- Timer1 en Fast PWM 8 bits, salida en OC1A (D9) -----
  // Borrar registros
  TCCR1A = 0;
  TCCR1B = 0;
  // Fast PWM 8 bits: WGM13:0 = 0b0101  -> WGM10=1, WGM12=1
  TCCR1A |= (1 << WGM10);
  TCCR1B |= (1 << WGM12);
  // Modo no inversor en OC1A: COM1A1=1, COM1A0=0
  TCCR1A |= (1 << COM1A1);
  // Prescaler = 8: CS11=1
  TCCR1B |= (1 << CS11);

  OCR1A = 0;   // Duty cycle inicial

  // UART solo para recibir setpoint y mandar texto al monitor serie
  Serial.begin(9600);
  Serial.println("Inicio - Control de posicion con encoder");
  Serial.println("Escribe un numero de cuentas y presiona Enter.");

  sei(); // Habilita interrupciones globales
}

// -------------------- LOOP --------------------
void loop(void)
{
  // Leer (si hay) un nuevo setpoint desde el monitor serie
  readSetpointFromSerial();

  // Leer posición actual de forma atómica
  long posicion = getEncoderCount();
  long error = setpoint - posicion;
  long absError = (error >= 0) ? error : -error;

  // Depuración: Imprimir los valores para verificar la posición y setpoint
  Serial.print("Posicion actual: ");
  Serial.print(posicion);
  Serial.print(" | Setpoint: ");
  Serial.println(setpoint);

  // Si estamos dentro del margen de tolerancia, detenemos el motor
  if (absError <= POSITION_TOLERANCE)
  {
    motorStop();
    setPWM(0);
  }
  else
  {
    // Control PID: Calculando la salida PID
    integral += error;  // Acumula el error para el término integral
    long derivative = error - previousError;  // Calcula el cambio en el error (término derivativo)

    // Calcula el valor del PWM usando los tres términos
    uint32_t pwm = (Kp * error) + (Ki * integral) + (Kd * derivative);

    // Asegurarse de que PWM no exceda el máximo
    if (pwm > PWM_MAX)
    {
      pwm = PWM_MAX;
    }
    if (pwm < 0)
    {
      pwm = 0;
    }

    setPWM((uint8_t)pwm);

    // Actualiza el valor del error para la próxima iteración
    previousError = error;

    // Decidir sentido de giro según el signo del error
    if (error > 0)
    {
      motorForward();
    }
    else
    {
      motorBackward();
    }
  }

  delay(25);  // Esperar un poco antes de la siguiente iteración
}

// -------------------- ISR ENCODER --------------------
// INT0 se dispara en flanco de subida del canal C1.
// El sentido de giro se obtiene leyendo el estado del canal C2.
ISR(INT0_vect)
{
  // Leer canal B (C2, PD3)
  if (PIND & (1 << PIND3))
  {
    encoderCount++;
    encoderDir = 1;
  }
  else
  {
    encoderCount--;
    encoderDir = -1;
  }
}

// -------------------- FUNCIONES AUXILIARES --------------------
void motorStop(void)
{
  // Ambas entradas en LOW => motor libre (coasting) en la mayoría de drivers
  PORTD &= ~(1 << PORTD7); // IN1 = 0
  PORTB &= ~(1 << PORTB0); // IN2 = 0
}

void motorForward(void)
{
  // IN1 = 1, IN2 = 0
  PORTD |= (1 << PORTD7);
  PORTB &= ~(1 << PORTB0);
}

void motorBackward(void)
{
  // IN1 = 0, IN2 = 1
  PORTD &= ~(1 << PORTD7);
  PORTB |= (1 << PORTB0);
}

void setPWM(uint8_t duty)
{
  OCR1A = duty;   // 0-255
}

// Copia atómica de la variable del encoder (tipo long, 32 bits)
long getEncoderCount(void)
{
  long value;
  uint8_t sreg = SREG; // guardar estado de interrupciones
  cli();
  value = encoderCount;
  SREG = sreg;         // restaurar SREG (interrupciones)
  return value;
}

// Lee enteros por el monitor serie como nuevo setpoint.
// Escribe un numero (en cuentas) y presiona Enter.
void readSetpointFromSerial(void)
{
  static char buffer[16];
  static uint8_t index = 0;

  while (Serial.available() > 0)
  {
    char c = Serial.read();
    if (c == '\n' || c == '\r')
    {
      if (index > 0)
      {
        buffer[index] = '\0';  // Termina el string
        long value = atol(buffer);  // Convierte a long
        setpoint = value;  // Actualiza el setpoint
        index = 0;

        Serial.print("Nuevo setpoint: ");
        Serial.println(setpoint);
      }
    }
    else
    {
      if (index < sizeof(buffer) - 1)
      {
        buffer[index++] = c;
      }
    }
  }
}
