// Blinker.h

#ifndef _BLINKER_h
#define _BLINKER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

class Blinker {
private:
    const unsigned long* _durations{ nullptr };
    unsigned int _count;
    int _index;
    unsigned long _startMs;
    unsigned long _durationMs;
    double _speedMultiplier;
    uint8_t _pin;
    uint8_t _state;
    bool _paused = false;
public:
    Blinker() {}
    void Begin(uint8_t pin, uint8_t startVal) {
        _pin = pin;
        _state = startVal;
        pinMode(_pin, OUTPUT);
    };

    void Start(const unsigned long* durations, unsigned int count, unsigned int speedPercent = 100) {
        _durations = durations;
        _count = count;
        _speedMultiplier = 100.0 / speedPercent;
        _index = -1;
        _startMs = 0;
        _durationMs = 0;
        digitalWrite(_pin, _state);
        _paused = false;
    }

    void Pause() {
        digitalWrite(_pin, LOW);
        _paused = true;
    }
    void Resume() {
        if (_durations != nullptr)
            _paused = false;
    }

    void Update(unsigned long frameTimeMs) {
        if (_paused) 
            return;

        if (ElapsedMs(_startMs, frameTimeMs) >= _durationMs) {
            if (++_index == _count)
                _index = 0;

            _state = _state == HIGH ? LOW : HIGH;
            digitalWrite(_pin, _state);

            _durationMs = _durations[_index] * _speedMultiplier;
            _startMs = frameTimeMs;
        }
    }
private:
    // Returns the elapsed time between 'start' and 'end' with handling of overflow.
    inline unsigned long ElapsedMs(unsigned long start, unsigned long end) const {
        auto diff = end - start;
        return diff >= 0 ? diff : (0xffffffff - start) + end;
    }
};
#endif

