/*
 * File:   Practica_Display.c
 * Author: daniel
 *
 * Created on November 21, 2025, 10:31 PM
 */
#include <xc.h>
#define _XTAL_FREQ 20000000
#define DEBOUNCE_MS 25

unsigned char counter = 0;

void digitos(unsigned char num) {
    switch(num) {
        case 0:
            PORTA = 0xC1;
            PORTE = 0xFE;
            break;

        case 1:
            PORTA = 0xF3;
            PORTE = 0xFF;
            break;

        case 2:
            PORTA = 0xC9;
            PORTE = 0xFD;
            break;

        case 3:
            PORTA = 0xE1;
            PORTE = 0xFD;
            break;

        case 4:
            PORTA = 0xF3;
            PORTE = 0xFC;
            break;

        case 5:
            PORTA = 0xE5;
            PORTE = 0xFC;
            break;

        case 6:
            PORTA = 0xC5;
            PORTE = 0xFC;
            break;

        case 7:
            PORTA = 0xF1;
            PORTE = 0xFF;
            break;

        case 8:
            PORTA = 0xC1;
            PORTE = 0xFC;
            break;

        case 9:
            PORTA = 0xE1;
            PORTE = 0xFC;
            break;

        default:
            PORTA = 0xFF;
            PORTE = 0xFF;
            break;
    }
}

void main(void) {

    TRISB = 0b00001000;  // RB3 como entrada (botón)
    TRISA = 0b00000000;  // Display
    TRISE = 0b11111100;
    TRISC = 0b01111111;

    unsigned char lastState = 0;

    while(1){

        unsigned char current = PORTBbits.RB3;

        // Detecta flanco: botón presionado (1) y antes estaba en 0
        if(current == 1 && lastState == 0){
            __delay_ms(DEBOUNCE_MS);

            if(PORTBbits.RB3 == 1) {
                counter++;
                if(counter > 9) counter = 0;   // reinicia después del 9
                digitos(counter);
            }
        }

        lastState = current;
    }

    return;
}
