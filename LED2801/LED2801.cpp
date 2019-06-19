/*
 * LED2801.cpp
 * Driver for LED pixels based on the WS2801 driver IC
 * 
 * Rev 1 4/2012 Keegan Morrow
 * Rev 2 5/2012 Keegan Morrow - added getByteCount added getByteCount, setAll, setColor, setAllColor, setColorOrder
 * Rev 3 9/2012 Keegan Morrow - added setColor(uint32_t) and aliased setAllColor
 * Rev 4 2/2013 Keegan Morrow - Added overloaded constructors to allow for mapping logical pixels to a larger number of physical pixels, fixed a bug in color orders
 * Rev 5 3/2013 Keegan Morrow - Fixed a bug in send() causing non-SPI communications to fail
 * Rev 6 6/2013 Keegan Morrow - disable interrupts during send operation (a pause >0.7ms is a latch signal)
 * Rev 7 11/2013 Keegan Morrow - Added colorReorder() to allow access to the current color order function
 * 
 */
#include "LED2801.h"

void LED2801::startup_spi()
{
	hwSPI = true;
	SPI.begin();
	SPI.setBitOrder(MSBFIRST);
	SPI.setDataMode(SPI_MODE0);
	SPI.setClockDivider(SPI_CLOCK_DIV8); // 2 MHz
}

void LED2801::startup_bb(byte dataPin, byte clockPin)
{
	hwSPI = false;
	dataPortPtr = portOutputRegister(digitalPinToPort(dataPin));
	clkPortPtr = portOutputRegister(digitalPinToPort(clockPin));
	dataMask = digitalPinToBitMask(dataPin);
	clkMask = digitalPinToBitMask(clockPin);

	pinMode(dataPin, OUTPUT);
	pinMode(clockPin, OUTPUT);

	*clkPortPtr &= ~clkMask; // set clock line to low
}

void LED2801::startup()
{
	outputBuffer = (byte*) calloc(this->numLEDs, 1);
	if (outputBuffer == NULL)
		while (1);
	autoUpdate = true;
	colorOrder = &_RGB;
}

LED2801::~LED2801()
{
	if(outputBuffer != NULL) free(outputBuffer);
}

LED2801::LED2801(word numLEDs, byte groupSize)
{
	this->numLEDs = numLEDs * 3;
	this->groupSize = groupSize;
	startup_spi();
	startup();
}

LED2801::LED2801(word numLEDs)
{
	this->numLEDs = numLEDs * 3;
	this->groupSize = 1;
	startup_spi();
	startup();
}

LED2801::LED2801(byte dataPin, byte clockPin, word numLEDs, byte groupSize)
{
	this->numLEDs = numLEDs * 3;
	this->groupSize = groupSize;
	startup_bb(dataPin, clockPin);
	startup();
}

LED2801::LED2801(byte dataPin, byte clockPin, word numLEDs)
{
	this->numLEDs = numLEDs * 3;
	this->groupSize = 1;
	startup_bb(dataPin, clockPin);
	startup();
}

void LED2801::setColorOrder(uint32_t (*ordFP)(uint32_t))
{
	colorOrder = ordFP;
}

void LED2801::sendSPI(byte data)
{
	SPDR = data;
	while (!(SPSR & (1 << SPIF)))
		; // wait for previous SPI transfer to finish
}

void LED2801::sendBB(byte data)
{
	for (byte bit = 0x80; bit; bit >>= 1)
	{
		if (data & bit)
			*dataPortPtr |= dataMask;
		else
			*dataPortPtr &= ~dataMask;
		*clkPortPtr |= clkMask;
		*clkPortPtr &= ~clkMask;
	}
}

