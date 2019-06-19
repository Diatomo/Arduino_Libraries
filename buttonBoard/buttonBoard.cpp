#include "buttonBoard.h"


/**
 *  \brief Allocates memory for input and output buffers, sets direction registers on IO pins.
 *
 *  \param data595Pin Data out pin, connect to DI on buttonBoard
 *  \param data165Pin Data in pin, connect to DO on buttonBoard
 *  \param clockPin Clock pin, connect to CLK on buttonBoard
 *  \param latch165Pin Input latch pin, connect to ILT on buttonBoard
 *  \param latch595Pin Output latch pin, connect to OLT on buttonBoard
 *  \param numBoards Number of boards in use
 *  \return Return_Description
 *
 */
buttonBoard::buttonBoard(byte data595Pin, byte data165Pin, byte clockPin,
                         byte latch165Pin, byte latch595Pin, byte numBoards)
{
	data165PortPtr = portInputRegister(digitalPinToPort(data165Pin));
	data595PortPtr = portOutputRegister(digitalPinToPort(data595Pin));
	clkPortPtr = portOutputRegister(digitalPinToPort(clockPin));
	latch165PortPtr = portOutputRegister(digitalPinToPort(latch165Pin));
	latch595PortPtr = portOutputRegister(digitalPinToPort(latch595Pin));

	data165Mask = digitalPinToBitMask(data165Pin);
	data595Mask = digitalPinToBitMask(data595Pin);
	clkMask = digitalPinToBitMask(clockPin);
	latch165Mask = digitalPinToBitMask(latch165Pin);
	latch595Mask = digitalPinToBitMask(latch595Pin);

	this->numBoards = numBoards;

	inBuffer = (byte *) calloc(numBoards, 1);
	if (inBuffer == NULL)
		while (1);
	outBuffer = (byte *) calloc(numBoards, 1);
	if (outBuffer == NULL)
		while (1);

	pinMode(data165Pin, INPUT);
	pinMode(data595Pin, OUTPUT);
	pinMode(latch165Pin, OUTPUT);
	pinMode(latch595Pin, OUTPUT);
	pinMode(clockPin, OUTPUT);
	digitalWrite(data165Pin, HIGH);
	digitalWrite(clockPin, LOW);
	digitalWrite(latch595Pin, LOW);
	digitalWrite(latch165Pin, HIGH);

	inputInvert = false;
	outputInvert = false;
	autoUpdate = true;
}

/**
 *  @brief Sets the state of a lamp output
 *
 *  @param buttonNumber Button number
 *  @param state        Lamp state, true = on
 *
 */
void buttonBoard::setLamp(byte buttonNumber, boolean state)
{
	byte byteNumber = buttonNumber >> 3;
	byte bitNumber = buttonNumber - (byteNumber << 3);
	if (byteNumber >= numBoards)
	{
		return;
	}
	bitWrite(*(outBuffer + byteNumber), bitNumber, state);
	if (autoUpdate)
	{
		update();
	}
}

/**
 *  @brief Sets the state of all lamps
 *
 *  @param state Lamp state, true = on
 *
 */
void buttonBoard::setLamp(boolean state)
{
	if (state)
	{
		memset(outBuffer, 0xFF, numBoards);
	}
	else
	{
		memset(outBuffer, 0x00, numBoards);
	}
	if (autoUpdate)
	{
		update();
	}
}

/**
 *  @brief Gets the state of a lamp
 *
 *  @param buttonNumber Button number
 *  @return State of the lamp true = on
 *
 */
boolean buttonBoard::getLampState(byte buttonNumber)
{
	byte byteNumber = buttonNumber >> 3;
	byte bitNumber = buttonNumber - (byteNumber << 3);
	return bitRead(*(outBuffer + byteNumber), bitNumber);
}

/**
 *  @brief Get the state of a button
 *
 *  @param buttonNumber Button number
 *  @return State of the button, true = pressed
 *
 */
boolean buttonBoard::getButton(byte buttonNumber)
{
	byte byteNumber = buttonNumber >> 3;
	byte bitNumber = buttonNumber - (byteNumber << 3);
	if (autoUpdate)
	{
		update();
	}
	return bitRead(*(inBuffer + byteNumber), bitNumber);
}

/**
 * @brief Count the total number of buttons being pressed
 *
 * @return Total number of pressed buttons
 */
word buttonBoard::countPressed()
{
	word result = 0;
	if (autoUpdate)
	{
		update();
	}
	for (byte i = 0; i < numBoards; i++)
	{
		result += __builtin_popcount(*(inBuffer + i));
	}
	return result;
}

/**
 * @brief Count the number of buttons being pressed in a range
 * 
 * @param offset first button in the range
 * @param count number of buttons in the range
 * 
 * @return Number of buutons pressed
 */
