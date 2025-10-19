#define F_CPU 11059200UL
#include <avr/io.h>
#include <util/delay.h>

/*
	Purpose: define constants for motor control directions and speed parameters
	POSITIVE/NEGATIVE: motor rotation direction
	UP/DOWN: speed profile direction (acceleration/deceleration)
	MIN_SPEED/MAX_SPEED: delay values for speed control (higher delay = slower speed)
	ITERATIONS: number of steps for each speed level
*/
#define POSITIVE 1
#define NEGATIVE 0
#define UP 1
#define DOWN 0
#define MIN_SPEED 6
#define MAX_SPEED 1
#define ITERATIONS 250

/*
	Purpose: half-step phase sequence for stepper motor
	8-step sequence for smoother operation and higher resolution
	Each value represents active phases on PORTC[3:0]
*/
uint8_t phase[8] = {
  0b0001,	// phase 1: coil A active
  0b0011,	// phase 2: coils A and B active
  0b0010,	// phase 3: coil B active
  0b0110,	// phase 4: coils B and C active
  0b0100,	// phase 5: coil C active
  0b1100,	// phase 6: coils C and D active
  0b1000,	// phase 7: coil D active
  0b1001,	// phase 8: coils D and A active
};

/*
	Purpose: custom delay function with millisecond precision
	Params: ms - number of milliseconds to delay
	Note: wraps _delay_ms(1) in a loop for variable timing
*/
void my_delay(uint8_t ms){
  for (int i = 0; i < ms; i++){
    _delay_ms(1);
  }
}

/*
	Purpose: find array index of current phase in phase sequence
	Params: current_phase - current phase pattern (4-bit value)
	Returns: index in phase array (0-7) or -1 if not found
*/
uint8_t phase_index(uint8_t current_phase){
  for (int i = 0; i < 8; i++){
    if (phase[i] == current_phase){
      return i;
    }
  }
  return -1;
}

/*
	Purpose: calculate next phase in sequence based on direction
	Params:
		current_phase - pointer to current phase value
		direction - POSITIVE (forward) or NEGATIVE (reverse)
	Returns: next phase pattern in sequence
	Operation: increments or decrements phase index with wrap-around
*/
uint8_t next_phase(uint8_t* current_phase, int direction){
  uint8_t result = *current_phase;
  int tmp = phase_index(*current_phase);
  
  if (direction == POSITIVE){
    tmp++;					// move to next phase (forward)
    if (tmp > 7){
      tmp = 0;				// wrap around to beginning
    }
    result = phase[tmp];
  } else if (direction == NEGATIVE){
    tmp--;					// move to previous phase (reverse)
    if (tmp < 0){
      tmp = 7;				// wrap around to end
    }
    result = phase[tmp];
  }
  
  return result;
}

/*
	Purpose: control motor rotation with speed profiling
	Params:
		min_speed - minimum speed (highest delay value)
		max_speed - maximum speed (lowest delay value)
		mode - UP (acceleration) or DOWN (deceleration)
		direction - POSITIVE or NEGATIVE rotation
		steps - number of steps (unused in current implementation)
		start_position - pointer to current phase position
	Operation:
		- Gradually changes speed from initial to goal value
		- Executes multiple iterations at each speed level
		- Updates motor phase after each delay
*/
void rotate(int min_speed, int max_speed, int mode, int direction, int steps, uint8_t* start_position){
  int j, goal;
  
  if (mode == POSITIVE){
    j = min_speed;			// start from slowest speed
    goal = max_speed;		// accelerate to fastest speed
  }
  else if (mode == NEGATIVE){
    j = max_speed;			// start from fastest speed
    goal = min_speed;		// decelerate to slowest speed
  }
  
  for (;;){
    for (int i = 0; i <= (7-j) * ITERATIONS; i++){
      *start_position = next_phase(start_position, direction);	// get next phase
      PORTC = *start_position;		// output phase to motor driver
      my_delay(j);				// delay determines rotation speed
    }
    
    if (mode == POSITIVE) {
      j--;					// decrease delay (increase speed)
      if (j <= goal){
        return;				// reached target speed
      }
    } else if (mode == NEGATIVE) {
      j++;					// increase delay (decrease speed)
      if (j >= goal){
        return;				// reached target speed
      }
    }
  }
}

/*
	Purpose: main function for stepper motor control demonstration
	Operation:
		- Initializes PORTC[3:0] as outputs for motor phases
		- Starts from phase 0 position
		- Executes acceleration/deceleration cycles in both directions
		- Repeats pattern indefinitely
*/
int main(void)
{
  DDRC = 0x0F;				// PORTC[3:0] as outputs (motor phases)
  
  uint8_t start_position = phase[0];	// start from first phase
  PORTC = start_position;	// output initial phase
  _delay_ms(1500);			// initial delay before starting
  
  for (;;) {
    // Acceleration and deceleration cycles in positive direction
    rotate(MIN_SPEED, MAX_SPEED, UP, POSITIVE, 5, &start_position);
    rotate(MIN_SPEED, MAX_SPEED, DOWN, POSITIVE, 5, &start_position);
    
    // Acceleration and deceleration cycles in negative direction
    rotate(MIN_SPEED, MAX_SPEED, UP, NEGATIVE, 5, &start_position);
    rotate(MIN_SPEED, MAX_SPEED, DOWN, NEGATIVE, 5, &start_position);
    
    // Repeat positive direction cycle
    rotate(MIN_SPEED, MAX_SPEED, UP, POSITIVE, 5, &start_position); 
    rotate(MIN_SPEED, MAX_SPEED, DOWN, POSITIVE, 5, &start_position);
  }
}
