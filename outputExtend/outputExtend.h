/*
 * outputExtend.h
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

#ifndef __outputExtend_h__
#define __outputExtend_h__

#define OUTPUTEXTEND 5 //revision number
#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#include "pins_arduino.h"
#endif

#include "../SPI/SPI.h"
#include <inttypes.h>

#include "utility/hook.h"
/**
 * Hardware interface class for the outputExtend board or other 74HC595 based boards.
 * @author Keegan Morrow
 * @version 5 2015.02.06
 */
class outputExtend: public hook
{
private:
	byte dataMask;
	byte clkMask;
	byte latchMask;
	volatile byte *dataPortPtr;
	volatile byte *clkPortPtr;
	volatile byte *latchPortPtr;
	void startup();
	void startupSPI(byte);
	void startupBB(byte, byte, byte);
	void updateSPI(byte);
	void updateBB(byte);
	boolean hwSPI;

protected:
	/**
	 * Buffer size, derived classes should not modify this.
	 */
	byte *boards;
	/**
	 * Input buffer, derived classes can modify the data, but should not change the pointer address.
	 */
	byte numChips;

public:
	/**
	 * Determines if inputExtend::update() is called automatically. Default is true.
	 */
	boolean autoUpdate; // setting this to false and calling update() can speed up some applications (use caution)
	outputExtend(byte, byte, byte, byte); // data, clock, latch, numBoards
	outputExtend(byte, byte); // latch, numBoards
	void update(); // see autoUpdate
	void extendedWrite(byte, boolean); // outNumber, state
	void byteWrite(byte *, byte, byte); // bytePtr, boardOffset, count
	void byteWrite(byte, byte); // boardNumber, data
	boolean getState(byte); // outNumber
	byte *getPtr(); // pointer to the output buffer
	byte getSize(); // returns the size of the output buffer
};

#endif //__outputExtend_h__
