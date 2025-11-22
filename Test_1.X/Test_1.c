/*
 * File:   Test_1.c
 * Author: daniel
 *
 * Created on October 29, 2025, 7:48 AM
 */


#define _XTAL_FREQ 20000000
#include <xc.h>


void main(void) {
    
    TRISB = 0b00000000;
    TRISA = 0xFF;
    
    
    PORTB = 0b00000000;
    
    
    if(PORTAbits.RA0 == 1){
    
        //0
        PORTB = 0b01111110;
        __delay_ms(1000);
        
        
        //1
        PORTB = 0b00110000;
        __delay_ms(1000);
        
        
        //2
        PORTB = 0b01101101;
        __delay_ms(1000);
        
        
        //3
        PORTB = 0b01111001;
        __delay_ms(1000);
        
        
        //4
        PORTB = 0b00110011;
        __delay_ms(1000);
        
        
        //5
        PORTB = 0b01011011;
        __delay_ms(1000);
        
        
        //6
        PORTB = 0b01011111;
        __delay_ms(1000);
        
        
        //7
        PORTB = 0b01110000;
        __delay_ms(1000);
        
        
        // 8
        PORTB = 0b01111111;
        __delay_ms(1000);
        
        
        //9
        PORTB = 0b01111011;
        __delay_ms(1000);
        }
    
    
    
    return;
}