word buttonBoard::countPressed(byte offset, byte count)
{
	word result = 0;
	if (autoUpdate)
	{
		update();
	}
	for (byte i = offset; i < (offset + count); i++)
	{
		byte byteNumber = i >> 3;
		byte bitNumber = i - (byteNumber << 3);
		result += bitRead(*(inBuffer + byteNumber), bitNumber) ? 1 : 0;
	}
	return result;
}

/**
 *  @brief Low level access, writes a byte value to the lamp outputs of an individual board.
 *
 *  @param board Board number
 *  @param val   Byte to be written to the lamp outputs (0xFF would turn the lamps on)
 *
 */
void buttonBoard::byteWrite(byte board, byte val)
{
	if (board >= numBoards)
	{
		return;
	}
	*(outBuffer + board) = val;
	if (autoUpdate)
	{
		update();
	}
}

/**
 *  @brief Low level access, reads the inputs of an individual board to a byte value.
 *
 *  @param board Board number
 *  @return Byte read from the button inputs (0x00 would be no buttons pressed)
 *
 */
byte buttonBoard::byteRead(byte board)
{
	if (autoUpdate)
	{
		update();
	}
	return *(inBuffer + board);
}

/**
 *  @brief Gets a pointer to the input buffer
 *
 *  @return Pointer to the input buffer
 *
 */
byte *buttonBoard::getInPtr()
{
	return inBuffer;
}

/**
 *  @brief Gets a pointer to the output buffer
 *
 *  @return Pointer to the output buffer
 *
 */
byte *buttonBoard::getOutPtr()
{
	return outBuffer;
}

/**
 *  @brief Gets the size of the input and output buffers. Same as the number of boards.
 *
 *  @return Number of elements in the buffer
 *
 */
byte buttonBoard::getSize()
{
	return numBoards;
}

/**
 *  @brief Set if the hardware inputs are electrically inverted (default is not inverted). This would normally be called in setup().
 *  @param inputInvert set to true to invert the inputs
 *
 */
void buttonBoard::setInputInvert(boolean inputInvert)
{
	this->inputInvert = inputInvert;
}

/**
 *  @brief Set if the hardware outputs are electrically inverted (default is not inverted). This would normally be called in setup().
 *  @param outputInvert Set to true to invert the outputs
 *
 */
void buttonBoard::setOutputInvert(boolean outputInvert)
{
	this->outputInvert = outputInvert;
}

/**
 *  @brief Force an update of the input and output buffers. This is called automatically if autoUpdate is set to true.
 *
 */
void buttonBoard::update()
{
	byte inTemp;
	byte outTemp;

	*latch165PortPtr |= latch165Mask; // latch the input registers

	for (byte i = 0; i < numBoards; i++)
	{
		inTemp = 0;
		if (outputInvert) { outTemp = ~(*(outBuffer + ((numBoards - 1) - i))); }
		else { outTemp = *(outBuffer + ((numBoards - 1) - i)); }
		for (byte j = 0; j < 8; j++)
		{
			if (outTemp & 0x80)
			{
				*data595PortPtr |= data595Mask;
			}
			else
			{
				*data595PortPtr &= ~data595Mask;
			}
			outTemp <<= 1;
			inTemp <<= 1;
			inTemp |= ((*data165PortPtr & data165Mask) == data165Mask);
			*clkPortPtr |= clkMask;
			*clkPortPtr &= ~clkMask;
		}
		if (inputInvert) { *(inBuffer + i) = inTemp; }
		else { *(inBuffer + i) = ~inTemp; }
	}
	*latch165PortPtr &= ~latch165Mask;
	*latch595PortPtr |= latch595Mask; // latch the output registers
	*latch595PortPtr &= ~latch595Mask;

	callHook();
}

/**
 *  @brief Toggle functionality for an individual button
 *
 *  @param bb           Pointer to buttonBoard object
 *  @param buttonNumber Button number to manage
 *
 */
buttonToggle::buttonToggle(buttonBoard *bb, byte buttonNumber)
{
	this->bb = bb;
	this->buttonNumber = buttonNumber;
	state = false;
}

/**
 *  @brief Get the state of the toggled button.
 *
 *  @return State of the button, true = active
 *
 */
boolean buttonToggle::getState()
{
	return state;
}

/**
 *  @brief Set the state of the toggled button.
 *
 *  @param state State of the button, true = active
 *
 */
void buttonToggle::setState(boolean state)
{
	this->state = state;
	bb->setLamp(buttonNumber, state);
	if (bb->autoUpdate == false)
	{
		bb->update();
	}
}

/**
 *  @brief Poll the button to see if there was a press. This should be called every 10-50ms.
 *
 *  @return True if there was a state change
 *
 */
