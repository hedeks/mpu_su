#define F_CPU 1000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>

#define RS 7
#define E 6
#define asterisk 10 // '*'
#define sharp 11 // '#'
#define nkey 12 // no key

/*
	Purpose: write command to command`s regs LCD
	Params: cmd - code of command
*/
void lcd_cmd(uint8_t cmd){
	DDRC = 0xFF;				// all bits PORTC to output
	DDRD |= ((1<<E) | (1<<RS)); // bits E and RS PORTD to output
	PORTD &= ~(1<<RS);			// RS=0
	PORTC = cmd;				// write command to PORTC
	PORTD |= (1<<E);			// form strobe signal
	_delay_ms(5);				// delay for strobe
	PORTD &= ~(1<<E);			// E=0;
	_delay_ms(100);				// delay for ending command
}

/* 
	Purpose: initialization of LCD
*/
void lcd_init(void){
	DDRC = 0xFF;				// all bits PORTC to output
	DDRD |= ((1<<E) | (1<<RS)); // bits E and RS PORTD to output
	_delay_ms(100);				// delay for supply voltage
	lcd_cmd(0x30);				//			
	lcd_cmd(0x30);				//  RS = 0, R/W = 0
	lcd_cmd(0x30);				//		
	lcd_cmd(0x38);				//	DL = 1, N = 1, F = 0
	lcd_cmd(0x0E);				//	D = 1, C = 1, B = 0
	lcd_cmd(0x06);				//	I/D = 1, S = 1;
	lcd_cmd(0x01);				//	clear LCD and cursor to 0
}
/* 
	Purpose: sends data to LCD
	Params: data - ASCII code of letter, digit, symbol
*/
void lcd_data(uint8_t data){
	DDRC = 0xFF;				// PORTC bits to output
	DDRD |= ((1<<E) | (1<<RS)); // PORTD E and RS to output
	PORTD |= (1<<RS);			// RS=1
	PORTC = data;				// data to PORTC
	PORTD |= (1<<E);			// E=1 and RS=1
	_delay_us(5);				// delay for strobe
	PORTD &= ~(1<<E);			// transmit command
	_delay_ms(100);				// delay for micro operations
}

/* 
	Purpose: register which key is pressed
*/
uint8_t scan_keypad(void){
	uint8_t keys[4][3] = {{1,2,3},{4,5,6},{7,8,9},{asterisk, 0, sharp}};
		
	for (uint8_t i = 0; i <= 3; i++){
		PORTA = (1<<i);
		_delay_ms(20);
		for (uint8_t j = 0; j <= 2; j++){
			if ((PIND & (1<<j)) != 0){ // if some voltage on j pin of PORTD
				_delay_ms(25);
				if ((PIND & (1<<j)) != 0){  // if some voltage on j pin of PORTD after delay
					return keys[i][j];
				}
			}
		}
	}
	return nkey;
}

/* 
	Purpose: prints key to LCD
	Params: key - element of array ->
	keys[4][3] = {{1,2,3},{4,5,6},{7,8,9},{asterisk, 0, sharp}};
*/

void print_key(uint8_t key){
	if (key == asterisk){
		lcd_data('*');
	} else if (key == sharp){
		lcd_data('#');
	} else {
		lcd_data(key + '0');
	}
}

int main(void)
{
	uint8_t key;
	uint8_t input[2];
	
	DDRA = 0x0F;
	lcd_init();
	
    for (;;){
		lcd_cmd(1);
		
		// we are waiting first digit
		do 
		{
			key = scan_keypad();
		} while ((key == nkey) || (key == asterisk) || (key == sharp));
		print_key(key);
		input[0] = key;
		while (scan_keypad() != nkey);
		
		// we are waiting asterisk
		do
		{
			key = scan_keypad();
		} while ((key == nkey) || (key != asterisk));
		while (scan_keypad() != nkey);
		
		lcd_data('&');
		
		// we are waiting second digit
		do
		{
			key = scan_keypad();
		} while ((key == nkey) || (key == asterisk) || (key == sharp));
		print_key(key);
		input[1] = key;
		while (scan_keypad() != nkey);
		
		// we are waiting sharp
		do
		{
			key = scan_keypad();
		} while ((key == nkey) || (key != sharp));
		while (scan_keypad() != nkey);
		
		uint8_t res = input[0] & input[1];
		
		lcd_data('=');	
		lcd_data(res + '0');
		
		// waiting for any click
		do
		{
			key = scan_keypad();
		} while (key == nkey);
	}
}
