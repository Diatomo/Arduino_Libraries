#include <sevenSegment.h>
#include <joeMisc.h>

#define heartBeat 8     //input pin of heart rate monitor

#define btwnTime 350    //time we wait to cancel out false triggers
#define minute   60000  //one minte in milliseconds

/**** constant delcartions for the smoothing algorithm ************************/ 

#define  firstShowNumber 3 // first number that gets thrown up to the display(not averaged
#define  amountToBuffer  10 //number of beats that are avereaged

/*******************************************************************************/


/**** variable delcartions for the smoothing algorithm *************************/ 

int  smoothedData = 0; 
int  prevSmoothedData = 0;
int  dataArray[amountToBuffer] = {0}; // array to store the beat values
byte bufferedDataCount = 0; //first set of beat coutns that fill the buffer -- index for dataArray 
boolean bufferFull = false;   //boolean to say if we've filled the array

/*******************************************************************************/



sevenSegment myDisplay = sevenSegment(17,18,19,3);  // setup display

unsigned long beatTime  = millis();  //variable to store last recorded beat
unsigned long interval = millis();   // time between beats --used in calculating BPM
boolean prevBeat = 0;  


void setup()
{
 Serial.begin(9600);
 pinMode(heartBeat,INPUT);
 digitalWrite(heartBeat,LOW);
 myDisplay.segDisp(123,0);
 delay(1000);
 myDisplay.blank();
}

void loop()
{
  if(digitalRead(heartBeat) == HIGH && prevBeat != 1)
  {
    interval = millis() - beatTime;
    if(interval > 5000) { clearData();}
    if(interval >= btwnTime)
    { 
      beatTime = millis();
      prevBeat = 1;
      smooth(minute/interval);
      myDisplay.segDisp(smoothedData,0);
      if(bufferFull == true) 
      {
        myDisplay.segDisp(smoothedData,0);
      }
    }
  }
  if(digitalRead(heartBeat) == LOW )
  {   
     prevBeat = 0;
     //clearData();
  }
}

void smooth(int data)
{
  smoothedData = 0; // set back to 0 so addition can happen to it
  if(bufferedDataCount < amountToBuffer && bufferFull == false)
  {
    dataArray[bufferedDataCount] = data; // store data into the array
    bufferedDataCount++;   //increment the dataArray index counter
    for(int i = 0; i < bufferedDataCount; i++)
    {
       Serial.print("Fill data = ");
       Serial.println(dataArray[i],DEC);
       smoothedData += dataArray[i];
    }
    smoothedData = smoothedData/bufferedDataCount;
    Serial.print("Fill Average = ");
    Serial.println(smoothedData,DEC);
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
        Serial.print("Data = ");
        Serial.println(dataArray[i],DEC);
     }
     smoothedData = smoothedData/amountToBuffer;
     Serial.print("Average = ");
     Serial.println(smoothedData,DEC);
    }
  if(bufferedDataCount == amountToBuffer){bufferFull = true;}
  
  return;
}

void clearData()
{
  myDisplay.blank();
  smoothedData = 0; 
  prevSmoothedData = 0;
  bufferedDataCount = 0; //first set of beat coutns that fill the buffer -- index for dataArray 
  bufferFull = false;   //boolean to say if we've filled the array
}
  
  
