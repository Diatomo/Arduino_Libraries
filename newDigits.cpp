
#include "newDigits.h"

//=====================================================
//					Digit Link
//
//			Data Structure and backend of
//			the seven segment display.
//=====================================================


//constructor
DigitLink::digitLink(){
}

/*
 *
 *	FxN :: getPointerHead
 *		+returns the head pointer of the linked list.
 *
 */
/*
Segment* DigitLink::getPointerHead(){
	return head;

}
*/
/*
 *
 *	FxN :: getHead
 *		+returns the head node of the linked list.
 *
 */

/*
Segment DigitLink::getHead(){
	return *head;
}
*/

//=====================================================
//					Digit Group
//=====================================================


DigitGroup::DigitGroup(Segment* node, uint8_t serialComm, uint8_t groupSize){
	head = node;
	mySerial.begin(250000);
	mySerial.setTX(serialComm);
	numDigits = groupSize;
	autoUpdate = true;
}


void DigitGroup::sendByte(uint8_t data){
	mySerial.write(data);
}

void DigitGroup::turnOnBrightness(){
	Segment* curr = head;
	while (curr != NULL){
		mySerial.write(curr->address);
		mySerial.write(0x44);
		mySerial.write(0x9F);
		mySerial.write((curr->address + 0x44 + 0x9F) % 64) + 0XC0;
		curr = curr->next;
	}
}


/*
 *
 *	FxN :: update
 *	
 *	@brief :: updates the seven segmented displays.
 *
 *
 */
void DigitGroup::update(){
	Segment* curr = head;
	uint8_t mod = 64;
	uint8_t eof = 0xC0;
	uint8_t command = 0x43;
	while (curr != NULL){
		mySerial.write(curr->address);
		mySerial.write(0x43); 
		mySerial.write(curr->number + 0x80);
		uint8_t checksum = ((curr->address + 0x43 + curr->number + 0x80) % 64) + 0xC0;
		mySerial.write(checksum);
		curr = curr->next;
	}

	/*
	while (curr != NULL){
		mySerial.write(curr->address);
		mySerial.write(command); 
		mySerial.write(curr->number + 0x80);
		uint8_t checksum = ((curr->address + command + curr->number + 0x80) % mod) + eof;
		mySerial.write(checksum);
		curr = curr->next;
	}
	delay(1000);
	Serial.println("calling lib update");
	*/
}
		/*
		delay(500);
		printData(groupA);
		mySerial.write(groupA->head->address);
		mySerial.write(0x43); 
		mySerial.write(groupA->head->number + 0x80);
		uint8_t checksum = ((groupA->head->address + 0x43 + groupA->head->number + 0x80) % 64) + 0xC0;
		mySerial.write(checksum);
		*/

void DigitGroup::setDigit(uint8_t segment, uint8_t num, boolean state = false){

	//iterate through linked list for node
	Segment* curr = head;
	if (segment > numDigits) { return; }
	for (uint8_t i = 0; i < segment; i++){
		curr = curr->next;
	}

	//set number
	//curr->number = _digits_mapToSegs(num);
	
	//set decimal
	
	/*
	if (state){
		curr->number |= 0x01;
	}
	else{
		curr->number &= 0xFE;
	}
	*/

	//update
	if (autoUpdate) { update(); }
}

//----------------------------------
//Segmented Displays -- DigitGroup
//----------------------------------

/*
 *
 *
 *	FxN :: digitToSeg
 *
 * 		@param number -> [uint32_t] inputted number
 *		@param numDig -> [uint8_t] number of digits
 *		@param fill   -> [uint8_t] fill (blank digit)
 *
 *	+brief :: addresses each place in a number to a segmented display else makes them blank.
 *
 */
uint8_t DigitGroup::digitToSeg(uint32_t number, uint8_t numDig){

	uint8_t blank = 0;
	uint8_t digitCount = 0;
	Segment* curr = head;
	if (number == 0){
		if (numDig > 1){
			curr->number = blank;
			digitCount = 1;
		}
	}
	else{
		for (uint8_t pos = 0; pos < numDig; pos++){
			if (number != 0){
				curr->number = number % 10;
				number /= 10;
				digitCount++;
			}
			else{
				curr->number = blank;
			}
			if (curr->next != NULL){
				curr = curr->next;
			}
		}
	}
	return digitCount;
}


