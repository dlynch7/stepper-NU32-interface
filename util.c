#include "util.h"

// Simple blocking delay function:
void simple_delay_us(uint32_t t_us) {
	uint32_t count = t_us*(NU32_SYS_FREQ/2E6); // convert microseconds to core timer tick count
	_CP0_SET_COUNT(0); // RESET the core timer
	while (_CP0_GET_COUNT() < count) {;} // blocking delay
}
