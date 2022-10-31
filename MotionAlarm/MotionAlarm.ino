//#define DEBUG

//#include "LowPower.h"
#include "Timing.h"
#include "Config.h"
#include "TonePlayer.h"
#include "Blinker.h"
#include "RgbLed.h"
#include "Button.h"

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

struct MorseCode {
	char Letter;
	char* Code;
};

constexpr MorseCode MorseCodeTable[]{
	{'A', ".-" },
	{'B', "-..." },
	{'C', "-,-,"},
	{'D', "-.."},
	{'E', "."},
	{'F', "..-."},
	{'G', "--." },
	{'H', "...."},
	{'I',".."},
	{'J',".---"},
	{'K',"-.-."},
	{'L',".-.."},
	{'M',"--"},
	{'N',"-."},
	{'O',"---"},
	{'P',".--."},
	{'Q',"--.-"},
	{'R',".-."},
	{'S',"..."},
	{'T',"-"},
	{'U',"..-"},
	{'V',"...-"},
	{'W',".--"},
	{'X',"-..-"},
	{'Y',"-.--"},
	{'Z',"--.."},

	{'1',".----"},
	{'2',"..---"},
	{'3',"...--"},
	{'4',"....-"},
	{'5',"....."},
	{'6',"-...."},
	{'7',"--..."},
	{'8',"---.."},
	{'9',"----."},
	{'0',"-----"}
};

RgbLed led{};
Blinker blinker{};
TonePlayer tonePlayer{};
Button button{};
Config configuration{};

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

	blinker.Start(&Blink1);

	attachInterrupt(digitalPinToInterrupt(PIN_TESTBUTTON), testButtonChanged, CHANGE);

	pinMode(PIN_PIR, INPUT);
	attachInterrupt(digitalPinToInterrupt(PIN_PIR), pirStatusChanged, CHANGE);

	pinMode(PIN_MORSE_BUZZER, OUTPUT);
}

void handleButtonInput(unsigned long frameTimeMs) {
	static ButtonState lastKnownState{ ButtonState::Released };
	static String morse{};
	static String word{};

	if (button.State() == ButtonState::Released) {
		auto duration = elapsedMs(button.LastChangedMs(), frameTimeMs);
		
		if (word.length() > 0 && duration > 500) {
			handleCommand(word);
			word = "";
		}
		else if (morse.length() > 0 && duration > 250) {
			if (auto ch = lookupMorseCode(morse); ch != 0) {
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
			if (duration < 120) {
				morse += ".";
			}
			else {
				morse += "-";
			}
		}

		lastKnownState = button.State();
	}
}

char lookupMorseCode(const String& code) {
	for (int i = 0; i < sizeof(MorseCodeTable) / sizeof(MorseCode); i++) {
		if (code == MorseCodeTable[i].Code) {
			return MorseCodeTable[i].Letter;
		}
	}
	return 0;
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
}