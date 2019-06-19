#include <alarmClock.h>

alarmClock ac = alarmClock(myAlarm);

boolean newAlarm = false;

void setup() 
{
  Serial.begin(9600);
}

void loop() 
{
  if(newAlarm)
  {
    newAlarm = false;
    ac.setAlarm(1000);
  }
  
  ac.poll();  
}

void myAlarm()
{
  Serial.println("ring!!!");
  newAlarm = true;
}
