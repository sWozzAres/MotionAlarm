// Config.h

#ifndef _CONFIG_h
#define _CONFIG_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

#include <EEPROM.h>

constexpr int EEPROM_CURRENT_ALARM = 0;

struct Config {
public:
	int CurrentAlarm{0};

	void Load() {
		CurrentAlarm = EEPROM.read(EEPROM_CURRENT_ALARM);
	}
	void Save() {
		EEPROM.update(EEPROM_CURRENT_ALARM, CurrentAlarm);
	}
};
#endif

