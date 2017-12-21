#ifndef HV518_cpp
#define HV518_cpp

#include "HV518.h"

HV518::HV518(uint8_t dataPin, uint8_t clockPin, uint8_t latchPin, uint8_t numDigits, uint8_t numLines){
	initData(dataPin, clockPin, latchPin, 0, numDigits, numLines, true, false);
}

HV518::HV518(uint8_t dataPin, uint8_t clockPin, uint8_t latchPin, uint8_t strobePin, uint8_t numDigits, uint8_t numLines){
	initData(dataPin, clockPin, latchPin, strobePin, numDigits, numLines, true, true);
}

HV518::HV518(uint8_t dataPin, uint8_t clockPin, uint8_t latchPin, uint8_t numDigits, uint8_t numLines, bool leftAlignDisplay){
	initData(dataPin, clockPin, latchPin, 0, numDigits, numLines, leftAlignDisplay, false);
}

HV518::HV518(uint8_t dataPin, uint8_t clockPin, uint8_t latchPin, uint8_t strobePin, uint8_t numDigits, uint8_t numLines, bool leftAlignDisplay){
	initData(dataPin, clockPin, latchPin, strobePin, numDigits, numLines, leftAlignDisplay, true);
}

void HV518::initData(uint8_t dataPin, uint8_t clockPin, uint8_t latchPin, uint8_t strobePin, uint8_t numDigits, uint8_t numLines, bool leftAlignDisplay, bool hasStrobe){
	this->dataPin = dataPin;
	this->clockPin = clockPin;
	this->latchPin = latchPin;
	this->strobePin = strobePin;
	this->numDigits = numDigits;
	this->numLines = numLines;
	
	this->hasStrobe = hasStrobe;
	this->leftAlignDisplay = leftAlignDisplay;

	numRegisters = this->numLines / 8;
	displayState = new byte[numRegisters];
	
	initDisplay();
}

void HV518::initDisplay(){
	pinMode(latchPin, OUTPUT);
	pinMode(clockPin, OUTPUT);
	pinMode(dataPin, OUTPUT);
	if(hasStrobe){
		pinMode(strobePin, OUTPUT);
		digitalWrite(strobePin, LOW);
	}

	// Reset latch
	digitalWrite(clockPin, HIGH);
	digitalWrite(dataPin, LOW);
	digitalWrite(latchPin, LOW);
	digitalWrite(latchPin, HIGH);
	digitalWrite(latchPin, LOW);
}

byte HV518::getDigit(uint8_t pos){
	if(leftAlignDisplay && pos < numDigits){
		pos = numDigits - pos - 1;
	}
	return displayState[pos];
}

void HV518::setDigit(uint8_t pos, byte data){
	if(pos > numRegisters){
		return;
	}

	// Reverse position for digits when left aligned
	if(leftAlignDisplay && pos < numDigits){
		pos = numDigits - pos - 1;
	}

	displayState[pos] = data;
}

void HV518::updateDisplay(){
	for(uint8_t i = 0; i < numRegisters; i++){
		shiftOut(dataPin, clockPin, MSBFIRST, displayState[i]);
	}

	// Maybe replace with faster digital writing
	digitalWrite(latchPin, HIGH);
	digitalWrite(latchPin, LOW);
}

void HV518::clearDisplayMemory(){
	for(uint8_t i = 0; i < numRegisters; i++){
		displayState[i] = 0;
	}
}

void HV518::clearDisplay(){
	clearDisplayMemory();
	updateDisplay();
}

void HV518::setAllHigh(){
	for(uint8_t i = 0; i < numRegisters; i++){
		displayState[i] = 255;
	}
	updateDisplay();
}

void HV518::writeSingleDigit(uint8_t pos, uint8_t number){
	// Convert from ASCII
	// Not a problem because this should be a SINGLE digit (not 2 digits)
	if(number >= 48 && number <= 57){
		number -= 48;
	}

	if(pos < numDigits && number < 10 && number > -10){
		// Handle negatives
		if(number < 0){
			number *= -1; // Make the number positive for looking it up in the byte array

			if(leftAlignDisplay){
				setDigit(pos++, punctuation.hyphen);
			}
			else{
				setDigit(pos+1, punctuation.hyphen);
			}
		}

		setDigit(pos, digits[number]);
	}
}

