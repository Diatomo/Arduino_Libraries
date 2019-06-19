/*
 * digits.h
 * advanced display library for seven segment displays
 *
 * Rev 0 - Keegan Morrow
 * Rev 1 - 20.02.2013 KM - Added segDispSign, chaseAnimation
 * Rev 2 - 18.04.2013 KM - Bug fix in digitGroup::segCalc() used by segDisp() and segDispSign() - out of bounds when displaying a '0'
 * Rev 3 - 23.10.2013 KM - Added digitGroup::getNumDigits(), edited symType enum to explicitly state each value
 * Rev 4 - 31.01.2014 KM - Added hook utilities to digits, added getSize() to digits
 * Rev 5 - 21.04.2015 KM - Updated update() to support the AS1109 LED driver chip
 * Rev 6 - 07.03.2016 KM - Added copySection() to the digits class, fixed autoUpdate bug
 * Rev 7 - 26.08.2016 KM - Added chaseAnimation8() to digitGroup
 *
 */

#ifndef __digits_h_
#define __digits_h_

#define DIGITS 7 //revision number
#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include <inttypes.h>

#include "utility/hook.h"

enum symType
{
	blank = 1, err = 2, foul = 3, dash = 4, test = 5 // don't change this!
};

static uint8_t _digits_mapToSegs(uint8_t);
static uint8_t _digits_iToSegs(uint32_t, uint8_t *, uint8_t, uint8_t);


/**
 *  Hardware interface class for a chain of digits.
 *  @author Keegan Morrow
 *  @version 7 2016.08.26
 */
class digits: public hook
{
private:
	uint8_t dataMask;
	uint8_t clkMask;
	uint8_t latchMask;
	volatile uint8_t *dataPortPtr;
	volatile uint8_t *clkPortPtr;
	volatile uint8_t *latchPortPtr;
	void sendByte(uint8_t);
protected:
	/**
	 * Buffer size, derived classes should not modify this.
	 */
	uint8_t numChips;

	/**
	 *  Output buffer, derived classes can modify the data, but should not change the pointer address.
	 */
	uint8_t *chips;
public:
	/**
	 *  Determines if digits::update() is called automatically. Default is true.
	 */
	boolean autoUpdate; // setting this to false and calling update() can speed up some applications (use caution)
	digits(uint8_t, uint8_t, uint8_t, uint8_t); // data, clock, latch, numBoards
	void update(); // see autoUpdate
	uint8_t *getPtr(); // pointer to the output buffer
	uint8_t getSize(); // returns the total number of digits (the size of the buffer)
	void setDigit(uint8_t, uint8_t, boolean); // position, number to display (single digit), decimal point
	void copySection(uint8_t, uint8_t, uint8_t);
};

/**
 *  Interface to the digits hardware interface class for logical groups of digits.
 *  @author Keegan Morrow
 *  @version 7 2016.08.26
 */
class digitGroup
{
private:
	digits *digitsPtr;
	uint8_t *digPtr;
	uint8_t numDigits;
	uint8_t segCalc(uint32_t, uint8_t);
public:
	digitGroup(digits *, uint8_t, uint8_t); // pointer to digits class, offset, numDigits
	void segDisp(uint32_t);
	void segDisp(uint32_t, uint8_t);
	void segDisp(symType); // [ blank, err, foul, dash, test ] (test will turn all segments on)
	boolean segDispSign(int32_t); // display a signed number and generate a '-' if negative or return true if not enough digits
	boolean segDispSign(int32_t, uint8_t); // signed number, position of decimal point
	void chaseAnimation(); // display a spinning animation (will advance when called)
	void chaseAnimation(uint8_t); // jump to a particular point in chaseAnimation (input is modulo 6)
	void chaseAnimation8(); // display a figure eight animation (will advance when called)
	void chaseAnimation8(uint8_t); // jump to a particular point in chaseAnimation8 (input is modulo 8)
	uint8_t getNumDigits(); // returns the number of digits in the group
};

#endif //__digits_h_