/*
 *
 *	FxN :: segCalc
 *		@param number -> [uint32_t] inputted number
 *		@param dpPos  -> [uint8_t] decimal point position
 *
 *	+brief :: calculates segment display.
 *
 */
uint8_t DigitGroup::segCalc(uint32_t number, uint8_t dpPos){
	uint8_t digitsUsed = digitToSeg(number, numDigits);
	Segment* curr = head;
	/*
	if (dpPos != 0 && dpPos < numDigits){
		uint8_t counter = 0;
		while (counter != dpPos){
			curr = curr->next;
			counter++;
		}
		curr-> number |= 0x01; //decimal value
	}
	*/
	//return (digitsUsed >= dpPos) ? digitsUsed : dpPos;
	return 0;
}

/*
 *
 *	FxN :: segDisp
 *		@param number -> [uint32_t] designated number
 *
 *	+brief :: calculates number without a decimal point
 *
 */
void DigitGroup::segDisp(uint32_t number, uint8_t dpPos=0){
	segCalc(number, dpPos);
	if (autoUpdate) { update(); }
}

/*

// signed number, position of decimal point
boolean DigitGroup::segDispSign(int32_t number, uint8_t dpPos=0){
	boolean signOverflow = false;
	uint32_t uNumber = abs(number);
	uint8_t signPos = segCalc(uNumber,dpPos);
	uint8_t dash = 1;//TODO this will change when we get some new proto in

	if (number != uNumber){
		if (signPos >= numDigits){
			signOverflow = true;
		}
		else{
			head->number |= dash; //TODO this will change.
		}
	}

	// Update &| return
	if (autoUpdate) { update(); }
	return signOverflow;
}
*/

//--------------------------------
//CHASE ANIMATIONS -- DigitGroup
//--------------------------------

/*
 *
 *	FxN :: chaseAnimation
 *	
 *	+brief:
 *		Displays a spinning animation (will advance only when called)
 *
 *
 */

/*
void DigitGroup::chaseAnimation(){
	static uint8_t pos;
	chaseAnimation(pos++);
	if (pos > 5){
		pos = 0;
	}
}

// jump to a particular point in chaseAnimation (input is modulo 6)
void DigitGroup::chaseAnimation(uint8_t pos){

	//set chaseAnimation
	if (autoUpdate) {update();}

}
// display a figure eight animation (will advance when called)
void DigitGroup::chaseAnimation8(){
	static uint8_t pos;
	chaseAnimation8(pos++);
	if (pos > 7){
		pos = 0;
	}
}

// jump to a particular point in chaseAnimation8 (input is modulo 8)
void DigitGroup::chaseAnimation8(uint8_t pos){
	uint8_t const pattern[8] =
	{ 0x02, 0x04, 0x80, 0x20, 0x10, 0x08, 0x80, 0x40 };

}
*/
//=====================================================
//					 Digits
//=====================================================

static const uint8_t dash = 0x80;
static const uint8_t blank = 0x0A;


/* NOT USED STUFF */
/*
 *
 * digits *digitsPtr;
 * uint8_t *digPtr;
 * uint8_t numDigits;
 *
 */

uint8_t _digits_mapToSegs(uint8_t);

/*
 *
 *	Constructor
 *		
 *
 */
Digits::Digits(uint8_t serialPin){
		mySerial.setTX(serialPin); //createSoftware Serial
		autoUpdate = true;
		//update();
}

Digits::Digits(){

}

/*
 *
 *	FxN :: getPtr
 *		@returns head -> [Segment*] the head pointer of the linked list.
 *
 * 	@brief :: Gets the head of the linked list.
 *
 *
 */
/*
Segment* Digits::getPtr(){
	return head;
}
*/
/*
 *
 *	FxN :: sendByte
 *		@param data -> [uint8_t] byte of data
 *
 *	@brief :: writes a byte using a custom serial library.
 *
 */
