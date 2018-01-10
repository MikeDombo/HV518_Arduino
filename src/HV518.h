#ifndef HV518_h
#define HV518_h

#if defined(__AVR__)
#include <avr/pgmspace.h>
#elif defined(ESP8266)
#include <pgmspace.h>
#endif

#if (ARDUINO >= 100)
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

const static byte digits[] PROGMEM = {
	0b01111110, // 0
	0b00001100, // 1
	0b10110110, // 2
	0b10011110, // 3
	0b11001100, // 4
	0b11011010, // 5
	0b11111010, // 6
	0b00001110, // 7
	0b11111110, // 8
	0b11011110  // 9
};

const static byte letters[] PROGMEM = {
	0b10111110, // a
	0b11111000, // b
	0b10110000, // c
	0b10111100, // d
	0b11110010, // E
	0b11100010, // F
	0b01111010, // g
	0b11101000, // h
	0b00100000, // i
	0b00011100, // j
	0, // k
	0b01110000, // L
	0, // m
	0b10101000, // n
	0b10111000, // o
	0b11100110, // P
	0b11001110, // q
	0b10100000, // r
	0b11011010, // S (5)
	0b11110000, // t
	0b01111100, // U
	0, // v
	0, // w
	0, // x
	0b11011100, // Y
	0b10110110 // Z (2)
};

const static struct PROGMEM {
	byte degree = 0b11000110;
	byte rightColonDot = 0b00100000;
	byte leftColonDot = 0b10000000;
	byte rightColonLine = 0b00010000;
	byte leftColonLine = 0b01000000;
	byte colonDot = 0b10100000;
	byte colonLine = 0b01010000;
} specialChars;

const static struct PROGMEM {
	byte period = 0b00000001;
	byte underscore = 0b00010000;
	byte hyphen = 0b10000000;
	byte equal = 0b10010000;
} punctuation;

class HV518 {
	public:
		HV518(uint8_t dataPin, uint8_t clockPin, uint8_t latchPin, uint8_t numDigits, uint8_t numLines);
		HV518(uint8_t dataPin, uint8_t clockPin, uint8_t latchPin, uint8_t strobePin, uint8_t numDigits, uint8_t numLines);
		HV518(uint8_t dataPin, uint8_t clockPin, uint8_t latchPin, uint8_t numDigits, uint8_t numLines, bool leftAlignDisplay);
		HV518(uint8_t dataPin, uint8_t clockPin, uint8_t latchPin, uint8_t strobePin, uint8_t numDigits, uint8_t numLines, bool leftAlignDisplay);

		byte getDigit(uint8_t pos);
		byte* getDisplayMemory();
		void setDigit(uint8_t pos, byte data);
		void updateDisplay();
		void clearDisplayMemory();
		void clearDisplay();
		void setAllHigh();
		void writeSingleDigit(uint8_t pos, uint8_t number);
		void writeNumber(uint8_t pos, long number, int totalLength);
		void writeNumber(uint8_t pos, long number);
		void writeString(uint8_t pos, String str);
		void displayWithAnodePWM(uint8_t duty, long dispTime);
		void displayWithAnodePWM(uint8_t duty, bool (*cont)());
		void setBrightnessStrobePWM(uint8_t brightness);

		~HV518();

	private:
		uint8_t dataPin;
		uint8_t clockPin;
		uint8_t latchPin;
		uint8_t strobePin;
		uint8_t numDigits;
		uint8_t numLines;
		uint8_t numRegisters;
		bool leftAlignDisplay;
		bool hasStrobe;

		byte* displayState;

		void initDisplay();
		void initData(uint8_t dataPin, uint8_t clockPin, uint8_t latchPin, uint8_t strobePin, uint8_t numDigits, uint8_t numLines, bool leftAlignDisplay, bool hasStrobe);
};


#endif /* HV518_h */