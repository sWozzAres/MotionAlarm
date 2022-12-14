// RgbLed.h

#ifndef _RGBLED_h
#define _RGBLED_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "Timing.h"

enum class RgbColour {
    Red,
    Green,
    Blue
};
class RgbLed {
private:
    uint8_t _redPin;
    uint8_t _bluePin;
    uint8_t _greenPin;
    int _redVal;
    int _greenVal;
    int _blueVal;

    uint8_t _flashPin;
    unsigned long _flashStartMs;
    bool _flashing{ false };

public:
    RgbLed() {}
    void Initialize(uint8_t redPin, uint8_t greenPin, uint8_t bluePin) {
        _redPin = redPin;
        _greenPin = greenPin;
        _bluePin = bluePin;
        pinMode(_redPin, OUTPUT);
        pinMode(_greenPin, OUTPUT);
        pinMode(_bluePin, OUTPUT);
        ChangeColour(255, 255, 255);
    }

    void ChangeFlashColour(RgbColour colour) {
        analogWrite(_flashPin, 255);
        _flashPin = PinFromColour(colour);
    }

    void StartFlashing(RgbColour colour, unsigned long frameTimeMs) {
        ChangeColour(255, 255, 255);
        _flashPin = PinFromColour(colour);
        _flashStartMs = frameTimeMs;
        _flashing = true;
    }

    void StopFlashing() {
        if (_flashing) {
            analogWrite(_flashPin, 255);
            _flashing = false;
        }
    }

    void Update(unsigned long frameTimeMs) {
        if (!_flashing)
            return;

        constexpr int CYCLETIME = 1000;
        constexpr double HALFCYCLE = CYCLETIME / 2.0;

        auto cycleMs = elapsedMs(_flashStartMs, frameTimeMs) % CYCLETIME; // 0 - 999

        auto colourValue = (cycleMs / HALFCYCLE) * 256.0;
        if (cycleMs > HALFCYCLE)
            colourValue = 512 - colourValue;
        analogWrite(_flashPin, 255 - max(min(colourValue, 255), 0));

        /*constexpr double ratio = 256 / HALFCYCLE;
        uint8_t colourValue = cycleMs < HALFCYCLE ? cycleMs * ratio : (CYCLETIME - 1 - cycleMs) * ratio;
        analogWrite(_flashPin, 255 - colourValue);*/
    }
    void ChangeColour(int redVal, int greenVal, int blueVal) {
        _redVal = redVal;
        _greenVal = greenVal;
        _blueVal = blueVal;
        WritePinValues();
    }
private:
    uint8_t PinFromColour(RgbColour colour) {
        switch (colour) {
        case RgbColour::Red:
            return _redPin;
        case RgbColour::Green:
            return _greenPin;
        case RgbColour::Blue:
            return _bluePin;
        }
    }
    
    void WritePinValues() {
        analogWrite(_redPin, _redVal);
        analogWrite(_greenPin, _greenVal);
        analogWrite(_bluePin, _blueVal);
    }
};
#endif

