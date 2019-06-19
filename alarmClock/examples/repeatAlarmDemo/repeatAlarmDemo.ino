#include <alarmClock.h>

repeatAlarm ra = repeatAlarm(myAlarm);

void setup() 
{
  Serial.begin(9600);
  ra.setInterval(1000);
}

void loop() 
{
  ra.poll();  
}

void myAlarm()
{
  Serial.println("ring!!!");
}
