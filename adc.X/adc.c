/*
 * File:   adc.c
 * Author: daniel
 *
 * Created on November 5, 2025, 1:32 AM
 */


#include <xc.h>
#define _XTAL_FREQ 20000000 //20MHz
#define RS PORTBbits.RB6
#define EN PORTBbits.RB7
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

void adc_init(void){
    TRISA = 0x01; //Pin AM0 - A0 entrada
    
    
    /*
     *ADCS2 = Combinar con el ADCON0 para el reloj
     * PCFG = 1110 Seleccion del canal
     */
    ADCON1 = 0xce; //0b11001110;
    
    /*
     * bit 0     AD0N = 1 -Enciende el adc
     * bit 2     Godone = 0  
     * bit 3-5   seleccion del canal 0 
     * bit 6-7   110
     */

    ADCON0 = 0b10000001;
    __delay_us(20);
    
    
}


//Leer el ADC
// unsigned int es de 16 bits 
unsigned int adc_read_an0(void){
    __delay_us(20);
    GO_nDONE = 1;
    
    while(GO_nDONE);
    return ((unsigned int)ADRESH<<8|ADRESL);



}



  void lcdCommand (unsigned char cmd){
        RS = 0;
        PORTD = cmd;
        EN = 1;
        __delay_ms(1);
        EN = 0;
        __delay_ms(1);
        
    }
    
    
     void lcdSendDataByte (unsigned char data){
        RS = 1;
        PORTD = data;
        EN = 1;
        __delay_ms(1);
        EN = 0;
        __delay_ms(1);
        
    }
     
     
     
    void lcd_init(){
        lcdCommand(0x38);  // COnfigura el display: 2 lineas matrix6x7
        lcdCommand(0x06);  // Mover cursor a la derecha
        lcdCommand(0x0f);  // Colocar el display en modo on y blinking
        lcdCommand(0x01);  // Borrar Pantalla
        lcdCommand(0x80);  // Mueve el cursor a la primera linea
        __delay_ms(1);
    } 
     
    void lcdSendString(char *addr){
        while(*addr){
            lcdSendDataByte(*addr);
            addr++;
        }
        
    }
  
    
  
    



void main(void) {
    
    TRISD = 0b00000000; // Salida micro/entrada lcd
    TRISB = 0b00000000; // Salida micro/entrada lcd
    
    lcd_init();
    adc_init();
    
    char buffer[16];
    uint16_t prev_mv = 0xff;
    
     while(1){
        uint16_t raw = adc_read_an0();
        uint16_t mV = (uint16_t)raw*5000L/1023U;
    
        if(mV != prev_mv){
            lcdCommand(0x80);
            sprintf(buffer, "RAW:%4u", raw);
            lcdSendString(buffer);
           
             lcdCommand(0xc0);
            sprintf(buffer, "mV:%4u", mV);
            lcdSendString(buffer);
            
            prev_mv = mV;
        }
        __delay_ms(100);
        
        
    }
    
    
    
    
    
    
    return;
}
