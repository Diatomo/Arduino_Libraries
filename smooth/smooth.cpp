//updated on 12/2011 for compatibility with arduino 100 - Keegan
// changed the memory allocation mode to remove the buffer size constraint 

#include "smooth.h"


smooth::smooth(int arraySize)
{
	 amountToBuffer = arraySize;
	 
	 dataArray = (unsigned int *)calloc(arraySize, sizeof(unsigned int));
	 while(dataArray == NULL);
	 smoothedData = 0; 
	 bufferedDataCount = 0; //first set of beat coutns that fill the buffer -- index for dataArray 
     bufferFull = false;   //boolean to say if we've filled the array 
}

void smooth::smoothData(long data)
{
  smoothedData = 0; // set back to 0 so addition can happen to it
  if(bufferedDataCount < amountToBuffer && bufferFull == false)
  {
    dataArray[bufferedDataCount] = data; // store data into the array
    bufferedDataCount++;   //increment the dataArray index counter
    for(int i = 0; i < bufferedDataCount; i++)
    {
      // Serial.print("Fill data = ");
      // Serial.println(dataArray[i],DEC);
       smoothedData += dataArray[i];
    }
    smoothedData = smoothedData/bufferedDataCount;
   // Serial.print("Fill Average = ");
   // Serial.println(smoothedData,DEC);
   }
   if(bufferFull == true)
   {
     for(int i = 0; i < amountToBuffer; i++)
     {
       if(i < (amountToBuffer-1))
       {
        dataArray[i] = dataArray[i+1];
       }
       else if(i == (amountToBuffer-1))
       {
          dataArray[i] = data;
       }
     }
     for(int i = 0; i < amountToBuffer; i++)
     {
        smoothedData += dataArray[i];
     }
     smoothedData = smoothedData/amountToBuffer;
   }

  if(bufferedDataCount == amountToBuffer){bufferFull = true;}
  return;
}

void smooth::clearData()
{
  smoothedData = 0; 
  bufferedDataCount = 0; //first set of beat coutns that fill the buffer -- index for dataArray 
  bufferFull = false;   //boolean to say if we've filled the array
}