void Digits::sendByte(uint8_t data){
	mySerial.write(data);
}

/*
 *
 *	FxN :: addGroup
 *		@address :: address of the 7-seg <check DIP switch> +Should be contiguous per group.
*		@groupSize :: size of the group
 *	Adds a group of 7segs to the linked list. 
 *	
 *		+returns a pointer to the head of the group. 
 *
 *
 */
DigitGroup* Digits::addGroup(uint8_t address, uint8_t groupSize){
	uint8_t static group = 0;
	Segment* groupPointer = NULL;
	static Segment* prevNode = NULL;
	if (groupSize != 0){
		for (uint8_t i = 0; i < groupSize; i++){
			if (i == 0){
				Segment* currNode = new Segment;
				currNode->number = 0;
				currNode->address = address;
				currNode->groupID = group;
				if (head == NULL){
					head = currNode;
				}
				groupHead = currNode;
				if (prevNode != NULL){
					prevNode->next = currNode;
				}
				prevNode = currNode;
			}
			else{
				Segment* currNode = new Segment;	
				currNode->number = 0;
				currNode->address = address;
				currNode->groupID = group;
				prevNode->next = currNode;
				prevNode = currNode;
				currNode->next = NULL;
			}
			address++;
		}
		group++;
	}
	DigitGroup* temp = new DigitGroup(groupHead, this->mySerial.getTxPin(), groupSize);
	return temp;
}

/*
 *
 *	FxN :: update
 *	
 *	@brief :: updates the seven segmented displays.
 *
 *
 */
void Digits::update(){
	/*
	sendByte(startBit);
	Segment* curr = head;
	while (curr != NULL){
		sendByte(curr->address);
		sendByte(curr->number);
		curr = curr->next;
	}
	sendByte(stopBit);
	*/
}


/*
 *
 *	FxN :: size
 *		@returns size -> number of bytes of the linked list
 *
 * 	@brief :: gets the size (in bytes) of the total number of digits
 *
 */
/*
uint32_t Digits::getSize(){
	uint32_t size = 0;
	Segment* curr = head;
	while (curr != NULL){
		size += sizeof(curr);
		curr = curr->next;
	}
	return size;
}
*/

/*
 *
 *	FxN :: getNumDigits
 *		@param group -> [Segment*] group of 1 or more digits
 *		@returns numDigits -> [uint8_t] unsigned byte of the number of digits in group
 * 	
 *	@brief :: retreives the number of digits in a group.
 *
 */
uint8_t Digits::getNumDigits(){
	uint8_t numDigits = 0;
	Segment* curr = head;
	while (curr != NULL){
		curr = curr->next;
		numDigits++;
	}
	return numDigits;
}



/*
 *
 *	FxN :: copySection
 *		@groupA -> [Segment*] group of 1 or more digits (source)
 *		@groupB -> [Segment*] group of 1 or more digits (destination)
 *
 * 	@brief :: This copies numbers from one group to another.
 *
 */
/*
void Digits::copySection(DigitGroup* groupA, DigitGroup* groupB){
	if (groupA->head->groupID != groupB->head->groupID){
		if (groupA->numDigits == groupB->numDigits){
			Segment* currA = groupA->head;
			Segment* currB = groupB->head;
			uint8_t tempAID = currA->groupID;
			uint8_t tempBID = currB->groupID;
			while (tempAID == currA->groupID && tempBID == currB->groupID){
				currB->number = currA->number;
				currA = currA->next;
				currB = currB->next;
			}
		}
	}
}
*/

void Digits::setDigit(Segment* group, uint8_t segment, uint8_t num, boolean state){

	//iterate through linked list for node
	Segment* curr = group;
	if (segment > getNumDigits()) { return; }
	for (uint8_t i = 0; i < segment; i++){
		curr = curr->next;
	}

	//set number
	curr->number = _digits_mapToSegs(num);
	
	//set decimal
	if (state){
		curr->number |= 0x01;
	}
	else{
		curr->number &= 0xFE;
	}

	//update
	if (autoUpdate) { update(); }
}

