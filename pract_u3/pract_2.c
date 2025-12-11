#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>

// Tabla para números 0 al 9 (cátodo común)
uint8_t numeros[10] = {
    0x3F, // 0
    0x06, // 1
    0x5B, // 2
    0x4F, // 3
    0x66, // 4
    0x6D, // 5
    0x7D, // 6
    0x07, // 7
    0x7F, // 8
    0x6F  // 9
};

int main(void)
{
    // ------------------------------
    // Configurar pines
    // ------------------------------

    // PORTD como salida para el display (PD0–PD6)
    DDRD = 0x7F;

    // PB0 como entrada con pull-up (botón)
    DDRB &= ~(1 << PB0);
    PORTB |= (1 << PB0);  // pull-up interno

    uint8_t contador = 0;
    PORTD = numeros[contador]; // Mostrar 0 inicial

    // ------------------------------
    // Bucle principal
    // ------------------------------
    while (1)
    {
        // Si el botón se presiona (PB0 pasa a LOW)
        if (!(PINB & (1 << PB0)))
        {
            _delay_ms(40); // Antirrebote

            if (!(PINB & (1 << PB0)))  // Verificación
            {
                // Incrementar número y reiniciar después de 9
                contador++;
                if (contador > 9) contador = 0;

                // Enviar número al display
                PORTD = numeros[contador];

                // Esperar a que se suelte el botón
                while (!(PINB & (1 << PB0)));
                _delay_ms(40);
            }
        }
    }
}

