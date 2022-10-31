// Button.h

#ifndef _BUTTON_h
#define _BUTTON_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "Timing.h"

enum class ButtonState {
    Pressed,
    Released
};


class Button {
private:
    unsigned long _buttonPressedMs{ 0 };
    unsigned long _buttonReleasedMs{ 0 };
    ButtonState _buttonState{ ButtonState::Released };
    uint8_t _pin;
    uint16_t _state = 0;
    unsigned long _lastChangeMs{ 0 };
public:
    Button() {}
    void Initialize(uint8_t pin) {
        _pin = pin;
        pinMode(_pin, INPUT);
    }
    inline ButtonState State() const {
        return _buttonState;
    }
    inline bool IsPressed() const {
        return _buttonState == ButtonState::Pressed;
    }
    void Update(unsigned long frameTimeMs) {
        constexpr int MIN_DETECT_MS = 10;
        _state = (_state << 1) | digitalRead(_pin) | 0xfe00;
        switch (_state & 0xff) {
        case 0xff:
            if (_buttonState != ButtonState::Pressed && elapsedMs(_lastChangeMs, frameTimeMs) > MIN_DETECT_MS) {
                _buttonState = ButtonState::Pressed;
                _buttonPressedMs = frameTimeMs;
                _lastChangeMs = _buttonPressedMs;
            }
            break;
        case 0x00:
            if (_buttonState != ButtonState::Released && elapsedMs(_lastChangeMs, frameTimeMs) > MIN_DETECT_MS) {
                _buttonState = ButtonState::Released;
                _buttonReleasedMs = frameTimeMs;
                _lastChangeMs = _buttonReleasedMs;
            }
            break;
        default:
            // bouncing
            break;
        }
    }
    // When button state is Released, returns time that the button
    // had been pressed (in ms).
    inline unsigned long LastPressedMs() const {
        return _buttonReleasedMs - _buttonPressedMs;
    }
    // When button state is Pressed, returns time that the button
    // had been released (in ms).
    inline unsigned long LastReleasedMs() const {
        return _buttonPressedMs - _buttonReleasedMs;
    }
    inline unsigned long LastChangedMs() const {
        return _lastChangeMs;
    }
};
#endif

