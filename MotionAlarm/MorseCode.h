// MorseCode.h

#ifndef _MORSECODE_h
#define _MORSECODE_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "WString.h"

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

char letterFromMorseCode(const String& code);
char* morseCodeFromLetter(const char& letter);

#endif

