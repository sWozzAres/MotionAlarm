// ConsoleApplication1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>

using namespace std;

struct MorseCode {
	char Letter;
	const char* Code;
};

constexpr MorseCode MorseCodeTable[]{
	{'A', ".-" },
	{'B', "-..." },
	{'C', "-.-."},
	{'D', "-.."},
	{'E', "."},
	{'F', "..-."},
	{'G', "--." },
	{'H', "...."},
	{'I',".."},
	{'J',".---"},
	{'K',"-.-"},
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

char letterFromMorseCode(const char* code) {
	for (int i = 0; i < sizeof(MorseCodeTable) / sizeof(MorseCode); i++) {
		if (strcmp(code, MorseCodeTable[i].Code) == 0) {
			return MorseCodeTable[i].Letter;
		}
	}
	return 0;
}

const char* morseCodeFromLetter(const char& letter) {
	for (int i = 0; i < sizeof(MorseCodeTable) / sizeof(MorseCode); i++) {
		if (letter == MorseCodeTable[i].Letter) {
			return MorseCodeTable[i].Code;
		}
	}
	return nullptr;
}

class MorseCodePlayer {
private:
	unsigned long startMs{ 0 };
	unsigned long durationMs{ 0 };
	const char* str{nullptr};
	const char* strPtr{nullptr};

	const unsigned long DOT_DURATION = 60;
public:

	void Play(const char* message) {
		str = message;
		strPtr = str;
		startMs = 0;
		durationMs = 0;
	}

	void Update(unsigned long frameTimeMs) {

		static unsigned long durations[10];
		static int size{ 0 };
		static int durationIndex = { size + 1 };
		static bool on{ true };

		if (frameTimeMs - startMs >= durationMs) {

			

			// get next duration
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

			std::cout << durations[durationIndex++] << '\t' << on << '\n';
			on = !on;
		}
	}
};

int main()
{
	MorseCodePlayer player{};
	player.Play("ABC");

	for (unsigned long loop = 0; loop < 10000; loop++) {
		player.Update(loop);
	}
}