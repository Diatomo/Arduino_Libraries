/*
 * pwmBoard.cpp
 *
 * PCA9634 8ch PWM Driver
 */

#include "pwmBoard.h"

#if ARDUINO >= 100
#define WIRE_WRITE_FUNCTION write
#else //ARDUINO < 100
#define WIRE_WRITE_FUNCTION send
#endif

/**
 * @brief Constructor for multiple board setups
 * 
 * @param baseAddress Address of the first board.
 * @param numBoards Number of boards with sequential addresses.
 */
pwmBoard::pwmBoard(uint8_t baseAddress, uint8_t numBoards)
{
	this->numBoards = numBoards;
	this->baseAddress = baseAddress;
	autoUpdate = true;
	if (alloc(numBoards) != numBoards)
		while (1); //this is a (kludgey) catch-all for out of memory errors
}

/**
 * @brief Constructor for single board setups.
 * 
 * @param baseAddress Address of the board.
 */
pwmBoard::pwmBoard(uint8_t baseAddress)
{
	this->baseAddress = baseAddress;
	numBoards = 1;
	autoUpdate = true;
	if (alloc(numBoards) != numBoards)
		while (1); //this is a (kludgey) catch-all for out of memory errors
}

/* private function */
uint8_t pwmBoard::alloc(uint8_t n)
{
	levels = (uint8_t *) calloc(n, 8);
	return (levels != NULL) ? n : 0;
}

/* private function */
uint8_t pwmBoard::getAddr(uint8_t addr)
{
	return ((~addr) & 0x0F) | 0x10;
}

/**
 * @brief Starts the i2c bus and initializes the board. This must be called before any other member functions.
 * 
 */
void pwmBoard::start()
{
	Wire.begin();
	for (uint8_t i = 0; i < numBoards; i++)
	{
		Wire.beginTransmission(getAddr(baseAddress + i));
		Wire.WIRE_WRITE_FUNCTION(0b10000000); // get access to control register 0x00 (MODE1)
		Wire.WIRE_WRITE_FUNCTION(0b10000001); // set 0x00 (MODE1) to auto inc bits to 0, enable respond to all-call
		Wire.WIRE_WRITE_FUNCTION(0b00011101); // set 0x01 (MODE2) to dimming mode, inverted output (for external FETs), output change on ACK
		Wire.endTransmission();
		delay(1);
		Wire.beginTransmission(getAddr(baseAddress + i));
		Wire.WIRE_WRITE_FUNCTION(0b10001100); // get access to control register 0x0c (LEDOUT0)
		Wire.WIRE_WRITE_FUNCTION(0b10101010); // in register 0x0c (LEDOUT0) set all to mode 10 (output using PWMx registers)
		Wire.WIRE_WRITE_FUNCTION(0b10101010); // in register 0x0d (LEDOUT1) set all to mode 10 (output using PWMx registers)
		Wire.endTransmission();
	}

}

/**
 * @brief Identical to send(), only here for backwards compatibility. Use send() for all new code.
 */
void pwmBoard::send()
{
	update();
}

/**
 * @brief Send the current output buffer to all boards.
 */
void pwmBoard::update()
{
	uint8_t *levPtr = levels;
	for (uint8_t i = 0; i < numBoards; i++)
	{
		Wire.beginTransmission(getAddr(baseAddress + i));
		Wire.WIRE_WRITE_FUNCTION(0b10100010);
		Wire.WIRE_WRITE_FUNCTION(levPtr, 8);
		Wire.endTransmission();
		levPtr += 8;
	}
}

/**
 * @brief Sets the output level of one output channel and if autoUpdate==true, sends the values to the boards.
 * 
 * @param index Output channel to set
 * @param level Level to set the channel to [0..255]
 */
void pwmBoard::setLevel(uint8_t index, uint8_t level)
{
	if (index < (numBoards * 8))
		*(levels + index) = level;
	if (autoUpdate)
		update();
}

/**
 * @brief Sets the output level to all outputs and if autoUpdate==true, sends the values to the boards.
 * 
 * @param level Level to set the outputs [0..255]
 */
void pwmBoard::setLevel(uint8_t level)
{
	memset(levels, level, (numBoards * 8));
	if (autoUpdate)
		update();
}

/**
 * @brief Sets the output level of one output channel and sends the values to the boards. This function is for backward compatibility.
 * @details [long description]
 * 
 * @param index Output channel to set
 * @param level Level to set the channel to [0..255]
 */
void pwmBoard::setLevelSend(uint8_t index, uint8_t level)
{
	setLevel(index, level);
	update();
}

/**
 * @brief Sets the output level to all outputs and sends the values to the boards. This function is for backward compatibility.
 * 
 * @param level Level to set the outputs [0..255]
 */
void pwmBoard::setLevelSend(uint8_t level)
{
	setLevel(level);
	update();
}

/**
 * @brief Get a pointer to the output buffer.
 * @return Pointer to the output buffer.
 */
byte * pwmBoard::getPtr()
{
	return levels;
}