void LED2801::send()
{
	boolean interruptStatus;

	interruptStatus = LED2801getInterruptStatus();
	noInterrupts();
	byte *bufP = outputBuffer;
	if (hwSPI)
	{
		for (word i = 0; i < numLEDs; i++)
		{
			byte r = *bufP++;
			byte g = *bufP++;
			byte b = *bufP++;
			for (byte j = 0; j < groupSize; j++)
			{
				sendSPI(r);
				sendSPI(g);
				sendSPI(b);
			}
		}
	}
	else
	{
		for (word i = 0; i < numLEDs; i++)
		{
			byte r = *bufP++;
			byte g = *bufP++;
			byte b = *bufP++;
			for (byte j = 0; j < groupSize; j++)
			{
				sendBB(r);
				sendBB(g);
				sendBB(b);
			}
		}
	}
	LED2801setInterruptStatus(interruptStatus);
	delay(1); //delay to latch data
}

void LED2801::setAll(byte level)
{
	memset(outputBuffer, level, numLEDs);
	if (autoUpdate)
		send();
}

void LED2801::setColor(word pixNum, uint32_t col)
{
	if ((pixNum * 3) >= numLEDs)
		return;

	uint32_t color = (*colorOrder)(col);

	byte *bufP = outputBuffer + (pixNum * 3);
	*bufP++ = (color >> 16) & 0xFF; //red
	*bufP++ = (color >> 8) & 0xFF; //green
	*bufP++ = color & 0xFF; //blue

	if (autoUpdate)
		send();
}

uint32_t LED2801::colorReorder(uint32_t c)
{
	return (*colorOrder)(c);
}

uint32_t LED2801::colorReorder(byte r, byte g, byte b)
{
	uint32_t c;
	c = r;
	c <<= 8;
	c |= g;
	c <<= 8;
	c |= b;
	return (*colorOrder)(c);
}

void LED2801::setAllColor(uint32_t col)
{
	setColor(col);
}

void LED2801::setColor(uint32_t col)
{
	uint32_t color = (*colorOrder)(col);
	byte red = (color >> 16) & 0xFF;
	byte green = (color >> 8) & 0xFF;
	byte blue = color & 0xFF;
	byte *bufP = outputBuffer;

	for (byte i = 0; i < numLEDs; i += 3)
	{
		*bufP++ = red;
		*bufP++ = green;
		*bufP++ = blue;
	}
	if (autoUpdate)
		send();
}

byte *LED2801::getPtr()
{
	return outputBuffer;
}

word LED2801::getByteCount()
{
	return numLEDs;
}

uint32_t _RGB(uint32_t color)
{
	return color;
}

uint32_t _GRB(uint32_t color)
{
	union
	{
		uint32_t UL;
		uint8_t UC[4];
	} x, y;

	x.UL = color;
	y.UC[1] = x.UC[2]; //r
	y.UC[2] = x.UC[1]; //g
	y.UC[0] = x.UC[0]; //b

	return y.UL;
}

uint32_t _RBG(uint32_t color)
{
	union
	{
		uint32_t UL;
		uint8_t UC[4];
	} x, y;

	x.UL = color;
	y.UC[2] = x.UC[2]; //r
	y.UC[0] = x.UC[1]; //g
	y.UC[1] = x.UC[0]; //b

	return y.UL;
}

uint32_t _GBR(uint32_t color)
{
	union
	{
		uint32_t UL;
		uint8_t UC[4];
	} x, y;

	x.UL = color;
	y.UC[0] = x.UC[2]; //r
	y.UC[2] = x.UC[1]; //g
	y.UC[1] = x.UC[0]; //b

	return y.UL;
}

uint32_t _BRG(uint32_t color)
{
	union
	{
		uint32_t UL;
		uint8_t UC[4];
	} x, y;

	x.UL = color;
	y.UC[1] = x.UC[2]; //r
	y.UC[0] = x.UC[1]; //g
	y.UC[2] = x.UC[0]; //b

	return y.UL;
}

uint32_t _BGR(uint32_t color)
{
	union
	{
		uint32_t UL;
		uint8_t UC[4];
	} x, y;

	x.UL = color;
	y.UC[0] = x.UC[2]; //r
	y.UC[1] = x.UC[1]; //g
	y.UC[2] = x.UC[0]; //b

	return y.UL;
}
