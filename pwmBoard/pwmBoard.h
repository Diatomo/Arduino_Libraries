/*
 * pwmBoard.h
 *
 * PCA9634 8ch PWM Driver
 */

#ifndef __pwmBoard_h_
#define __pwmBoard_h_

#define PWMBOARD 4 //revision number
#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include <inttypes.h>
#include "../Wire/Wire.h"

/**
 * @brief Hardware interface class for the pwmBoard PCA9634 based PWM dimmer board.
 * @author Keegan Morrow
 * @version 4
 * @details Revision history:
 * 
 * r2 - 12/2011 - KM - update for compatibility with arduino 100
 * 
 * r3 - 2/2012 - KM - added support for multiple boards per class instance 
 * 
 * r4 - 8/2012 - KM - added update() and autoUpdate for similarity with other libraries
 * 
 * 
 */
class pwmBoard
{
private:
	uint8_t baseAddress;
	uint8_t numBoards;
	uint8_t alloc(uint8_t);
	uint8_t getAddr(uint8_t);
	uint8_t *levels;

public:
	/**
	 * If autoUpdate is set to true, update() is called automatically from setLevel()
	 */
	boolean autoUpdate;
	pwmBoard(uint8_t, uint8_t); // baseAddress, count
	pwmBoard (uint8_t); // address
	void start();
	void setLevel(uint8_t, uint8_t); // output, level
	void setLevel(uint8_t); //level (to all outputs)
	void setLevelSend(uint8_t, uint8_t); // depreciated, use setLevel and autoUpdate
	void setLevelSend(uint8_t); // depreciated, use setLevel and autoUpdate
	byte* getPtr(); //use this function to access the levels array like this: className.getPtr()[channel] = level;
	void send(); // only here for backwards compatibility, use update()
	void update();
};

#endif //__pwmBoard_h_
