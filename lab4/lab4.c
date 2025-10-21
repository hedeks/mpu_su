#define F_CPU 11059200UL
#include <avr/io.h>
#include <util/delay.h>

/*
	Purpose: generate square wave for sound output
	Operation: 
		- Toggles PB4 pin to create alternating signal
		- 5ms delay creates 100Hz tone (period = 10ms)
		- Used for both dot and dash sounds in Morse code
*/
void sound(){
	PORTB ^= (1<<4);		// toggle PB4 pin (buzzer control)
	_delay_ms(5); 			// 100 Hz => T/2 = 1/200 s = 5 ms
}

/*
	Purpose: generate Morse code dot signal
	Operation:
		- Sets RGB LED to cyan color for visual indication
		- Generates short sound (100 cycles = 500ms total)
		- Adds inter-symbol pause after completion
*/
void dot(){
	PORTE = 0b00011000; 	// cyan color (G+B) for dot visualization
	for (uint8_t i = 0; i <= 100; i++){ // 500ms / 5ms = 100 cycles
		sound();
	}
	PORTE = 0; 				// turn off LED
	_delay_ms(500);			// inter-symbol pause
}

/*
	Purpose: generate Morse code dash signal
	Operation:
		- Sets RGB LED to red color for visual indication
		- Generates long sound (300 cycles = 1500ms total)
		- Adds inter-symbol pause after completion
*/
void dash(){
	PORTE = 0b00100000; 	// red color for dash visualization
	for (uint8_t i = 0; i <= 300; i++){ // 1500ms / 5ms = 300 cycles
		sound();
	}
	PORTE = 0; 				// turn off LED
	_delay_ms(500);			// inter-symbol pause
}

/*
	Purpose: Morse code representation of word "ÃÎËÈÊ" (GOLIK)
	Encoding:
		0 = dot (short signal)
		1 = dash (long signal) 
		2 = inter-letter pause (1000ms)
	Word breakdown:
		Г: --.    (1,1,0)
		О: ---    (1,1,1)
		Л: .-..   (0,1,0,0)
		И: ..     (0,0)
		К: -.-    (1,0,1)
*/
uint8_t word[] = {
	/* Г */	1,1,0, 2, 	// dash, dash, dot, pause
	/* О */	1,1,1, 2, 	// dash, dash, dash, pause  
	/* Л */	0,1,0,0, 2, // dot, dash, dot, dot, pause
	/* И */	0,0, 2, 	// dot, dot, pause
	/* К */	1,0,1 		// dash, dot, dash
};

/*
	Purpose: main function for Morse code generation
	Operation:
		- Configures ports for RGB LED and buzzer control
		- Continuously transmits encoded word in Morse code
		- Adds 5-second pause between word repetitions
*/
int main(void)
{
	// Configure PORTE bits 3,4,5 as outputs (RGB LED control)
	DDRE = (1 << 3) | (1 << 4) | (1 << 5);
	// Configure PORTB bit 4 as output (buzzer control)
	DDRB = (1 << 4);
	
	for (;;){ 				// infinite loop
		// Iterate through each element in Morse code array
		for (uint8_t i = 0; i < sizeof(word); i++){
			if (word[i] == 0) {
				dot();		// generate dot signal
			}
			else if (word[i] == 1) {
				dash();		// generate dash signal
			} 
			else if (word[i] == 2) {
				_delay_ms(1000); // inter-letter pause (1000ms)
			}
		}
		_delay_ms(5000); 	// pause between word repetitions (5000ms)
	}

}
