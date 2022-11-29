
/******************************************************************************
 *      Software interface for STR8 Stepper Motor Driver                      *
 *      from Applied Motion Products                                          *
 *                                                                            *
 *      https://www.applied-motion.com/products/stepper-drives/str8           *
 ******************************************************************************/

#include "stepper.h"

#include "util.h"

#include <stdio.h>

/******************************************************************************
 *      QUASI-PRIVATE VARIABLES                                               *
 *****************************************************************************/
static volatile int32_t StepCount = 0; // number of steps taken by motor
static volatile uint16_t CoastSpeed = COAST_SPEED_DEFAULT; // steps per second when coasting
static volatile uint8_t RampEnabled = 0; // enable/disable speed ramp up/down

/******************************************************************************
 *		QUASI-PUBLIC VARIABLES												  *
 *****************************************************************************/

/******************************************************************************
 *		QUASI-PRIVATE FUNCTIONS												  *
 *****************************************************************************/

/******************************************************************************
 *		QUASI-PUBLIC FUNCTIONS												  *
 ******************************************************************************/
// Clear STR8 alarm(s) and fault(s):
void clear_STR8(void) {
	// From the datasheet: activating then de-activating the EN input clears
	// alarms and faults, and re-enables the motor in the case of drive faults.
	EN_PIN = DISABLED;
	simple_delay_us(1E3); // delay for 1 millisecond (1e3 microseconds)
	EN_PIN = ENABLED;
}

// Enable/disable motor:
void enable_motor(uint8_t enable) {
	EN_PIN = enable;
}

// Enable/disable ramping:
void enable_ramp(uint8_t enable) {
	RampEnabled = enable;
}

// Initialize STR8 stepper motor interface:
void init_STR8_interface(void) {
	// Configure GPIO pins as outputs:
	TRISEbits.TRISE4 = 0;	// STEP_PIN is an output
	TRISEbits.TRISE5 = 0; 	// DIR_PIN  is an output
	TRISEbits.TRISE6 = 0; 	// EN_PIN   is an output

	// Configure GPIO output pins as open-drain outputs;
	// this step is necessary because the STR8 operates on 5-24 V logic,
	// whereas the GPIO pins on the NU32 operate on 3.3 V logic;
	// NOTE: ensure each pin is 5 V tolerant.
	// NOTE: pull-up resistors (R > 500 Ohms) are required.
	ODCEbits.ODCE4 = 1; 	// STEP_PIN is an open-drain output
	ODCEbits.ODCE5 = 1; 	// DIR_PIN  is an open-drain output
	ODCEbits.ODCE6 = 1; 	// EN_PIN   is an open-drain output

	// Set initial values of GPIO pins:
	enable_motor(DISABLED);
	STEP_PIN = 0;
	DIR_PIN = 0;

	reset_step_count();

	set_coast_speed(COAST_SPEED_DEFAULT);

	// enable_motor(ENABLED);
}

// Move (relative):
void move_rel(int32_t num_steps) {
	// use sign of num_steps to determine direction:
	uint8_t direction = 0;		// FWD
	int8_t increment = 1;
	if (num_steps < 0) {
		num_steps = -num_steps; // keep it positive
		direction = 1; 			// REV
		increment = -1;
	}
	
	// execute the move:
	uint32_t i = 0;
	float step_speed = ((float) CoastSpeed);
	float max_speed = step_speed; // initialize, value might change if ramping
	uint32_t t_delay_us = ((uint32_t) (1e6/step_speed));
	DIR_PIN = direction;
	for (i = 0; i < num_steps; i++) {
		// Compute speed for each step:
		if (RampEnabled) { 								// ramping enabled
			// Determine if coast speed can be reached,
			// given commanded number of steps:
			if (num_steps > (2*RAMP_COUNT)) {			// coast speed reachable
				max_speed = SPEED_MIN + RAMP_COUNT*(CoastSpeed - SPEED_MIN)/((float) RAMP_COUNT);
				if (i < RAMP_COUNT) { 					// ramp-up phase
					step_speed = SPEED_MIN \
							   + i*(max_speed - SPEED_MIN)/((float) RAMP_COUNT);
					// step_speed = SPEED_MIN;					
				}
				else if ((num_steps - i) < RAMP_COUNT) {// ramp-down phase
					step_speed = SPEED_MIN \
							   + (num_steps - i)*(CoastSpeed - SPEED_MIN)/((float) RAMP_COUNT);
					// step_speed = SPEED_MIN;
				}
				else { 									// coast phase
					step_speed = CoastSpeed;
				}
			}
			else { 										// coast speed not reachable
				max_speed = SPEED_MIN + (num_steps/2)*(CoastSpeed - SPEED_MIN)/((float) RAMP_COUNT);
				if (i < (num_steps/2))	{ 				// ramp-up phase
					step_speed = SPEED_MIN \
							   + i*(max_speed - SPEED_MIN)/((float) RAMP_COUNT);
					// step_speed = SPEED_MIN;
				}
				else { 									// ramp-down phase
					step_speed = SPEED_MIN \
							   + (num_steps - i)*(max_speed - SPEED_MIN)/((float) RAMP_COUNT);
					// step_speed = SPEED_MIN;
				}
			}
		}
		else { 											// ramping disabled
			step_speed = CoastSpeed;
		}

		// Convert step speed to delay time:
		t_delay_us = ((uint32_t) (1e6/step_speed));

		// Take step and wait:
		STEP_PIN = !STEP_PIN;	// toggle STEP_PIN to step motor once
		StepCount += increment; // increment or decrement StepCount
		simple_delay_us(t_delay_us);
	}

	// char msg[100];
	// sprintf(msg,"max_speed: %f.\r\n",max_speed);
	// NU32_WriteUART3(msg);
}

// Move (absolute):
void move_abs(uint32_t step_num) {
	// TODO
}

// Query step count:
int32_t query_step_count(void) {
	return StepCount;
}

// Reset the step count to zero:
void reset_step_count(void) {
	StepCount = 0;
}

// Query coasting speed (number of steps per second):
uint16_t query_coast_speed(void) {
	return CoastSpeed;
}

// Set coasting speed (number of steps per second):
void set_coast_speed(uint16_t steps_per_sec) {
	if (steps_per_sec > SPEED_MIN) {
		CoastSpeed = steps_per_sec;
	}
	else {
		CoastSpeed = SPEED_MIN;
	}
}
