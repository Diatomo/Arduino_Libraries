#ifndef __digits_h_
#define __digits_h_

#define DIGITS 7 //revision number
#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif
#include "SegSerial.h"//custom serial library


//static uint8_t _digits_mapToSegs(uint8_t);
//static uint8_t _digits_iToSegs(uint32_t, uint8_t *, uint8_t, uint8_t);

//forward declaration;
struct segment;
typedef segment Segment;
class DigitGroup;


/*
 *	Struct display
 *		Collections object for each individual segmented display
 *		contains : {number,next, prev}
 */
struct segment{
	uint8_t number; //0b000;
	uint8_t address; //0-255;
	uint8_t groupID;
	segment* next;
};


/**
 *
 * Class digitLink
 *
 * 	This class is responsible for managing the data structure that
 * 	goes along with manipulating the segmented displays.
 *
 *
 */
class DigitLink{
	protected:
		//Segment addLink(uint8_t, uint8_t, Segment, Segment);
		Segment* head = NULL; //this is the first segment
		Segment* groupHead = NULL;
	public:
		digitLink();
		//Segment* addGroup(uint8_t, uint8_t);
		//Segment* getPointerHead();
		//Segment getHead();
		//Segment getTail();
};



/*
 *
 *
 *	Class DigitGroup	
 *		handles communication protocols to get the segmented
 *		displays to change number and/or brightness.
 *
 */
class DigitGroup{
	private:
		void sendByte(uint8_t);
		SegSerial mySerial;
		//uint8_t numDigits;
		//Segment* head;
		uint8_t segCalc(uint32_t, uint8_t);
		uint8_t digitToSeg(uint32_t, uint8_t);
	public:		
		uint8_t numDigits;
		Segment* head;
		DigitGroup(Segment*, uint8_t, uint8_t); 
		
		boolean autoUpdate; 
		void update();
		void turnOnBrightness();
		
		//set && copy
		void setDigit(uint8_t, uint8_t, boolean); 
		
		//displays a number on the seven segment display (group's digits);
		void segDisp(uint32_t, uint8_t);
		//boolean segDispSign(int32_t, uint8_t);

		//chase animation on the seven segment display (group's digits);
		//void chaseAnimation();
		//void chaseAnimation(uint8_t);
		//void chaseAnimation8();
		//void chaseAnimation8(uint8_t);
};

/*
 *
 *	Class digits	
 *		handles communication protocols to get the segmented
 *		displays to change number and/or brightness.
 *
 */
class Digits : public DigitLink{
	private:
		void sendByte(uint8_t);
		SegSerial mySerial;
		uint8_t segCalc(uint32_t, uint8_t);
		uint8_t digitToSeg(uint32_t, uint8_t);
	public:
		//constructor
		Digits();
		Digits(uint8_t); 
		
		//updates
		boolean autoUpdate; 
		void update();
	
		//adding a group
		DigitGroup* addGroup(uint8_t, uint8_t);
	//	void copySection(DigitGroup*, DigitGroup*);
		void setDigit(Segment*, uint8_t, uint8_t, boolean); 
		
		//meta data about the segment displays
		Segment* getPtr(); 
	//	uint32_t getSize(); 
		uint8_t getNumDigits();
		
		//display a number on the seven segment display.
		void segDisp(uint32_t, uint8_t);
		boolean segDispSign(int32_t, uint8_t);
		
		//chase animation on the seven segment display (all digits);
	//	void chaseAnimation();
	//	void chaseAnimation(uint8_t);
	//	void chaseAnimation8();
	//	void chaseAnimation8(uint8_t);
};

#endif //__digits_h_
