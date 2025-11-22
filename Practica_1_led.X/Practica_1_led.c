/*
 * File:   Practica_1_led.c
 * Author: daniel
 *
 * Created on November 21, 2025, 9:45 PM
 */

// PIC16F877A Configuration Bit Settings

// 'C' source line config statements

// CONFIG
#pragma config FOSC = XT        // Oscillator Selection bits (XT oscillator)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOREN = ON       // Brown-out Reset Enable bit (BOR enabled)
#pragma config LVP = OFF        // Low-Voltage (Single-Supply) In-Circuit Serial Programming Enable bit (RB3 is digital I/O, HV on MCLR must be used for programming)
#pragma config CPD = OFF        // Data EEPROM Memory Code Protection bit (Data EEPROM code protection off)
#pragma config WRT = OFF        // Flash Program Memory Write Enable bits (Write protection off; all program memory may be written to by EECON control)
#pragma config CP = OFF         // Flash Program Memory Code Protection bit (Code protection off)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <xc.h>
#include "pic16F877AConfig1.h"

#define _XTAL_FREQ 20000000
#include <xc.h>

void main(void) {
    //TRISB = 0b00001000; pin RB3 como entrada
    //TRISC = 0b01111111; pin RC7 como salida
    TRISB = 0x08;
    TRISC = 0x7f;
    
    PORTCbits.RC7 = 0;
    
    if(PORTBbits.RB3 == 1){
        PORTCbits.RC7 = 1;
        
    }
    
    
    return;
}






