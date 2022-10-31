// Blinker.h

#ifndef _BLINKER_h
#define _BLINKER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "Timing.h"

struct BlinkPattern {
    uint8_t StartState;
    unsigned int Size;
    unsigned long Durations[];
};

class Blinker {
private:
    const BlinkPattern* _pattern;
    int _index;
    unsigned long _startMs;
    unsigned long _durationMs;
    double _speedMultiplier;
    uint8_t _pin;
    uint8_t _state;
    bool _paused = false;
public:
    Blinker() {}
    void Initialize(uint8_t pin) {
        _pin = pin;
        pinMode(_pin, OUTPUT);
    };

    void Start(const BlinkPattern* pattern, unsigned long startTimeMs = millis(), unsigned int speedPercent = 100) {
        _pattern = pattern;
        _speedMultiplier = 100.0 / speedPercent;
        _index = 0;
        _startMs = startTimeMs;
        _durationMs = pattern->Durations[0] * _speedMultiplier;
        _state = pattern->StartState;
        digitalWrite(_pin, _state);
        _paused = false;
    }

    void Pause() {
        digitalWrite(_pin, LOW);
        _paused = true;
    }
    void Resume() {
        if (_pattern != nullptr)
            _paused = false;
    }

    void Update(unsigned long frameTimeMs) {
        if (_paused) 
            return;

        if (elapsedMs(_startMs, frameTimeMs) >= _durationMs) {
            if (++_index == _pattern->Size)
                _index = 0;

            _state = _state == HIGH ? LOW : HIGH;
            digitalWrite(_pin, _state);

            _durationMs = _pattern->Durations[_index] * _speedMultiplier;
            _startMs = frameTimeMs;
        }
    }
};
#endif

