//updated on 12/2011 for compatibility with arduino 100 - Keegan

#ifndef smooth_h
#define smooth_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include<inttypes.h>

/*
 * this library can use memory quickly, be careful with the
 * number of elements (two bytes per)
 * 
 */

class smooth
{
 private:
	byte arraySize;
	int  amountToBuffer;
	unsigned int  *dataArray;
 public:
	 unsigned long  smoothedData;
     //unsigned int  dataArray[MAX]; // array to store the beat values
     bool bufferFull;              //boolean to say if we've filled the array
     uint8_t bufferedDataCount;    //first set of beat coutns that fill the buffer -- index for dataArray 
	 
	 smooth(int); 
     void smoothData(long);
	 void clearData();
	 
};
#endif

