#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>   // Para itoa()

// ------------------------------
// Definiciones del LCD
// ------------------------------
#define LCD_PORT   PORTD
#define LCD_DDR    DDRD
#define RS         PB0
#define EN         PB1

// ------------------------------
// Prototipos
// ------------------------------
void lcd_cmd(uint8_t cmd);
void lcd_data(uint8_t data);
void lcd_init(void);
void lcd_clear(void);
void lcd_gotoxy(uint8_t x, uint8_t y);
void lcd_print(char *str);

uint16_t adc_read(void);
void adc_init(void);

// ------------------------------
// Función principal
// ------------------------------
int main(void)
{
    // LCD en PORTD
    LCD_DDR = 0xFF;  

    // Pines de control RS y EN
    DDRB |= (1 << RS) | (1 << EN);

    // Inicializar LCD y ADC
    lcd_init();
    adc_init();

    char buffer[16];

    while(1)
    {
        uint16_t raw = adc_read();
        uint32_t mv = (raw * 5000UL) / 1023;   // Conversión a mV

        lcd_clear();
        lcd_gotoxy(0,0);
        lcd_print("raw: ");
        itoa(raw, buffer, 10);
        lcd_print(buffer);

        lcd_gotoxy(0,1);
        lcd_print("mV: ");
        itoa(mv, buffer, 10);
        lcd_print(buffer);

        _delay_ms(300);
    }
}

// ------------------------------
// Implementación LCD
// ------------------------------

void lcd_cmd(uint8_t cmd)
{
    LCD_PORT = cmd;
    PORTB &= ~(1 << RS);   // RS = 0 → comando

    PORTB |= (1 << EN);
    _delay_us(1);
    PORTB &= ~(1 << EN);
    _delay_ms(2);
}

void lcd_data(uint8_t data)
{
    LCD_PORT = data;
    PORTB |= (1 << RS);    // RS = 1 → datos

    PORTB |= (1 << EN);
    _delay_us(1);
    PORTB &= ~(1 << EN);
    _delay_ms(2);
}

void lcd_init(void)
{
    _delay_ms(20);
    lcd_cmd(0x38); // 8 bits, 2 líneas
    lcd_cmd(0x0C); // Display ON, cursor OFF
    lcd_cmd(0x06); // Auto-incremento
    lcd_cmd(0x01); // Clear
    _delay_ms(2);
}

void lcd_clear(void)
{
    lcd_cmd(0x01);
    _delay_ms(2);
}

void lcd_gotoxy(uint8_t x, uint8_t y)
{
    uint8_t addr = (y == 0) ? 0x80 + x : 0xC0 + x;
    lcd_cmd(addr);
}

void lcd_print(char *str)
{
    while(*str)
        lcd_data(*str++);
}

// ------------------------------
// ADC
// ------------------------------

void adc_init(void)
{
	DDRC &= ~(1<<PC0);  // aseguramos entrada
	PORTC &= ~(1<<PC0); // aseguramos pull-up OFF
    	ADMUX = (1 << REFS0); // Referencia AVcc, canal ADC0
    	ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1); // Prescaler 64
}

uint16_t adc_read(void)
{
    ADCSRA |= (1 << ADSC);          // Start conversion
    while(ADCSRA & (1 << ADSC));    // Esperar fin
    return ADC;
}

