// C standard library #includes:
#include <stdio.h>

// Application-specific #includes:
#include "NU32.h"       // constants, funcs for startup and UART
#include "stepper.h"	// STR8 stepper motor library

#define MAX_MESSAGE_LENGTH 200

const char menu_str[] =	"\
\t0: clear alarm(s) and fault(s)\r\n\
\t1: enable/disable motor\r\n\
\t2: enable/disable ramping\r\n\
\t3: move (absolute)\r\n\
\t4: move (relative)\r\n\
\t5: query step count\r\n\
\t6: reset step count\r\n\
\t7: query coasting speed\r\n\
\t8: set coasting speed\r\n\
\t9: reset coasting speed\r\n";

int main(void) {
	char message[MAX_MESSAGE_LENGTH]; // for serial I/O
	int32_t num_steps = 0; // for relative moves
	// uint32_t step_num = 0; // for absolute moves (TODO: implement absolute moves)
	uint16_t steps_per_sec = COAST_SPEED_DEFAULT; // for setting coasting speed

  
	NU32_Startup(); // cache on, interrupts on, LED/button init, UART init
	NU32_WriteUART3("Initialized NU32.\r\n");
	
	init_STR8_interface();
	NU32_WriteUART3("Initialized STR8 stepper motor interface.\r\n");

	while (1) {
		NU32_WriteUART3("\nEnter a single-digit menu option:\r\n");
		NU32_WriteUART3(menu_str);
		NU32_ReadUART3(message, MAX_MESSAGE_LENGTH);// get message from computer

		NU32_WriteUART3("Received ");
		NU32_WriteUART3(message);                   // send message back
		NU32_WriteUART3(" from user.\r\n");			// carriage return and newline
			
		switch (message[0]) {
			case '0': { // clear alarm(s) and fault(s)
				clear_STR8();
				NU32_WriteUART3("Cleared alarm(s) and/or fault(s).\r\n");
				break;
			}
			case '1': { // enable/disable motor
				NU32_WriteUART3("Enter 1 to enable motor, 0 to disable motor:\r\n");
				NU32_ReadUART3(message, MAX_MESSAGE_LENGTH);
	
				NU32_WriteUART3("Received ");
				NU32_WriteUART3(message);
				NU32_WriteUART3(" from user.\r\n");

				switch (message[0]) {
					case '0': {
						enable_motor(DISABLED); // disable motor
						NU32_WriteUART3("Motor disabled.\r\n");
						break;
					}
					case '1': {
						enable_motor(ENABLED); // enable motor
						NU32_WriteUART3("Motor enabled.\r\n");
						break;
					}
					default: {
						NU32_WriteUART3("Unrecognized option.\r\n");
					}
				}
				break;
			}
			case '2': { // enable/disable ramping
				NU32_WriteUART3("Enter 1 to enable ramping, 0 to disable ramping:\r\n");
				NU32_ReadUART3(message, MAX_MESSAGE_LENGTH);
	
				NU32_WriteUART3("Received ");
				NU32_WriteUART3(message);
				NU32_WriteUART3(" from user.\r\n");

				switch (message[0]) {
					case '0': {
						enable_ramp(0); // disable speed ramp up/down
						NU32_WriteUART3("Ramping disabled.\r\n");
						break;
					}
					case '1': {
						enable_ramp(1); // enable speed ramp up/down
						NU32_WriteUART3("Ramping enabled.\r\n");
						break;
					}
					default: {
						NU32_WriteUART3("Unrecognized option.\r\n");
					}
				}
				break;
			}
			case '3': { // TODO: move (absolute)
				break;
			}
			case '4': { // move (relative)
				NU32_WriteUART3("Enter number of steps (-32768 to 32767):\r\n");
				NU32_ReadUART3(message,MAX_MESSAGE_LENGTH);
				sscanf(message,"%d",&num_steps);
				sprintf(message,"Received %d steps.\r\n",num_steps);			
				NU32_WriteUART3(message);
				move_rel(num_steps);
				NU32_WriteUART3("Move complete.\r\n");
				break;
			}
			case '5': { // query step count
				sprintf(message,"step count = %d.\r\n",query_step_count());
				NU32_WriteUART3(message);
				break;
			}
			case '6': { // reset step count
				reset_step_count();
				NU32_WriteUART3("Reset step count to 0.\r\n");
				break;
			}
			case '7': { // query coasting speed
				sprintf(message,"Coast speed = %d steps per second.\r\n",query_coast_speed());
				NU32_WriteUART3(message);
				break;
			}
			case '8': { // set coasting speed
				NU32_WriteUART3("Enter an unsigned integer coasting speed (ex: 200) in steps per second:\r\n");
				NU32_ReadUART3(message,MAX_MESSAGE_LENGTH);
				sscanf(message,"%hu",&steps_per_sec);
				set_coast_speed(steps_per_sec);
				sprintf(message,"Set coasting speed to %hu steps per second.\r\n",query_coast_speed());
				NU32_WriteUART3(message);
				break;
			}
			default: { // Unrecognized option, handle error
				NU32_WriteUART3("Unrecognized option.\r\n");
				break;
			}
		}
		
		NU32_LED1 = !NU32_LED1;                       // toggle the LEDs
		NU32_LED2 = !NU32_LED2;
	}
	return 0;
}