//----------------------------------
//Segmented Displays -- Digits
//----------------------------------

/*
 *
 *
 *	FxN :: digitToSeg
 *
 * 		@param number -> [uint32_t] inputted number
 *		@param numDig -> [uint8_t] number of digits
 *		@param fill   -> [uint8_t] fill (blank digit)
 *
 *	+brief :: addresses each place in a number to a segmented display else makes them blank.
 *
 */
uint8_t Digits::digitToSeg(uint32_t number, uint8_t numDig){

	uint8_t blank = 0;
	uint8_t digitCount = 0;
	Segment* curr = head;
	if (number == 0){
		if (numDig > 1){
			curr->number = blank;
			digitCount = 1;
		}
	}
	else{
		for (uint8_t pos = 0; pos < numDig; pos++){
			if (number != 0){
				curr->number = number % 10;
				number /= 10;
				digitCount++;
			}
			else{
				curr->number = blank;//TODO blank digit!!
			}
			curr = curr->next;
		}
	}
	return digitCount;
}

/*
 *
 *	FxN :: segCalc
 *
 *		@param number -> [uint32_t] inputted number
 *		@param dpPos  -> decimal point position
 *
 *	+brief :: calculates the display on a segmented display.
 *
 */
uint8_t Digits::segCalc(uint32_t number, uint8_t dpPos){
	uint8_t digitsUsed = digitToSeg(number, getNumDigits());
	Segment* curr = head;
	if (dpPos != 0 && dpPos < getNumDigits()){
		uint8_t counter = 0;
		while (counter != dpPos){
			curr = curr->next;
		}
		curr-> number |= 0x01; //decimal value
	}
	return (digitsUsed >= dpPos) ? digitsUsed : dpPos;
}

/*
 *
 *	FxN :: segDisp
 *		@param number -> [uint32_t] designated number
 *
 * 	+brief :: calculates with a decimal point in a particular position
 *
 */
void Digits::segDisp(uint32_t number, uint8_t dpPos=0){
	segCalc(number, dpPos);
	if (autoUpdate) { update(); }
}


/*

 *
 *	FxN :: segDispSign
 *		@param number -> [int32_t] input number
 *		@param dpPos  -> [uint8_t] dpPos
 *
 * +brief :: set a dash in front of a negative number
 *
 */
/*
boolean Digits::segDispSign(int32_t number, uint8_t dpPos=0){
	//declarations
	boolean signOverflow = false;
	uint32_t uNumber = abs(number);
	uint8_t signPos = segCalc(uNumber,dpPos);
	
	if (number != uNumber){
		if (signPos >= getNumDigits()){
			signOverflow = true;
		}
		else{
			head->number |= dash; 
		}
	}
	// Update &| return
	if (autoUpdate) { update(); }
	return signOverflow;
}
*/


//----------------------------------
//chaseAnimations -- Digits
//----------------------------------
/*
 *
 *	FxN :: chaseAnimation
 *	
 *	+brief:
 *		Displays a spinning animation (will advance only when called)
 *
 *
 */
/*
void Digits::chaseAnimation(){
	static uint8_t pos;
	chaseAnimation(pos++);
	if (pos > 5){
		pos = 0;
	}
}

// jump to a particular point in chaseAnimation (input is modulo 6)
void Digits::chaseAnimation(uint8_t pos){

	//set chaseAnimation
	if (autoUpdate) {update();}

}
// display a figure eight animation (will advance when called)
void Digits::chaseAnimation8(){
	static uint8_t pos;
	chaseAnimation8(pos++);
	if (pos > 7){
		pos = 0;
	}
}

// jump to a particular point in chaseAnimation8 (input is modulo 8)
void Digits::chaseAnimation8(uint8_t pos){

	uint8_t const pattern[8] =
	{ 0x02, 0x04, 0x80, 0x20, 0x10, 0x08, 0x80, 0x40 };

}
*/

//===============================================================
//===============================================================
//===============================================================



