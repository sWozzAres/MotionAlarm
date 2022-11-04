// 
// 
// 

#include "MorseCode.h"

char letterFromMorseCode(const String& code) {
	for (int i = 0; i < sizeof(MorseCodeTable) / sizeof(MorseCode); i++) {
		if (code == MorseCodeTable[i].Code) {
			return MorseCodeTable[i].Letter;
		}
	}
	return 0;
}

char* morseCodeFromLetter(const char& letter) {
	for (int i = 0; i < sizeof(MorseCodeTable) / sizeof(MorseCode); i++) {
		if (letter == MorseCodeTable[i].Letter) {
			return MorseCodeTable[i].Code;
		}
	}
	return nullptr;
}