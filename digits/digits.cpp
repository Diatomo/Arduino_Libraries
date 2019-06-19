/*
 * digits.cpp
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

#include "digits.h"

static const uint8_t _digits_dashSeg = 0x80;
static const uint8_t _digits_blankDigit = 0x0A;


/**
 *  @brief Sets the direction of the pins used and allocates memory.
 *
 *  @param dataPin  Pin number connected to data
 *  @param clockPin Pin number connected to clock
 *
 *  @param latchPin Pin number connected to latch
 *  @param numChips Number of total digits in the chain
 *
 */
digits::digits(uint8_t dataPin, uint8_t clockPin, uint8_t latchPin, uint8_t numChips)
{
	dataPortPtr = portOutputRegister(digitalPinToPort(dataPin));
	clkPortPtr = portOutputRegister(digitalPinToPort(clockPin));
	latchPortPtr = portOutputRegister(digitalPinToPort(latchPin));

	dataMask = digitalPinToBitMask(dataPin);
	clkMask = digitalPinToBitMask(clockPin);
	latchMask = digitalPinToBitMask(latchPin);

	this->numChips = numChips;

	chips = (uint8_t *) calloc(numChips, sizeof(uint8_t));
	if (chips == NULL)
		while (1); //this is a (kludgy) catch-all for out of memory errors

	pinMode(dataPin, OUTPUT);
	pinMode(latchPin, OUTPUT);
	pinMode(clockPin, OUTPUT);
	digitalWrite(latchPin, LOW);

	autoUpdate = true;
	update();
}

/**
 * @brief Copy a section from one set of digits to another
 *
 * @param fromStart Offset for the data to be copied
 * @param toStart Offset for the destination
 * @param length Number of digits to copy
 */
void digits::copySection(uint8_t fromStart, uint8_t toStart, uint8_t length)
{
	if ((fromStart + length) > numChips) { return; }
	if ((toStart + length) > numChips) { return; }
	if ((fromStart + length) > toStart) { return; }
	memcpy(chips + toStart, chips + fromStart, length);
}

/**
 *  @brief Send the output buffer to the chain
 */
void digits::update()
{
	for (uint8_t i = numChips; i != 0; i--)
	{
		sendByte(*(chips + (i - 1)));
	}
	*latchPortPtr |= latchMask;
	*latchPortPtr &= ~latchMask;
	callHook();
}

/* Private Function */
void digits::sendByte(uint8_t data)
{
	for (uint8_t j = 0; j < 8; j++)
	{
		if (data & 0x80)
		{
			*dataPortPtr |= dataMask;
		}
		else
		{
			*dataPortPtr &= ~dataMask;
		}
		*clkPortPtr |= clkMask;
		*clkPortPtr &= ~clkMask;
		data = data << 1;
	}
}

/**
 *  @brief Get a pointer to the output buffer
 *
 *  @return [uint8_t *] Pointer to the output buffer
 */
uint8_t *digits::getPtr()
{
	return chips;
}

/**
 *  @brief Get the size of the output buffer in uint8_t
 *
 *  @return [uint8_t] Size of the output buffer
 */
uint8_t digits::getSize()
{
	return numChips;
}

/**
 *  @brief Display a single digit at a position on the chain
 *
 *  @param digit Position in the chain
 *  @param num   Number to be displayed
 *  @param state State of the decimal point
 */
void digits::setDigit(uint8_t digit, uint8_t num, boolean state)
{
	if (digit >= numChips)
	{
		return;
	}
	*(chips + digit) = _digits_mapToSegs(num);
	if (state)
	{
		*(chips + digit) |= 0x01;
	}
	else
	{
		*(chips + digit) &= 0xFE;
	}
	if (autoUpdate)
	{
		update();
	}
}

/**
 *  @brief Set-up the digit group
 *
 *  @param digitsPtr Pointer to the digits object
 *  @param offset    Position of the first digit in the group in the digits chain
 *  @param numDigits Number of digits in the group
 */
digitGroup::digitGroup(digits *digitsPtr, uint8_t offset, uint8_t numDigits)
{
	this->digitsPtr = digitsPtr;
	this->numDigits = numDigits;
	digPtr = digitsPtr->getPtr() + offset;
}

/* Private Function */
uint8_t digitGroup::segCalc(uint32_t number, uint8_t dpPos)
{
	uint8_t digitsUsed;
	digitsUsed = _digits_iToSegs(number, digPtr, numDigits, _digits_blankDigit);
	if ((dpPos != 0) && (dpPos <= numDigits))
	{
		for (uint8_t i = dpPos - 1; i > 0; i--)
		{
			if (*(digPtr + dpPos - i) == 0x00)
			{
				*(digPtr + dpPos - i) = _digits_mapToSegs(0);
			}
		}
		*(digPtr + dpPos - 1) |= 0x01;
	}
	return (digitsUsed >= dpPos) ? digitsUsed : dpPos;
}

/**
 *  @brief Display a decimal number
 *
 *  @param number Number to display
 */
void digitGroup::segDisp(uint32_t number)
{
	segCalc(number, 0);
	if (digitsPtr->autoUpdate) { digitsPtr->update(); }
}

/**
 *  @brief Display a decimal number
 *
 *  @param number Number to display
 *  @param dpPos  Position of the decimal point (0 = none, 1 = right)
 */
void digitGroup::segDisp(uint32_t number, uint8_t dpPos)
{
	segCalc(number, dpPos);
	if (digitsPtr->autoUpdate) { digitsPtr->update(); }
}

