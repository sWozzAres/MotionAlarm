//#include "LowPower.h"
#include "MorseCodePlayer.h"
#include "Timing.h"
#include "Config.h"
#include "TonePlayer.h"
#include "Blinker.h"
#include "RgbLed.h"
#include "Button.h"
#include "MorseCode.h"
#include "MorseCodePlayer.h"

constexpr uint8_t PIN_TESTBUTTON{ 2 };
constexpr uint8_t PIN_PIR{ 3 };
constexpr uint8_t PIN_BLUE{ 9 };
constexpr uint8_t PIN_GREEN{ 10 };
constexpr uint8_t PIN_RED{ 6 };
constexpr uint8_t PIN_ALARM{ 5 };
constexpr uint8_t PIN_MORSE_BUZZER{ 4 };

Tone CloseEncounters[] = {
  { 932, 250 }, { 0, 30 }, { 1047, 250 }, { 0, 30 }, { 831, 250 }, { 0, 200 }, { 415, 500 }, { 0, 100 }, { 622, 2000 }
};
Tone Alarm2[] = {
  { 1000, 500 }, { 600, 500 }
};
Tone Alarm3[] = {
  { 100, 200 }, { 200, 200 }, { 500, 500 }, { 1000, 500 }, { 0, 500 }
};
Tone Alarm4[] = {
  { 1000, 200 }, { 800, 200 }
};
Tone Alarm5[] = {
  { 1200, 200 }, { 0, 200 }
};
Tone Alarm6[] = {
	{ 550,60 }, { 0, 60 },
	{ 550,60 }, { 0, 60 },
	{ 550,60 }, { 0, 60 },
	{ 550,180 }, { 0, 180 },
	{ 550,60 }, { 0, 60 },
	{ 550,60 }, { 0, 60 },
	{ 550,60 }, { 0, 60 },
	{ 550,180 }, { 0, 180 },
	{ 550,60 }, { 0, 60 },
	{ 550,60 }, { 0, 60 },
	{ 550,60 }, { 0, 60 },
	{ 550,180 }, { 0, 420 },

};

struct Alarm {
	Tone* Tones;
	int Size;
};

constexpr Alarm Alarms[] = {
  { CloseEncounters, sizeof(CloseEncounters) / sizeof(Tone) },
  { Alarm2, sizeof(Alarm2) / sizeof(Tone) },
  { Alarm3, sizeof(Alarm3) / sizeof(Tone) },
  { Alarm4, sizeof(Alarm4) / sizeof(Tone) },
  { Alarm5, sizeof(Alarm5) / sizeof(Tone) },
  { Alarm6, sizeof(Alarm6) / sizeof(Tone) },
};

const BlinkPattern Blink1 =
{
	HIGH, 4, { 200, 100, 200, 800 }
};

RgbLed led{};
Blinker blinker{};
TonePlayer tonePlayer{};
Button button{};
Config configuration{};
MorseCodePlayer morsePlayer{};

bool alarm{ false };

volatile unsigned long lastInterruptMs{ 0 };

// Interrupt Service Routines (ISR)
void testButtonChanged() {
	lastInterruptMs = millis();
}

volatile bool motionDetectStarted{ false };
volatile bool motionDetectEnded{ false };
volatile unsigned long motionDetectedMs{ 0 };

void pirStatusChanged() {
	lastInterruptMs = millis();

	if (digitalRead(PIN_PIR) == HIGH) {
		motionDetectStarted = true;
		motionDetectedMs = millis();
	}
	else {
		motionDetectEnded = true;
		motionDetectedMs = 0;
	}
}

void setup() {
	configuration.Load();
	if (configuration.CurrentAlarm > sizeof(Alarms) / sizeof(Alarm))
		configuration.CurrentAlarm = 0;

	tonePlayer.Initialize(PIN_ALARM);
	blinker.Initialize(LED_BUILTIN);
	led.Initialize(PIN_RED, PIN_GREEN, PIN_BLUE);
	button.Initialize(PIN_TESTBUTTON);
	morsePlayer.Initialize(PIN_ALARM);
	
	
	blinker.Start(&Blink1);

	attachInterrupt(digitalPinToInterrupt(PIN_TESTBUTTON), testButtonChanged, CHANGE);

	pinMode(PIN_PIR, INPUT);
	attachInterrupt(digitalPinToInterrupt(PIN_PIR), pirStatusChanged, CHANGE);

	pinMode(PIN_MORSE_BUZZER, OUTPUT);
	morsePlayer.Play("MARK");
	//morsePlayer.Play("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
}

struct ButtonPress {
	ButtonState State;
	unsigned int Duration;
};

