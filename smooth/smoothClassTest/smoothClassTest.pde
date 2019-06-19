#include <smooth.h>
#include <sevenSegment.h>
#include <joeMisc.h>


#define heartBeat 8     //input pin of heart rate monitor

#define btwnTime 350    //time we wait to cancel out false triggers
#define minute   60000  //one minte in milliseconds

/***************************************
 Since this a microcontroller it's best
 to statically locate memory for the 
 array(default 15). To increase the MAX size of the
 array just adjust the number of MAX
 in the smooth.h file and redownload 
 the code to the controller. The number
 used in smooth(10) as seen below is to 
 just set the number of numbers you want 
 smooth, so it can be any number up to
 and including what  MAX is set to
 ***************************************/
smooth dataSet = smooth(10); 
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
  if(millis() - beatTime > 5000) {dataSet.clearData();}
  
  if(digitalRead(heartBeat) == HIGH && prevBeat != 1)
  {
    interval = millis() - beatTime;
     if(interval >= btwnTime)
    { 
      beatTime = millis();
      prevBeat = 1;
      dataSet.smoothData(minute/interval);
      myDisplay.segDisp(dataSet.smoothedData,0);
   
    }
  }
  
  if(digitalRead(heartBeat) == LOW )
  {   
     prevBeat = 0;
  }
}

  
  
