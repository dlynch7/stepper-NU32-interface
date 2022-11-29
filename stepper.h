/******************************************************************************
 *	Software interface for STR8 Stepper Motor Driver		      *
 *	from Applied Motion Products					      *
 *									      *
 *	https://www.applied-motion.com/products/stepper-drives/str8	      *
 ******************************************************************************/

#ifndef STEPPER__H__
#define STEPPER__H__

#include "NU32.h"
#include <stdint.h>

#define STEP_PIN LATEbits.LATE4 // "step" signal on pin E4 (output pin)
#define DIR_PIN  LATEbits.LATE5 // "direction" signal on pin E5 (output pin)
#define EN_PIN   LATEbits.LATE6 // "enable" signal on pin E6 (output pin)

#define ENABLED  0 // STR8 driver is active-low
#define DISABLED 1

#define COAST_SPEED_DEFAULT 1000 // default coasting speed (steps per second)=
#define SPEED_MIN 500 // minimum speed (steps per second)
#define RAMP_COUNT 500 // number of steps in ramp

// Clear STR8 alarm(s) and fault(s):
void clear_STR8(void);

// Enable/disable motor:
void enable_motor(uint8_t);

// Enable/disable ramping:
void enable_ramp(uint8_t);

// Initialize STR8 stepper motor interface:
void init_STR8_interface(void);

// Move (relative):
void move_rel(int32_t);

// Move (absolute):
void move_abs(uint32_t);

// Query step count:
int32_t query_step_count(void);

// Reset the step count to zero:
void reset_step_count(void);

// Query coasting speed (number of steps per second):
uint16_t query_coast_speed(void);

// Set coasting speed (number of steps per second):
void set_coast_speed(uint16_t);

#endif // STEPPER__H__
