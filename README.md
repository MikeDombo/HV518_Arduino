# HV518_Arduino
Arduino/ESP Library for controlling Microchip HV518 High Voltage shift registers. The library does make some assumptions based on my own needs. For example, my colons are connected to the 7th bank of registers because I have 6 digits, but 8 total banks. This means that the special characters that use the colons may not work if your setup is not the same as mine.

## Methods

### Constructors

You must choose one of 4 constructors. The differ in whether they offer a stobe pin and a boolean for left-aligning the display.

```c
HV518(uint8_t dataPin, uint8_t clockPin, uint8_t latchPin, uint8_t numDigits, uint8_t numLines);
HV518(uint8_t dataPin, uint8_t clockPin, uint8_t latchPin, uint8_t strobePin, uint8_t numDigits, uint8_t numLines);
HV518(uint8_t dataPin, uint8_t clockPin, uint8_t latchPin, uint8_t numDigits, uint8_t numLines, bool leftAlignDisplay);
HV518(uint8_t dataPin, uint8_t clockPin, uint8_t latchPin, uint8_t strobePin, uint8_t numDigits, uint8_t numLines, bool leftAlignDisplay);
```

#### Example

```c
// DIN, CLK, LATCH, Number of Digits, Total Shift Lines, left align display
// No dimming using the strobe line. This example shows dimming using anode PWM
HV518 disp = HV518(D5, D6, D7, 6, 64, true);
```

### Writing to Display

To write to the display, use one of the methods below and then call `updateDispay();` to write the data to the HV518.

#### Manually Write Single Digit

Use `setDigit` to set one register to a specific bit pattern.
```c
void setDigit(uint8_t pos, byte data);
```
#### Write a Single Digit

Use `writeSingleDigit` to write a number (0-9) into a specified position.

```c
void writeSingleDigit(uint8_t pos, uint8_t number);
```

#### Write a Number

To write a multiple-numeral integer number, use `writeNumber`. You can optionally specify the total length you would like which will pad the output with 0's.

```c
void writeNumber(uint8_t pos, long number, int totalLength);
void writeNumber(uint8_t pos, long number);
```

#### Write a String

To write a string including alphanumeric characters and some punctuation, use `writeString`.

```c
void writeString(uint8_t pos, String str);
```

## Utilities

Some methods are included as utilities to make using the library simpler for more complex tasks. 

- `getDigit` returns the current state of a specified register.
- `getDisplayMemory` returns a pointer to a byte array that stores the state of all registers.
- `clearDisplayMemory` sets the stored display state to all 0. This does not update the actual display.
- `clearDisplay` clears the display memory and then writes all 0's to the display.
- `setAllHigh` sets all registers to 255 and updates the display.

```c
byte getDigit(uint8_t pos);
byte* getDisplayMemory();
void clearDisplayMemory();
void clearDisplay();
void setAllHigh();
```

## Brightness

There are two methods to control the brightness of the HV518 display. The prefered method is by PWMing the strobe pin on the HV518 which temporarily turns all outputs low and then back to the saved state. Using the PWM method allows the program to continue running even while the display is displaying and dimmed.

The other method, if no strobe pin is connected, is to PWM manually by writing 0's into the shift register and then writing the original state back into it. This method is **blocking** because the processor will spend all its time writing to the HV518.

### Set Strobe Brightness

Use `setBrightnessStrobePWM` to change the brightness. This only needs to be called when you want the brightness to change and not every time you update the display.

```c
void setBrightnessStrobePWM(uint8_t brightness);
```

### Manual Anode PWM Brightness

There are two methods for the manual PWM: the first runs for a specific amount of time, and the second runs until a callback function returns false.

```c
void displayWithAnodePWM(uint8_t duty, long dispTime);
void displayWithAnodePWM(uint8_t duty, bool (*cont)());
```
