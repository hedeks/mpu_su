#include <avr/io.h>
#define F_CPU 11059200UL
#include <util/delay.h>

/*
	Purpose: extract digit from 5-digit decimal number
	Params: 
		d - positive decimal number
		m - digit position (1-5, left to right)
	Returns: digit value at specified position
*/

uint8_t digit(uint16_t d, uint8_t m){
  uint8_t i = 5, a;
  while (i){
    a = d%10;				// get last digit
    if (i-- == m) break;	// check if this is the requested position
    d /= 10;				// remove last digit
  }
  return a;
}

/*
	Purpose: 7-segment display codes for digits 0-9
	Segment mapping: 
		7  6  5  4  3  2  1  0
		h  g  f  e  d  c  b  a
*/
uint8_t indicationByte[10] = {
  0b00111111,	// 0 - segments: a,b,c,d,e,f
  0b00000110,	// 1 - segments: b,c
  0b01011011,	// 2 - segments: a,b,d,e,g
  0b01001111,	// 3 - segments: a,b,c,d,g
  0b01100110,	// 4 - segments: b,c,f,g
  0b01101101,	// 5 - segments: a,c,d,f,g
  0b01111101,	// 6 - segments: a,c,d,e,f,g
  0b00000111,	// 7 - segments: a,b,c
  0b01111111,	// 8 - all segments
  0b01101111	// 9 - segments: a,b,c,d,f,g
};

/*
	Purpose: display arithmetic progression on 5-digit 7-segment display
	Progression: x(i+1) = x(i) + d
	Initial values: x0 = 2, d = 3
	Displays: 20 sequence members with 1-second pauses
*/
int main(void){
  DDRA = 0b11111111;		// PORTA all outputs (digit selection)
  DDRC = 0b11111111;		// PORTC all outputs (segment data)
  
  for(;;){					// infinite loop
    int d = 3;				// progression difference
    int x0 = 2;				// initial value
    
    for (int i = 0; i < 20; i++){		// 20 sequence members
      for (int k = 1; k <= 5; k++){	// iterate through 5 digits
        int number = digit(x0, k); 	// extract k-th digit from current number
        PORTC = indicationByte[number];	// output segment pattern
        PORTA = PORTA | 1<<k;			// enable latch for k-th digit
        _delay_us(1);					// short pulse for latch
        PORTA = 0;						// disable all latches
      }
      x0 = x0 + d;			// calculate next sequence member
      _delay_ms(1000);		// 1 second pause between numbers
    }
  }
}
