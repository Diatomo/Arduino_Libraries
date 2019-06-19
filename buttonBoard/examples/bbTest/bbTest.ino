/*
 * Roto
 * Example Code
 *
 * Library:
 * buttonBoard, alarmClock
 *
 * Author(s):
 * Keegan Morrow
 *
 */

// Libraries:
#include <alarmClock.h>
#include <buttonBoard.h>

// Pin Assignments:
const byte fet4 = 4;
const byte fet5 = 5;
const byte fet6 = 6;
const byte fet7 = 7;

const byte bbDI = 8;
const byte bbDO = 9;
const byte bbCLK = 10;
const byte bbILT = 11;
const byte bbOLT = 12;

// Constants:
const byte bbCount = 1;

// Global Variables:

// Global Classes:
buttonBoard bb = buttonBoard(bbDI, bbDO, bbCLK, bbILT, bbOLT, bbCount);

buttonToggle toggle0 = buttonToggle(&bb, 0); // this is a helper class for buttons that toggle states
buttonToggle toggle1 = buttonToggle(&bb, 1); // Called as buttonToggle( [pointer to buttonBoard], [button number] );
buttonToggle toggle2 = buttonToggle(&bb, 2);

buttonSelect select = buttonSelect(&bb, 4, 4, true); // this is a helper class for a one of many group of buttons
// Called as buttonSelect( [pointer to buttonBoard], [starting button number], [number of buttons], [reset state] );

repeatAlarm serialTimer = repeatAlarm(serialStatus); // calls the function serialStatus() on a set interval
repeatAlarm pollTimer = repeatAlarm(pollButtons); // calls the function pollButtons() on a set interval

alarmClock selectTimeout = alarmClock(resetButtons); // calls the function resetButtons() when its timer expires


void setup()
{
	pinMode(fet4, OUTPUT);
	pinMode(fet5, OUTPUT);
	pinMode(fet6, OUTPUT);
	pinMode(fet7, OUTPUT);
	digitalWrite(fet4, LOW);
	digitalWrite(fet5, LOW);
	digitalWrite(fet6, LOW);
	digitalWrite(fet7, LOW);

  bb.setLamp(HIGH);
  delay(500);
  bb.setLamp(LOW);
  delay(500);
  pollTimer.setInterval(10); // sets the time interval for pollButtons()
  serialTimer.setInterval(250); // sets the time interval for serialStatus()
  Serial.begin(9600);
}

void loop()
{
	pollTimer.poll();
	selectTimeout.poll();
	serialTimer.poll();

	if (bb.getButton(3)) resetButtons(); // watch button 3 and reset the other button states if pressed
	if ((select.getState() == buttonReset) && (toggle0.getState() == false) && (toggle1.getState() == false) && (toggle2.getState() == false)) bb.setLamp(3, false); // turn the reset button lamp off
	else bb.setLamp(3, true); // turn the reset button lamp on if it will do something
}

void pollButtons()
{
	if (toggle0.poll()) selectTimeout.setAlarm(5000); // reset the timeout timer if the toggle button has been pressed (state change)
	if (toggle1.poll()) selectTimeout.setAlarm(5000);
	if (toggle2.poll()) selectTimeout.setAlarm(5000);
	if (select.poll()) selectTimeout.setAlarm(5000); // reset the timeout timer if the group of buttons has changed state
}

void resetButtons()
{
	select.setState(buttonReset);
	toggle0.setState(false);
	toggle1.setState(false);
	toggle2.setState(false);
}


void serialStatus()
{
	Serial.print("0:");
	if (toggle0.getState() == true) Serial.print("ON\t");
	else Serial.print("OFF\t");
	Serial.print("1:");
	if (toggle1.getState() == true) Serial.print("ON\t");
	else Serial.print("OFF\t");
	Serial.print("2:");
	if (toggle2.getState() == true) Serial.print("ON\t");
	else Serial.print("OFF\t");

	Serial.print("S:");
	Serial.println(select.getState(), DEC);

}
