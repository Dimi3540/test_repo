#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>

void ADC_init(void) {
    // Referencia AVcc, canal ADC0 (PC0)
    ADMUX = (1 << REFS0); // REFS0=1 -> AVcc reference, MUX bits = 0 -> ADC0

    // Habilitar ADC, prescaler 128 -> AD clock ~125kHz (ideal)
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);

    // Deshabilitar entrada digital en ADC0 para reducir ruido
    DIDR0 |= (1 << ADC0D);

    // Lectura dummy para estabilizar ADC
    ADCSRA |= (1 << ADSC);
    while (ADCSRA & (1 << ADSC));
}

uint16_t ADC_read(void) {
    // inicio conversión
    ADCSRA |= (1 << ADSC);
    while (ADCSRA & (1 << ADSC));
    // ADC (macro) ya lee ADCL y ADCH correctamente -> 0..1023
    return ADC;
}

void PWM_init(void) {
    // PD6 (OC0A) como salida
    DDRD |= (1 << PD6);

    // Timer0: Fast PWM 8-bit, non-inverting on OC0A
    TCCR0A = (1 << WGM00) | (1 << WGM01) | (1 << COM0A1);

    // Prescaler = 64 -> PWM freq ~ 16MHz/(64*256) ≈ 976 Hz
    TCCR0B = (1 << CS01) | (1 << CS00);

    OCR0A = 0; // duty inicial 0
}

int main(void) {
    ADC_init();
    PWM_init();

    uint16_t adc_val;
    uint8_t pwm_val;
    uint8_t smooth = 0; // valor suavizado del OCR

    while (1) {
        adc_val = ADC_read();       // 0..1023

        // Map 0..1023 -> 0..255 (simple shift)
        pwm_val = (uint8_t)(adc_val >> 2);

        // Suavizado simple: incrementa/decrementa un paso hacia pwm_val
        if (smooth < pwm_val) smooth++;
        else if (smooth > pwm_val) smooth--;

        OCR0A = smooth;            // actualizar duty

        _delay_ms(5);              // pequeña pausa para estabilidad visual
    }

    return 0;
}
