#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>

// ===============================
//  Toggle LED con cada pulso
// ===============================

int main(void)
{
    // Configurar PB0 como salida (LED)
    DDRB |= (1 << PB0);

    // Configurar PD2 como entrada con pull-up activado
    DDRD &= ~(1 << PD2);     // Entrada
    PORTD |= (1 << PD2);     // Pull-up interno

    uint8_t estado_led = 0;  // Guarda el estado ON/OFF del LED

    while (1)
    {
        // Detectar pulso (cuando el botón pasa de HIGH → LOW)
        if (!(PIND & (1 << PD2)))  // Botón presionado (activo en LOW)
        {
            _delay_ms(50);         // Anti-rebote

            if (!(PIND & (1 << PD2)))
            {
                // Cambia el estado
                estado_led ^= 1;

                if (estado_led)
                    PORTB |= (1 << PB0);  // Encender
                else
                    PORTB &= ~(1 << PB0); // Apagar

                // Esperar a que el botón se suelte
                while (!(PIND & (1 << PD2)));
                _delay_ms(50);
            }
        }
    }
}
