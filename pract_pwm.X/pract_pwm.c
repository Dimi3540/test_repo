/*
 * File:   pract_pwm.c
 * Author: daniel
 *
 * Created on November 25, 2025, 7:29 PM
 */


#include <xc.h>
#define _XTAL_FREQ 20000000

void pwm_init(void){
    TRISCbits.TRISC2 = 0; // CCP1 PWM output
    CCP1CON = 0b00001100; // PWM mode

    PR2 = 249;       // 1 kHz con TMR2 prescaler 4
    T2CON = 0b00000101; // TMR2 ON, prescaler 4
    
    CCPR1L = 0;
}

void set_percent(unsigned char percent){
    if(percent > 100) percent = 100;
    
    unsigned int max_counts = 4 * (PR2 + 1);
    unsigned long dc = ((unsigned long)percent * max_counts) / 100;

    CCPR1L = (unsigned char)(dc >> 2);
    CCP1CONbits.CCP1X = (dc >> 1) & 1;
    CCP1CONbits.CCP1Y = dc & 1;
}


void main(void){
    TRISB = 0xFF;
    TRISA = 0x00;

    pwm_init();
    unsigned char velocity = 0;

    while(1){
        if(PORTBbits.RB1){ //aumentar pwm
            if(velocity < 100) velocity++;
            set_percent(velocity);
            __delay_ms(20);
        }
        if(PORTBbits.RB2){ //disminuir pwm
            if(velocity > 0) velocity--;
            set_percent(velocity);
            __delay_ms(20);
        }
        
        if(PORTBbits.RB3){
            PORTA = 0x01;
        }
        
        if(PORTBbits.RB4){
            PORTA = 0x02;
        }
        
        if(PORTBbits.RB5){ //apagar motor y velocity 0
            PORTA = 0x00;
            set_percent(0);
        }
    }
}

