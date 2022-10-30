// Timing.h

#ifndef _TIMING_h
#define _TIMING_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

// Returns the elapsed time between 'start' and 'end' with handling of overflow.
inline unsigned long ElapsedMs(unsigned long start, unsigned long end) {
    auto diff = end - start;
    return diff >= 0 ? diff : (0xffffffff - start) + end;
}
#endif

