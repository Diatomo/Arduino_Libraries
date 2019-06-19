/*
 * outputExtend.cpp
 * Syncronus Serial Output Library
 * For use with the 74HC595
 *
 * Rev 1 - Keegan Morrow - 1/2012
 * Rev 2 - KM 3/2012 - Changed 'byte *boards' and 'byte numChips' to protected, to allow access from derived classes
 * Rev 3 - KM 6/2012 - Added 'boolean getState(pin)'
 * Rev 4 - Keegan Morrow - 1/2014 added getSize(), added hook utility
 * Rev 5 - KM 2/2015 - added code to allow use of the hardware SPI module for very fast updates
 *
 */

#ifndef __outputExtend_cpp__
#define __outputExtend_cpp__
#include "outputExtend.h"

/*private function*/
void outputExtend::startupSPI(byte latchPin)
{
	hwSPI = true;
	SPI.begin();
	SPI.setBitOrder(MSBFIRST);
	SPI.setDataMode(SPI_MODE0);
	SPI.setClockDivider(SPI_CLOCK_DIV8);
	latchPortPtr = portOutputRegister(digitalPinToPort(latchPin));
	latchMask = digitalPinToBitMask(latchPin);
	pinMode(latchPin, OUTPUT);
	digitalWrite(latchPin, LOW);
}

/*private function*/
void outputExtend::startupBB(byte dataPin, byte clockPin, byte latchPin)
{
	hwSPI = false;
	dataPortPtr = portOutputRegister(digitalPinToPort(dataPin));
	clkPortPtr = portOutputRegister(digitalPinToPort(clockPin));
	latchPortPtr = portOutputRegister(digitalPinToPort(latchPin));
	dataMask = digitalPinToBitMask(dataPin);
	clkMask = digitalPinToBitMask(clockPin);
	latchMask = digitalPinToBitMask(latchPin);
	pinMode(dataPin, OUTPUT);
	pinMode(clockPin, OUTPUT);
	pinMode(latchPin, OUTPUT);
	digitalWrite(latchPin, LOW);
}

/*private function*/
void outputExtend::startup()
{
	boards = (byte *) calloc(numChips, 1);
	if (boards == NULL)
		while (1); //this is a (kludgy) catch-all for out of memory errors
	autoUpdate = true;
	update();
}

/**
 * Bitbanging mode constructor. Sets the direction of the io pins and allocates needed memory.
 * This should be used to declare a global object.
 * @param dataPin Pin number attached to the data pin
 * @param clockPin Pin number attached to the data pin
 * @param latchPin Pin number attached to the latch pin
 * @param numChips Number of boards in use
 */
outputExtend::outputExtend(byte dataPin, byte clockPin, byte latchPin,
                           byte numChips)
{
	this->numChips = numChips;
	startupBB(dataPin, clockPin, latchPin);
	startup();
}

/**
 * SPI mode constructor. Sets the direction of the io pins and allocates needed memory.
 * Starts the hardware SPI module. Note: data must be on pin 11, clock must be on pin 13,
 * pin 12 cannot be used.
 * This should be used to declare a global object.
 * @param latchPin Pin number attached to the latch pin
 * @param numChips Number of boards in use
 */
outputExtend::outputExtend(byte latchPin, byte numChips)
{
	this->numChips = numChips;
	startupSPI(latchPin);
	startup();
}

/**
 * Writes an individual output pin.
 * Pin numbers are sequential from the first pin on the first board.
 * Pin 0 is board 0 output 0, pin 8 is board 1 output 0.
 * @param pinNumber Input pin to write
 * @param state State of the pin. HIGH or LOW (true or false)
 */
void outputExtend::extendedWrite(byte pinNumber, boolean state)
{
	byte byteNumber = pinNumber >> 3;
	byte bitNumber = pinNumber - (byteNumber << 3);
	bitWrite(*(boards + byteNumber), bitNumber, state);
	if (autoUpdate)
	{
		update();
	}
}

/**
 * Writes an array of data to the outputs.
 * Note, if count + offset exceeds the output buffer size, the array will be truncated.
 * @param *bytePtr pointer to the array of bytes
 * @param offset number of boards to skip
 * @param count Size of the array to be copied
 */
void outputExtend::byteWrite(byte *bytePtr, byte offset, byte count)
{
	if (offset >= numChips)
	{
		return;
	}
	count = (count + offset) > numChips ? numChips - offset : count;
	memcpy(&boards[offset], bytePtr, (size_t) count);
	if (autoUpdate)
	{
		update();
	}
}

/**
 * Writes bytewise data to a board.
 * @param board Board number to write to
 * @param data Data to be written
 */
void outputExtend::byteWrite(byte board, byte data)
{
	if (board >= numChips)
	{
		return;
	}
	*(boards + board) = data;
	if (autoUpdate)
	{
		update();
	}
}

/**
 * @return Pointer to the output buffer
 */
byte *outputExtend::getPtr()
{
	return boards;
}

/**
 * @return Size in bytes of the output buffer (same as the number of chips)
 */
byte outputExtend::getSize()
{
	return numChips;
}

/**
 * Gets the state of the output pin.
 * Pin numbers are sequential from the first pin on the first board.
 * Pin 0 is board 0 output 0, pin 8 is board 1 output 0.
 * Note, this function reads from the output buffer and does not communicate with any external hardware.
 * @param pinNumber Input pin to read
 * @return State of the pin. HIGH or LOW (true or false)
 */
boolean outputExtend::getState(byte pinNumber)
{
	byte byteNumber = pinNumber >> 3;
	byte bitNumber = pinNumber - (byteNumber << 3);
	return bitRead(*(boards + byteNumber), bitNumber);
}

/**
 * Update the output pins with the current contents of the output buffer.
 * This function is normally called automatically  when needed.
 * In a situation where very fast multi-board writes or synchronized outputs are needed, autoUpdate
 * can be set to false and this can be called manually.
 */
void outputExtend::update()
{
	if (hwSPI)
	{
		for (byte i = numChips; i != 0; i--)
		{
			updateSPI(*(boards + (i - 1)));
		}
	}
	else
	{
		for (byte i = numChips; i != 0; i--)
		{
			updateBB(*(boards + (i - 1)));
		}
	}
	*latchPortPtr |= latchMask;
	*latchPortPtr &= ~latchMask;

	callHook();
}

/*private function*/
void outputExtend::updateSPI(byte data)
{
	SPDR = data;
	while (!(SPSR & (1 << SPIF))); // wait for previous SPI transfer to finish
}

/*private function*/
void outputExtend::updateBB(byte data)
{
	for (byte j = 0; j < 8; j++)
	{
		if (data & 0x80)
		{
			*dataPortPtr |= dataMask;
		}
		else
		{
			*dataPortPtr &= ~dataMask;
		}
		data = data << 1;
		*clkPortPtr |= clkMask;
		*clkPortPtr &= ~clkMask;
	}
}

#endif //__outputExtend_cpp__