boolean buttonToggle::poll()
{
	boolean buttonState;
	boolean returnState = false;
	if (bb->autoUpdate == false)
	{
		bb->update();
	}
	buttonState = bb->getButton(buttonNumber);
	if (buttonState && !latch)
	{
		state = state ? false : true;
		latch = true;
		returnState = true;
		eventState = true;
	}
	if (!buttonState)
	{
		latch = false;
	}
	bb->setLamp(buttonNumber, state);
	if (bb->autoUpdate == false)
	{
		bb->update();
	}
	return returnState;
}

/**
 *  @brief Check if there has been a state change event
 *
 *  @return True if there has been a state change since the last call
 *
 */
boolean buttonToggle::event()
{
	if (eventState)
	{
		eventState = false;
		return true;
	}
	else
	{
		return false;
	}
}

/**
 *  @brief Selector functionality for a group of buttons
 *
 *  @param bb           Pointer to buttonBoard object
 *  @param offset       First button number
 *  @param count        Number of buttons in the group
 *  @param defaultState State of the buttons in the reset state, true = on
 *
 */
buttonSelect::buttonSelect(buttonBoard *bb, byte offset, byte count,
                           boolean defaultState)
{
	this->bb = bb;
	this->offset = offset;
	this->count = count;
	this->defaultState = defaultState;
	state = buttonReset;
}

/**
 *  @brief Gets the current state of the button group
 *
 *  @return Current state of the button group, buttonReset = reset state, # = button number in the group
 *
 */
byte buttonSelect::getState()
{
	return (state == buttonReset) ? buttonReset : (state - offset);
}

/**
 *  @brief Set the state of the button group.
 *
 *  @param state 'buttonReset' or button number
 *
 */
void buttonSelect::setState(byte state)
{
	this->state = (state == buttonReset) ? buttonReset : (state + offset);
	updateLamps();
}

/**
 *  @brief Poll the buttons to see if there was a press. This should be called every 10-50ms.
 *
 *  @return True if there was a state change
 *
 */
boolean buttonSelect::poll()
{
	byte stateTemp = buttonReset;
	if (bb->autoUpdate == false)
	{
		bb->update();
	}
	for (byte i = offset; i < (offset + count); i++)
	{
		if (bb->getButton(i) == true)
		{
			stateTemp = i;
		}
	}
	if (stateTemp == buttonReset)
	{
		return false;
	}
	else if (state != stateTemp)
	{
		state = stateTemp;
		updateLamps();
		eventState = true;
		return true;
	}
	else
	{
		updateLamps();
		return false;
	}
}

/**
 *  @brief Check if there has been a state change event
 *
 *  @return True if there has been a state change since the last call to this function
 *
 */
boolean buttonSelect::event()
{
	if (eventState)
	{
		eventState = false;
		return true;
	}
	else
	{
		return false;
	}
}

void buttonSelect::updateLamps()
{
	if (state == buttonReset)
	{
		for (byte i = offset; i < (offset + count); i++)
		{
			bb->setLamp(i, defaultState);
		}
	}
	else
	{
		for (byte i = offset; i < (offset + count); i++)
		{
			if (state == i)
			{
				bb->setLamp(i, true);
			}
			else
			{
				bb->setLamp(i, false);
			}
		}
	}
	if (bb->autoUpdate == false)
	{
		bb->update();
	}
}

buttonToggleNoLamp::buttonToggleNoLamp(buttonBoard *bb, byte buttonNumber)
{
	this->bb = bb;
	this->buttonNumber = buttonNumber;
	states = 2;
	state = false;
}

buttonToggleNoLamp::buttonToggleNoLamp(buttonBoard *bb, byte buttonNumber, byte states)
{
	this->bb = bb;
	this->buttonNumber = buttonNumber;
	this->states = states;
	state = false;
}

byte buttonToggleNoLamp::getState()
{
	return state;
}

void buttonToggleNoLamp::setState(byte state)
{
	this->state = state % states;
}

boolean buttonToggleNoLamp::poll()
{
	boolean buttonState;
	boolean returnState = false;
	if (bb->autoUpdate == false)
	{
		bb->update();
	}
	buttonState = bb->getButton(buttonNumber);
	if (buttonState && !latch)
	{
		if (states == 2)
		{
			state = state ? false : true;
		}
		else if (++state >= states)
		{
			state = 0;
		}
		latch = true;
		returnState = true;
		eventState = true;
	}
	if (!buttonState)
	{
		latch = false;
	}
	if (bb->autoUpdate == false)
	{
		bb->update();
	}
	return returnState;
}

boolean buttonToggleNoLamp::event()
{
	if (eventState)
	{
		eventState = false;
		return true;
	}
	else
	{
		return false;
	}
}
