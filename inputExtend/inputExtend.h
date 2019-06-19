/*
 * inputExtend.h
 * Synchronous Serial Input Library
 * For use with the 74HC165
 * 
 * Rev 1 - Keegan Morrow - 10/2011
 * Rev 2 - Keegan Morrow - 6/2012 moved numChips and boards to protected  -  fixed a compile error with v22
 * Rev 3 - Keegan Morrow - 1/2014 added getSize(), added hook utility
 * 
 */

#ifndef __inputExtend_h_
#define __inputExtend_h_

#define INPUTEXTEND 3 //revision number
#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#include "pins_arduino.h"
#endif

#include <inttypes.h>

#include "utility/hook.h"

/**
 * Hardware interface class for the inputExtend board or other boards based on the 74HC165 chip.
 * @author Keegan Morrow
 * @version 3 31.01.2014
 */
class inputExtend: public hook
{
private:
	byte dataMask;
	byte clkMask;
	byte latchMask;
	volatile byte *dataPortPtr;
	volatile byte *clkPortPtr;
	volatile byte *latchPortPtr;

protected:
	/**
	 * Buffer size, derived classes should not modify this.
	 */
	byte numChips;
	/**
	 * Input buffer, derived classes can modify the data, but should not change the pointer address.
	 */
	byte *boards;

public:
	/**
	 * Determines if inputExtend::update() is called automatically. Default is true.
	 */
	boolean autoUpdate;
	inputExtend(byte, byte, byte, byte); //data, clock, latch, boardCount
	boolean extendedRead(byte);
	byte* byteRead();
	byte byteRead(byte);
	void update();
	byte* getPtr(); // pointer to the output buffer
	byte getSize(); // returns the size of the output buffer
};

#endif //__inputExtend_h_