void HV518::writeNumber(uint8_t pos, long number, int totalLength){
	char numChar[12]; // 12 is max length of a long (incl. negative)
	ltoa(number, numChar, 10);
	int numLen = strlen(numChar);

	if(leftAlignDisplay){
		if(totalLength > -1 && numLen < totalLength){
			int padLen = totalLength - numLen;
			for(int i = 0; i < padLen; i++){
				writeSingleDigit(pos++, 0);
			}
		}
		for(int i = 0; i < numLen; i++){
			if(numChar[i] == '-'){
				// Convert from ASCII
				writeSingleDigit(pos, (numChar[i + 1] - 48)* -1);
				i++; // Extra increment for negative
				pos += 2;
			}
			else{
				writeSingleDigit(pos++, numChar[i]);
			}
		}
	}
	else { // Right aligned display
		if(totalLength > -1 && numLen < totalLength){
			int padLen = totalLength - numLen;
			int padPos = pos;
			for(int i = 0; i < padLen; i++){
				writeSingleDigit((padPos++) + numLen, 0);
			}
		}
		for(int i = numLen - 1; i >= 0; i--){
			if(i == 1 && number < 0){
				// Convert from ASCII
				writeSingleDigit(pos, (numChar[i] - 48)* -1);
				pos += 2;
				i--;
			}
			else{
				writeSingleDigit(pos++, numChar[i]);
			}
		}
	}
}

void HV518::writeNumber(uint8_t pos, long number){
	writeNumber(pos, number, -1);
}

void HV518::writeString(uint8_t pos, String str){
	clearDisplayMemory();

	uint8_t firstPosition = 0;
	if(!leftAlignDisplay){
		firstPosition = numDigits;
	}

	for(uint8_t i = 0; i < str.length(); i++){
		char c;
		if(leftAlignDisplay){
			c = str.charAt(i);
		}
		else{
			// Go through the string backwards
			c = str.charAt(str.length() - i - 1);
		}

		if(c >= 48 && c <= 57){
			c -= 48;
			// OR with current data to keep period (if any)
			setDigit(pos++, digits[c] | getDigit(pos));		
		}
		else if(c == '-'){
			setDigit(pos++, punctuation.hyphen | getDigit(pos));
		}
		else if(c == '_'){
			setDigit(pos++, punctuation.underscore | getDigit(pos));
		}
		else if(c == '='){
			setDigit(pos++, punctuation.equal | getDigit(pos));
		}
		else if(c == ' '){
			pos++;
			continue;
		}
		else if(c == '.'){
			uint8_t prevPos = pos - 1;
			bool doneOne = false;

			if(!leftAlignDisplay){
				prevPos = pos;

				int nextIndex = str.length() - i - 2;
				if(nextIndex >= 0 && str.charAt(nextIndex) == '.'){
					setDigit(pos++, getDigit(prevPos) | punctuation.period);
					doneOne = true;
				}
			}
			
			if(!doneOne){
				if(pos == firstPosition || getDigit(prevPos) & punctuation.period > 0){
					setDigit(pos++, punctuation.period);
				}
				else{
					setDigit(prevPos, getDigit(prevPos) | punctuation.period);
				}
			}
		}
	}
}

void HV518::displayWithAnodePWM(uint8_t duty, long dispTime){
	unsigned long startTime = millis();
	unsigned long endTime = startTime + dispTime;

	// Backup current display data
	byte dataBak[numRegisters];
	memcpy(dataBak, displayState, numRegisters * sizeof(byte));

	uint8_t offTime = 0;
	uint8_t onTime = 1;
	if(duty == 0){
		onTime = 0;
		clearDisplay();
	}
	else if(duty < 255){
		offTime = 255 - duty;
		onTime = duty;
	}

	uint8_t interlaceRatioOff = offTime / onTime;
	uint8_t interlaceRatioOn = onTime / offTime;

	// Loop until time is up
	while(millis() < endTime){
		if(offTime > onTime){
			for(uint8_t i = 0; i < offTime && millis() < endTime; i++){
				clearDisplay();
				if(i % interlaceRatioOff == 0 && onTime > 0 && millis() < endTime){
					// On Display
					memcpy(displayState, dataBak, numRegisters * sizeof(byte));
					updateDisplay();
				}
			}
		}
		else{
			for(uint8_t i = 0; i < onTime && millis() < endTime; i++){
				// On Display
				memcpy(displayState, dataBak, numRegisters * sizeof(byte));
				updateDisplay();
				if(i % interlaceRatioOn == 0 && offTime > 0 && millis() < endTime){
					clearDisplay();
				}
			}
		}
	}

	// Make sure the display state is restored
	memcpy(displayState, dataBak, numRegisters * sizeof(byte));
}

void HV518::setBrightnessStrobePWM(uint8_t brightness){
	if(hasStrobe){
		if(brightness > 0 && brightness < 255){ // PWM
			analogWrite(strobePin, brightness);
		}
		else if(brightness == 0){ // Steady-state off
			digitalWrite(strobePin, HIGH);
		}
		else{ // Steady-state on
			digitalWrite(strobePin, LOW);
		}
	}
}

HV518::~HV518(){
	if(displayState){
		delete[] displayState;
	}
}

#endif /* HV518_cpp */