#ifndef HV518_h
#define HV518_h

#if (ARDUINO >= 100)
#include <Arduino.h>
#else
#include <WProgram.h>
#endif /* Arduino */

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

		const byte digits[10] = {
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

		const struct {
			byte degree = 0b11000110;
			byte rightColonDot = 0b00100000;
			byte leftColonDot = 0b10000000;
			byte rightColonLine = 0b00010000;
			byte leftColonLine = 0b01000000;
			byte colonDot = 0b10100000;
			byte colonLine = 0b01010000;
		} specialChars;

		const struct {
			byte period = 0b00000001;
			byte underscore = 0b00010000;
			byte hyphen = 0b10000000;
			byte equal = 0b10010000;
		} punctuation;

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