//void handleButtonInput2(unsigned long frameTimeMs) {
//	constexpr int BUFFER_SIZE = 100;
//	static ButtonPress inputBuffer[BUFFER_SIZE];
//	static int bufferIndex = 0;
//
//	static ButtonState lastKnownState{ ButtonState::Released };
//	static String morse{};
//	static String word{};
//
//	if (bufferIndex > 0 && button.State() == ButtonState::Released && elapsedMs(button.LastChangedMs(), frameTimeMs) > 1000) {
//		
//		// get minimum press duration
//		unsigned int dotDuration = 0xffff;
//		for (int i = 0; i < bufferIndex; i++) {
//			ButtonPress& bp = inputBuffer[i];
//			if (bp.State == ButtonState::Pressed && bp.Duration < dotDuration)
//				dotDuration = bp.Duration;
//		}
//		
//		unsigned int dashDuration = dotDuration * 3;
//		unsigned int wordSeperatorDuration = dotDuration * 7;
//
//		morse = "";
//		word = "";
//		for (int i = 0; i < bufferIndex; i++) {
//			ButtonPress& bp = inputBuffer[i];
//			word += "[";
//			word += bp.Duration;
//			word += "] ";
//
//			switch (bp.State) {
//			case ButtonState::Pressed:
//				if (bp.Duration < dashDuration)
//					morse += ".";
//				else
//					morse += "-";
//				break;
//			case ButtonState::Released:
//				if (bp.Duration >= dashDuration)
//					morse += " ";
//				break;
//			}
//		}
//
//		bufferIndex = 0;
//	}
//
//	if (button.State() != lastKnownState) {
//		if (bufferIndex < BUFFER_SIZE)
//		{
//			ButtonPress& entry = inputBuffer[bufferIndex++];
//
//			
//
//			if (button.State() == ButtonState::Pressed) {
//				digitalWrite(PIN_MORSE_BUZZER, HIGH);
//				entry.State = ButtonState::Released;
//				entry.Duration = button.LastReleasedMs();
//			}
//			else {
//				digitalWrite(PIN_MORSE_BUZZER, LOW);
//				entry.State = ButtonState::Pressed;
//				entry.Duration = button.LastPressedMs();
//			}
//		}
//		lastKnownState = button.State();
//	}
//}

void handleButtonInput(unsigned long frameTimeMs) {
	static ButtonState lastKnownState{ ButtonState::Released };
	static String morse{};
	static String word{};

	constexpr unsigned int dotDuration = 60;
	constexpr unsigned int dashDuration = dotDuration * 3;
	constexpr unsigned int wordSeperatorDuration = dotDuration * 7;

	if (button.State() == ButtonState::Released) {
		auto duration = elapsedMs(button.LastChangedMs(), frameTimeMs);
		
		if (word.length() > 0 && duration > wordSeperatorDuration) {
			handleCommand(word);
			word = "";
		}
		else if (morse.length() > 0 && duration > dashDuration) {
			if (auto ch = letterFromMorseCode(morse); ch != 0) {
				word += ch;
			}
			morse = "";
		}
	}

	if (button.State() != lastKnownState) {
		if (button.State() == ButtonState::Pressed) {
			digitalWrite(PIN_MORSE_BUZZER, HIGH);
		}
		else {
			digitalWrite(PIN_MORSE_BUZZER, LOW);

			auto duration = button.LastPressedMs();
			if (duration <= dashDuration) {
				morse += ".";

				//if (duration < dotDuration) {
				//	// recalculate
				//	dotDuration = duration;
				//	dashDuration = dotDuration * 3;
				//	wordSeperatorDuration = dotDuration * 7;
				//}
			}
			else {
				morse += "-";
			}
		}

		lastKnownState = button.State();
	}
}

void changeAlarm(int index) {
	configuration.CurrentAlarm = index;
	configuration.Save();

	tonePlayer.Play(Alarms[index].Tones, Alarms[index].Size, true);
}

void handleCommand(const String& command) {
	if (command == "1")
		changeAlarm(0);
	else if (command == "2")
		changeAlarm(1);
	else if (command == "3")
		changeAlarm(2);
	else if (command == "4")
		changeAlarm(3);
	else if (command == "5")
		changeAlarm(4);
	else if (command == "6")
		changeAlarm(5);
	else if (command == "E")
		tonePlayer.Stop();
	else if (command == "A")
		morsePlayer.Play("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
}

void loop() {
	auto frameTimeMs{ millis() };

	blinker.Update(frameTimeMs);

	button.Update(frameTimeMs);
	handleButtonInput(frameTimeMs);

	if (motionDetectStarted) {
		led.StartFlashing(RgbColour::Blue, frameTimeMs);
		motionDetectStarted = false;
	}
	if (motionDetectEnded) {
		led.StopFlashing();
		motionDetectEnded = false;
	}

	if (motionDetectedMs > 0 && elapsedMs(motionDetectedMs, frameTimeMs) > 10000) {
		if (!alarm) {
			led.ChangeFlashColour(RgbColour::Red);
			if (!tonePlayer.IsPlaying())
				tonePlayer.Play(Alarms[configuration.CurrentAlarm].Tones, Alarms[configuration.CurrentAlarm].Size, true);
			alarm = true;
		}
	}
	else
		alarm = false;

	led.Update(frameTimeMs);
	tonePlayer.Update(frameTimeMs);
	morsePlayer.Update(frameTimeMs);
}