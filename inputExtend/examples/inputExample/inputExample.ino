/*
 * Roto
 * Example Code
 *
 * Library:
 * inputExtend
 *
 * Author(s):
 * Keegan Morrow
 *
 */

// Libraries:
#include <inputExtend.h>

// Pin Assignments:
const byte fet4 = 4;
const byte fet5 = 5;
const byte fet6 = 6;
const byte fet7 = 7;

const byte inputDataPin  = 8;
const byte inputClockPin = 9;
const byte inputLatchPin = 10;

// Constants:
const byte numberOfInputChips = 1;

// Global Variables:

// Global Classes:
inputExtend inputs = inputExtend(inputDataPin, inputClockPin, inputLatchPin, numberOfInputChips);


void setup()
{
	pinMode(fet4, OUTPUT);
	pinMode(fet5, OUTPUT);
	pinMode(fet6, OUTPUT);
	pinMode(fet7, OUTPUT);
	digitalWrite(fet4, LOW);
	digitalWrite(fet5, LOW);
	digitalWrite(fet6, LOW);
	digitalWrite(fet7, LOW);

	Serial.begin(9600);
	Serial.println("Startup");
}

void loop()
{
	byte inByte;
	boolean inBit0, inBit1;

	inByte = inputs.byteRead(0); // 0 = first board in the chain

	inBit0 = inputs.extendedRead(0); // 0 = first pin of first board (8 = first pin of second board)
	inBit1 = inputs.extendedRead(1); // 1 = second pin of first board

	Serial.print("inByte = ");
	Serial.println(inByte, HEX);

	Serial.print("inBit0 = ");
	Serial.println(inBit0, BIN);

	Serial.print("inBit1 = ");
	Serial.println(inBit1, BIN);

	delay(100);
}
