#include <alarmClock.h>

repeatAlarm ra = repeatAlarm(myAlarm);

repeatAlarm displayTimer = repeatAlarm(displayService); // this is for displaying the countdown

void setup() 
{
  Serial.begin(9600);
  ra.setInterval(10000); // this only changes the interval time (faster call)
  displayTimer.setInterval(107); // the time was chosen for no particular reason
}

void loop() 
{
  static boolean state;
  if(Serial.available())
  {
    Serial.read(); // throw the data away, we are just using this as an event
    if(state)
    {
      ra.setInterval(60000); // this will reset the timer and change the interval time
      
      /* with the following line commented out, the timer will continue to count down from the cuurent time remaining */
      //ra.reset(); // this is another way to reset the internal timer if you don't need to change the interval time
    }
    else
    {
      ra.setIntervalReset(10000);
    }
    state = !state;
  }
  ra.poll();  
  displayTimer.poll();
}

void myAlarm()
{
  Serial.println("ring!!!");
}

void displayService()
{
  Serial.print(ra.getRemainingTime(), DEC);
  Serial.print("\t");
  Serial.println(ra.getInterval(), DEC);
}
