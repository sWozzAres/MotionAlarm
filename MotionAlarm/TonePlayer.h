// TonePlayer.h

#ifndef _TONEPLAYER_h
#define _TONEPLAYER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "Timing.h"

struct Tone {
    unsigned int Frequency;
    unsigned long Duration;
    
};

class TonePlayer {
private:
    Tone* _tones;
    unsigned int _count;
    int _index;
    unsigned long _startMs;
    unsigned long _durationMs;
    double _speedMultiplier;
    uint8_t _pin;
    bool _isPlaying = false;
    bool _repeat;
public:
    TonePlayer() {}
    void Initialize(uint8_t pin) {
        _pin = pin;
        pinMode(_pin, OUTPUT);
    };

    void Play(Tone* tones, unsigned int count, bool repeat = false, unsigned int speedPercent = 100) {
        _tones = tones;
        _count = count;
        _speedMultiplier = 100.0 / speedPercent;
        _index = -1;
        _startMs = 0;
        _durationMs = 0;
        _repeat = repeat;
        _isPlaying = true;
    }

    void Stop() {
        noTone(_pin);
        _isPlaying = false;
    }

    void Update(unsigned long frameTimeMs) {
        if (!_isPlaying)
            return;

        if (elapsedMs(_startMs, frameTimeMs) >= _durationMs) {
            if (++_index == _count) {
                if (_repeat)
                    _index = -1;
                else {
                    noTone(_pin);
                    _isPlaying = false;
                }

            }
            else {
                if (auto freq = _tones[_index].Frequency; freq == 0)
                    noTone(_pin);
                else
                    tone(_pin, freq);

                _durationMs = _tones[_index].Duration * _speedMultiplier;
                _startMs = frameTimeMs;
            }
        }
    }
    bool IsPlaying() const {
        return _isPlaying;
    }
};
#endif