/**
 *  @brief Display a Symbol or message
 *
 *  @param sym Symbol to display of type symType
 */
void digitGroup::segDisp(symType sym)
{
	memset(digPtr, 0x00, numDigits);

	if (sym == err)
	{
		const uint8_t errPat[] = { 0xA0, 0xA0, 0xF2 };
		if (sizeof(errPat) == numDigits)
		{
			memcpy(digPtr, errPat, numDigits);
		}
		else if (sizeof(errPat) < numDigits)
			memcpy((digPtr + (numDigits - sizeof(errPat)) / 2), errPat,
			       sizeof(errPat));
		else
		{
			memcpy(digPtr, errPat, numDigits);
		}
	}
	else if (sym == foul)
	{
		const uint8_t foulPat[] = { 0x0C, 0x38, 0xB8, 0xE2 };
		if (sizeof(foulPat) == numDigits)
		{
			memcpy(digPtr, foulPat, numDigits);
		}
		else if (sizeof(foulPat) < numDigits)
			memcpy((digPtr + (numDigits - sizeof(foulPat)) / 2), foulPat,
			       sizeof(foulPat));
		else
		{
			memcpy(digPtr, foulPat, numDigits);
		}
	}
	else if (sym == dash)
	{
		memset(digPtr, _digits_dashSeg, numDigits);
	}
	else if (sym == test)
	{
		memset(digPtr, 0xFF, numDigits);
	}
	if (digitsPtr->autoUpdate) { digitsPtr->update(); }
}

/**
 *  @brief Display a signed number
 *
 *  @param number Number to display
 *  @return [boolean] true if there is a sign overflow
 *
 *  @details This function will show a '-' sign on the right-most leading digit if the number is
 *  negative, if there are not enough digits, the function will return true if the sign is missing.
 *  The return value can be used to trigger a sign LED if needed.
 */
boolean digitGroup::segDispSign(int32_t number)
{
	return segDispSign(number, 0);
}

/**
 *  @brief Display a signed number with a decimal point
 *
 *  @param number Number to display
 *  @param dpPos  Position of the decimal point (0 = none, 1 = right)
 *  @return [boolean] true if there is a sign overflow
 *
 *  @details This function will show a '-' sign on the right-most leading digit if the number is
 *  negative, if there are not enough digits, the function will return true if the sign is missing.
 *  The return value can be used to trigger a sign LED if needed.
 */
boolean digitGroup::segDispSign(int32_t number, uint8_t dpPos)
{
	boolean signOverflow = false;
	uint8_t signPos;
	uint32_t uNumber = abs(number);
	signPos = segCalc(uNumber, dpPos);
	if (number != (long) uNumber)
	{
		if (signPos >= numDigits)
		{
			signOverflow = true;
		}
		else
		{
			*(digPtr + signPos) |= _digits_dashSeg;
		}
	}
	if (digitsPtr->autoUpdate) { digitsPtr->update(); }
	return signOverflow;
}

/**
 *  @brief Display an animation, each call advances the position
 */
void digitGroup::chaseAnimation()
{
	static uint8_t pos;
	chaseAnimation(pos++);
	if (pos > 5)
	{
		pos = 0;
	}
}

/**
 *  @brief Display an animation at a particular position
 *
 *  @param pos Position, 0-5
 */
void digitGroup::chaseAnimation(uint8_t pos)
{
	memset(digPtr, (1 << ((pos % 6) + 1)), numDigits);
	if (digitsPtr->autoUpdate) { digitsPtr->update(); }
}

/**
 *  @brief Display a figure eight animation, each call advances the position
 */
void digitGroup::chaseAnimation8()
{
	static uint8_t pos;
	chaseAnimation8(pos++);
	if (pos > 7)
	{
		pos = 0;
	}
}

/**
 *  @brief Display an animation at a particular position
 *
 *  @param pos Position, 0-5
 */
void digitGroup::chaseAnimation8(uint8_t pos)
{
	const uint8_t pattern[8] =
	{ 0x02, 0x04, 0x80, 0x20, 0x10, 0x08, 0x80, 0x40 };
	memset(digPtr, pattern[pos % 8], numDigits);
	if (digitsPtr->autoUpdate) { digitsPtr->update(); }
}

/**
 *  @brief Get the number of digits in the group
 *
 *  @return [uint8_t] Number of digits
 */
uint8_t digitGroup::getNumDigits()
{
	return numDigits;
}

uint8_t _digits_iToSegs(uint32_t inp, uint8_t *outPtr, uint8_t len, uint8_t fill)
{
	uint8_t digitCount = 0;
	if (inp == 0)
	{
		if (len > 1)
		{
			memset((outPtr + 1), _digits_mapToSegs(fill), len - 1);
		}
		*outPtr = _digits_mapToSegs(0);
		digitCount = 1;
	}
	else
	{
		for (uint8_t pos = 0; pos < len; pos++)
		{
			if (inp != 0)
			{
				*outPtr = _digits_mapToSegs(inp % 10);
				inp /= 10;
				digitCount++;
			}
			else
			{
				*outPtr = _digits_mapToSegs(fill);
			}
			outPtr++;
		}
	}
	return digitCount;
}

uint8_t _digits_mapToSegs(uint8_t i) // input > 9 will blank the digit
{
	const uint8_t segMap[10] =
	{ 0x7E, 0x0C, 0xB6, 0x9E, 0xCC, 0xDA, 0xFA, 0x0E, 0xFE, 0xDE };
	return (i < 10) ? segMap[i] : 0;
}


