/*
 * inputExtend.cpp
 * Synchronous Serial Input Library
 * For use with the 74HC165
 * 
 * Rev 1 - Keegan Morrow - 10/2011
 * Rev 2 - Keegan Morrow - 6/2012 moved numChips and boards to protected
 * Rev 3 - Keegan Morrow - 1/2014 added getSize(), added hook utility
 * 
 */

#include "inputExtend.h"

/**
 * Sets the direction of the IO pins and allocates needed memory.
 * This should be used to declare a global object.
 * @param dataPin Pin number attached to the data pin
 * @param clockPin Pin number attached to the data pin
 * @param latchPin Pin number attached to the latch pin
 * @param numChips Number of boards in use
 */
inputExtend::inputExtend(byte dataPin, byte clockPin, byte latchPin,
		byte numChips)
{

	dataPortPtr = portInputRegister(digitalPinToPort(dataPin));
	clkPortPtr = portOutputRegister(digitalPinToPort(clockPin));
	latchPortPtr = portOutputRegister(digitalPinToPort(latchPin));

	dataMask = digitalPinToBitMask(dataPin);
	clkMask = digitalPinToBitMask(clockPin);
	latchMask = digitalPinToBitMask(latchPin);

	this->numChips = numChips;

	boards = (byte*) calloc(numChips, 1);
	if (boards == NULL)
		while (1); //this is a (kludgy) catch-all for out of memory errors

	pinMode(dataPin, INPUT);
	pinMode(latchPin, OUTPUT);
	pinMode(clockPin, OUTPUT);
	digitalWrite(dataPin, HIGH); //for the internal pull-up
	digitalWrite(latchPin, HIGH);
	digitalWrite(clockPin, LOW);
	
	autoUpdate = true;
}

/**
 * Reads the inputs to the input buffer.
 * THIS IS OBSOLETE, DO NOT USE FOR NEW CODE!
 * @return Pointer to the buffer
 * \deprecated
 */
byte* inputExtend::byteRead()
{
	if (autoUpdate)
		update();
	return boards;
}

/**
 * @return Pointer to the input buffer
 */
byte* inputExtend::getPtr()
{
	return boards;
}

/**
 * @return Size in bytes of the input buffer (same as the number of chips)
 */
byte inputExtend::getSize()
{
	return numChips;
}

/**
 * Reads one board
 * @param boardNumber Board to read from. 
 * @return bytewise data from the inputs
 */
byte inputExtend::byteRead(byte boardNumber)
{
	if (autoUpdate)
		update();
	return *(boards + boardNumber);
}

/**
 * Reads an individual input pin. 
 * Pin numbers are sequential from the first pin on the first board.
 * Pin 0 is board 0 input 0, pin 8 is board 1 input 0.
 * @param pinNumber Input pin to read
 * @return State of the pin. HIGH or LOW (true or false)
 */
boolean inputExtend::extendedRead(byte pinNumber)
{
	byte byteNumber = pinNumber >> 3;
	byte bitNumber = pinNumber - (byteNumber << 3);
	if (autoUpdate)
		update();
	return bitRead(*(boards + byteNumber), bitNumber);
}

/**
 * Update the input buffer with the current state of the pins.
 * This function is normally called automatically  when needed.
 * In a situation where very fast reads or snapshots are needed, autoUpdate
 * can be set to false and this can be called manually.
 */
void inputExtend::update()
{
	byte temp = 0;

	*latchPortPtr |= latchMask; //set latch pin

	for (byte i = 0; i < numChips; i++)
	{
		temp = 0;
		for (byte j = 0; j < 8; j++)
		{
			temp = temp << 1; //shift data over one place to make room for the new bit
			temp |= ((*dataPortPtr & dataMask) == dataMask); //read data pin to the LSB of temp
			*clkPortPtr |= clkMask; //set clock pin
			*clkPortPtr &= ~clkMask; //clear clock pin
		}
		*(boards + i) = temp; //move the new input byte to the output array (pointer)
	}
	*latchPortPtr &= ~latchMask; //clear latch pin
	callHook();
}
