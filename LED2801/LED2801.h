/*
 * LED2801.h
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

#ifndef __LED2801_h_
#define __LED2801_h_

#define LED2801REV 7 //revision number
#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#include "pins_arduino.h"
#endif
#include "../SPI/SPI.h"
#include <inttypes.h>

#if defined(ARDUINO)
#define LED2801getInterruptStatus() ((SREG&0x80)==0x80)
#define LED2801setInterruptStatus(x) SREG = x?(SREG | 0x80):(SREG & ~0x80)
#endif

uint32_t _RGB(uint32_t);
uint32_t _GBR(uint32_t);
uint32_t _BRG(uint32_t);
uint32_t _RBG(uint32_t);
uint32_t _GRB(uint32_t);
uint32_t _BGR(uint32_t);

class LED2801
{
private:
	byte dataMask;
	byte clkMask;
	volatile byte *dataPortPtr;
	volatile byte *clkPortPtr;
	byte groupSize;

	uint32_t (*colorOrder)(uint32_t);

	void sendSPI(byte);
	void sendBB(byte);

	void startup();
	void startup_spi();
	void startup_bb(byte, byte);

protected:
	byte *outputBuffer;
	word numLEDs;
	boolean hwSPI;

public:
	boolean autoUpdate;

	LED2801(word, byte); // count, groupSize (count is logical pixels)
	LED2801(byte, byte, word, byte); // dataPin, clockPin, count, groupSize (count is logical pixels)
	LED2801 (word); // count
	LED2801(byte, byte, word); // dataPin, clockPin, count
	~LED2801();
	
	void setColorOrder(uint32_t (*ordFP)(uint32_t));

	void setAll(byte);
	void setColor(word, uint32_t);
	void setAllColor(uint32_t);
	void setColor(uint32_t);
	void send();
	
	uint32_t colorReorder(uint32_t);
	uint32_t colorReorder(uint8_t, uint8_t, uint8_t);

	byte *getPtr();
	word getByteCount();

};
#endif //__LED2801_h_
