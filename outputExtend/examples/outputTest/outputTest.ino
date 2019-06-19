/*
 * Roto
 * Example Code
 *
 * Library:
 * outputExtend
 *
 * Author(s):
 * Keegan Morrow
 *
 */

// Libraries:
#include <outputExtend.h>

// Pin Assignments:
const byte fet4 = 4;
const byte fet5 = 5;
const byte fet6 = 6;
const byte fet7 = 7;

const byte dataPin = 8;
const byte clockPin = 9;
const byte latchPin = 10;

// Constants:
const byte numberOfBoards = 2;

// Global Variables:

// Global Classes:
outputExtend outputs = outputExtend(dataPin, clockPin, latchPin, numberOfBoards);

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

	//one way to set a group of outputs:
	outputs.byteWrite(0, 0xFF); //set all outputs on the first board to high
	outputs.byteWrite(1, 0xFF); //set all outputs on the second board to high
	delay(500);

	//another way to set a group of outputs:
	memset(outputs.getPtr(), 0, numberOfBoards); //set the output buffer array to all zeros
	//this could also be done like this: outputs.getPtr()[0] = 0; outputs.getPtr()[1] = 0;
	outputs.update(); //update all of the boards with the output buffer

	delay(500);
}

void loop()
{
	static byte i;

	//how to set a single output:
	outputs.extendedWrite(i, HIGH); // set output number 'i' high
	delay(100);
	outputs.extendedWrite(i, LOW);
	delay(100);
	i++;
	if (i >= (numberOfBoards * 8)) i = 0;
}
