/**
*  @file buttonBoard.h
*  @brief Hardware interface for the buttonBoard board with interface helpers.
*  @author Keegan Morrow
*  @version 7 08.18.2015
*
*  @details Revision history
*
*  Rev 0 - 7/2012 Keegan Morrow
*
*  Rev 1 - 8/2012 Keegan Morrow - added comments and example code
*
*  Rev 2 - 9/2012 Keegan Morrow - added .event() to buttonSelect and buttonToggle
*
*  Rev 3 - 10/2012 Keegan Morrow   - added buttonToggleNoLamp class to allow external control of the lamps
*
*  Rev 4 - 1/2014 Keegan Morrow - added getSize() and hook utilities to buttonBoard
*
*  Rev 5 - 8/2014 Keegan Morrow - Bugfix in buttonSelect::poll() to fix incorrect return value in buttonSelect::event()
*
*  Rev 6 - 12/2014 Keegan Morrow - Added setInputInvert() and setOutputInvert() to buttonBoard
*  
*  Rev 7 - 8/2015 Keegan Morrow - Added countPressed()
*
*/

#ifndef __buttonBoard_h_
#define __buttonBoard_h_

#define BUTTONBOARD 7 //revision number
#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include <inttypes.h>

#include "utility/hook.h"

#define buttonReset 0xFF


class buttonBoard: public hook
{
private:
	byte data165Mask;
	byte data595Mask;
	byte clkMask;
	byte latch165Mask;
	byte latch595Mask;
	volatile byte *data165PortPtr;
	volatile byte *data595PortPtr;
	volatile byte *clkPortPtr;
	volatile byte *latch165PortPtr;
	volatile byte *latch595PortPtr;

protected:
	byte *inBuffer;
	byte *outBuffer;
	byte numBoards;
	boolean inputInvert;
	boolean outputInvert;

public:
	boolean autoUpdate; // setting this to false and calling update() can speed up some applications (use caution)
	buttonBoard(byte, byte, byte, byte, byte, byte); // DI, DO, CLK, ILT, OLT, boardCount
	void byteWrite(byte, byte); // boardNumber, byte
	byte byteRead(byte); // boardNumber
	void setLamp(byte, boolean); // buttonNumber, state
	void setLamp(boolean); // state
	boolean getButton(byte); // buttonNumber
	word countPressed();
	word countPressed(byte, byte);
	boolean getLampState(byte); // buttonNumber
	void update();
	byte *getInPtr(); // this will return a pointer to the input buffer, it can also be used like an array
	byte *getOutPtr(); // this will return a pointer to the output buffer, it can also be used like an array
	byte getSize(); // returns the size of the input and output buffers (number of boards)
	void setInputInvert(boolean);
	void setOutputInvert(boolean);
};

/*
 * For buttonSelect and buttonToggle poll() should be called once every 10-50 ms
 * if it is called faster then there will be de-bounce issues, if slower it may miss a press.
 */

class buttonSelect
{
private:
	buttonBoard *bb;
	byte state;
	byte offset;
	byte count;
	boolean eventState;
	void updateLamps();

public:
	/**
	 *
	 */
	boolean defaultState;
	buttonSelect(buttonBoard *, byte, byte, boolean); // &bb, offset, count, resetState
	byte getState();
	void setState(byte); // call .setState(buttonReset) to return to the default state
	boolean poll(); // this will return true if the state has changed, useful for time-outs
	boolean event(); // will return true if there has been a button event since the last call
};

/*
 * If you have a set of toggle buttons you can use the array access from buttonBoard to read a number of buttons in a row:
 * for (byte i=firstButton; i<lastButton; i++)
 * {
 *  if (bb.getLampState(i)) doSomething();
 * }
 */

class buttonToggle
{
private:
	buttonBoard *bb;
	boolean state;
	boolean latch;
	boolean eventState;
	byte buttonNumber;

public:
	buttonToggle(buttonBoard *, byte); // &bb, buttonNumber
	boolean getState();
	void setState(boolean);
	boolean poll(); // this will return true if the state has changed, useful for time-outs
	boolean event(); // will return true if there has been a button event since the last call
};

class buttonToggleNoLamp
{
private:
	buttonBoard *bb;
	byte state;
	byte states;
	boolean latch;
	boolean eventState;
	byte buttonNumber;

public:
	buttonToggleNoLamp(buttonBoard *, byte); // &bb, buttonNumber
	buttonToggleNoLamp(buttonBoard *, byte, byte); // &bb, buttonNumber, number of states
	byte getState();
	void setState(byte);
	boolean poll(); // this will return true if the state has changed, useful for time-outs
	boolean event(); // will return true if there has been a button event since the last call
};
#endif //__buttonBoard_h_
