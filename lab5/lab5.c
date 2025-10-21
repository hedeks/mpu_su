#define F_CPU 11059200UL

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>

#define RS 7
#define E 6

/*
	Purpose: write command to command`s regs LCD
	Params: cmd - code of command
*/
void lcd_cmd(uint8_t cmd){
	DDRC = 0xFF;				// all bits PORTC to output
	DDRD |= ((1<<E) | (1<<RS));		// bits E and RS PORTD to output
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
	DDRD |= ((1<<E) | (1<<RS)); 		// bits E and RS PORTD to output
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
	DDRD |= ((1<<E) | (1<<RS)); 		// PORTD E and RS to output
	PORTD |= (1<<RS);			// RS=1
	PORTC = data;				// data to PORTC
	PORTD |= (1<<E);			// E=1 and RS=1
	_delay_us(5);				// delay for strobe
	PORTD &= ~(1<<E);			// transmit command
	_delay_ms(100);				// delay for micro operations
}


/*
	Purpose: Implement Euler method for digital control system
	Params: 
		f = -4y + 2u - 5
		T = 2
		y(0) = 0
		u(k): 3,7,11,15,19 for k=1..5
		y(k+1) = y(k) + T * f(y(k),u(k));
	Output: "Result: Y(k) = X" with 3s delay
	LCD: even k - line 1, odd k - line 2 -> my variant has odd u(k)
*/

int main(void)
{
	lcd_init();
	
    for (;;){
		uint8_t y = 0;
		uint8_t T = 2;
		uint8_t u[] = {3,7,11,15,19};
			
		// 5 iterations
		for (uint8_t k = 0; k <= 5; k++){	
			
			if (k == 0){
				y = 0;
			} else {
				y = y + T * (-4*y + 2*u[k-1] - 5); // recursive function	
			}
		
			char data[30];
			sprintf(data, "Y(%d) = %d", k, y);
			
			lcd_cmd(1<<7 | 64);
			uint8_t i = 0;
			while (data[i] != '\0'){
				lcd_data(data[i]);
				i++;
			}
			
			_delay_ms(3000); // delay before next iteration
			lcd_cmd(1);
		}
		
	}
}

