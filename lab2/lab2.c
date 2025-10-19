#define F_CPU 1159200UL
#include <avr/io.h>
#include <util/delay.h>
#define DELAY 3

/*
	Purpose: column selection codes for 8x8 matrix decoder
	Bits [7:4] of PORTD control 4-to-16 decoder
	Each value selects one column (0-7) from left to right
*/
uint8_t column[8] = {
	0b00000000, /* column 1 - decoder input: 0000 */
	0b00010000, /* column 2 - decoder input: 0001 */
	0b00100000, /* column 3 - decoder input: 0010 */
	0b00110000, /* column 4 - decoder input: 0011 */
	0b01000000, /* column 5 - decoder input: 0100 */
	0b01010000, /* column 6 - decoder input: 0101 */
	0b01100000, /* column 7 - decoder input: 0110 */
	0b01110000, /* column 8 - decoder input: 0111 */
};

/*
	Purpose: display matrix pattern on LED matrix with scanning
	Params:
		matrix - 8x8 array representing pixel pattern (1=on, 0=off)
		mode - display mode (1=normal, 2=inverted)
	Operation:
		- Sequentially activates each column
		- For each column, sets row data on PORTA
		- Uses persistence of vision to create stable image
*/
void light_from_matrix(uint8_t (*matrix)[8], int mode){
	int it;
	
	if (mode == 2){ 
		it = 200;	// fewer iterations for inverted mode (shorter display time)
	} else {
		it = 800;	// more iterations for normal mode (longer display time)
	}
		
	for (int k = 0; k < it; k++){			// repeat for persistence of vision
		int column_lights = 0;
		for (int i = 0; i < 8; i++){		// iterate through columns
			column_lights = 0;
			for (int j = 0; j < 8; j++){	// iterate through rows
				PORTD = column[i];			// select current column via decoder
				if (mode == 1){
					if (matrix[j][i] == 1){ /* light up ones in mode = 1 */
						column_lights |= 1<<j;	// set corresponding row bit
					}
				}
				else if (mode == 2){
					if (matrix[j][i] == 0){ /* light up zeros in mode = 2 (inverted) */
						column_lights |= 1<<j;	// set corresponding row bit
					} 
				}
			}
			PORTA = column_lights;		// output row data for current column
			_delay_ms(10);				// delay for column display time
		}
	}
}

/*
	Purpose: main function to display pattern on LED matrix
	Pattern: diamond shape with border
	Operation:
		- Displays pattern in normal mode
		- Displays pattern in inverted mode
		- Repeats in infinite loop
*/
int main(void)
{
	DDRC = 0xFF;	// PORTC all outputs (unused in this code)
	PORTC = 0;		// clear PORTC
	DDRA = 0xFF;	// PORTA all outputs (row data)
	DDRD = 0xF0;	// PORTD[7:4] outputs, [3:0] inputs (column selection)
	
	while (1)
	{
		/*
		8x8 matrix pattern definition:
		- 1 represents lit LED
		- 0 represents unlit LED
		Pattern creates diamond shape with full border
		*/
		uint8_t matrix [8][8] = {
			{1,1,1,1,1,1,1,1}, // row 1 - full top border
			{0,1,0,0,0,0,1,0}, // row 2
			{0,0,1,0,0,1,0,0}, // row 3
			{0,0,0,1,1,0,0,0}, // row 4 - center
			{0,0,0,1,1,0,0,0}, // row 5 - center
			{0,0,1,0,0,1,0,0}, // row 6
			{0,1,0,0,0,0,1,0}, // row 7
			{1,1,1,1,1,1,1,1}, // row 8 - full bottom border
		};
		light_from_matrix(matrix, 1);	// display pattern in normal mode
		light_from_matrix(matrix, 2);	// display pattern in inverted mode
	}
}
