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

class Config {
private:
	int _currentAlarm{0};
public:
	Config() {

	}
	void Load() {
		_currentAlarm = EEPROM.read(EEPROM_CURRENT_ALARM);
	}
	inline int CurrentAlarm() { return _currentAlarm; }
	inline int SetCurrentAlarm(int index) {
		if (_currentAlarm != index)
		{
			_currentAlarm = index;
			EEPROM.update(EEPROM_CURRENT_ALARM, _currentAlarm);
		}
	}
};
#endif

