// MorseCodePlayer.h

#ifndef _MORSECODEPLAYER_h
#define _MORSECODEPLAYER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "MorseCode.h"

class MorseCodePlayer {
private:
	unsigned long startMs{ 0 };
	unsigned long durationMs{ 0 };
	const char* str{ nullptr };
	const char* strPtr{ nullptr };
	bool on{ true };
	const unsigned long DOT_DURATION = 60;
	uint8_t _pin;
public:
	void Initialize(uint8_t pin) {
		_pin = pin;
		pinMode(_pin, OUTPUT);
	};

	void Play(const char* message) {
		str = message;
		strPtr = str;
		startMs = 0;
		durationMs = 0;
		on = true;
	}

	void Update(unsigned long frameTimeMs) {

		static unsigned long durations[10];
		static int size{ 0 };
		static int durationIndex = { size + 1 };

		if (frameTimeMs - startMs >= durationMs) {
			
			startMs = frameTimeMs;

			if (durationIndex > size) {
				if (*strPtr == '\0') {
					return;
				}
				else {
					auto morse = morseCodeFromLetter(*strPtr++);
					auto morsePtr = morse;

					size = -1;

					while (*morsePtr != 0) {
						if (*morsePtr == '.') {
							durations[++size] = DOT_DURATION;
						}
						else {
							durations[++size] = DOT_DURATION * 3;
						}

						morsePtr++;

						// symbol space
						if (*morsePtr != 0)
							durations[++size] = DOT_DURATION;
					}

					durationIndex = 0;

					// letter space
					durations[++size] = DOT_DURATION * 3;
				}
			}

			durationMs = durations[durationIndex++];

			//std::cout << durations[durationIndex++] << '\t' << on << '\n';
			if (on)
				tone(_pin, 550);// , durations[durationIndex++]);
			else
				noTone(_pin);

			on = !on;
		}
	}
};
#endif

