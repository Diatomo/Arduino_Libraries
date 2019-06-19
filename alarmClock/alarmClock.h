/*
 * alarmClock.h
 * alarmClock provides a single event timer, used for timeouts or similar
 * repeatAlarm provides a repeating event timer, used for polling inputs or updating a display
 *
 * Rev 1 - Keegan Morrow
 * Rev 2 - Keegan Morrow -  Added repeatAlarm
 * Rev 3 - Keegan Morrow -  Bugfix for repeatAlarm to address a ~1ms slip per event
 *                          Bugfix for alarmClock::poll() changing set causing unpredictable return from alarmClock::isSet()
 * Rev 4 - Keegan Morrow -  Added unSetAlarm to alarmClock class
 * Rev 5 - Keegan Morrow -  Added setIntervalReset() and reset() to repeatAlarm, to restart the timer
 *                          Added getRemainingTime() to alarmClock and repeatAlarm
 * Rev 6 - Keegan Morrow -  Bugfix in alarmClock::poll() preventing the alarm from being set from the ringer function
 * Rev 7 - Keegan Morrow -	Added the ability to alarmClock to not call a function
 *
 */

#ifndef __alarmClock_h_
#define __alarmClock_h_

#if (ARDUINO >= 100)
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

#define ALARMCLOCKREV 7

/**
 * Utility class for managing one-shot time events.
 * @author Keegan Morrow
 * @version 7
 */
class alarmClock
{
public:
	boolean isSet();
	void setAlarm(unsigned long);
	void unSetAlarm();
	boolean poll();
	unsigned long getRemainingTime();
	alarmClock();
	alarmClock(void (*ringer)(void));
private:
	unsigned long setTime;
	void (*ringer)(void);
	boolean set;

};

/**
 * Utility class for managing repeating time events.
 * @author Keegan Morrow
 * @version 7
 */
class repeatAlarm
{
public:
	repeatAlarm(void (*ringer)(void));
	void setInterval(unsigned long);
	void setIntervalReset(unsigned long);
	void reset();
	boolean poll();
	unsigned long getRemainingTime();
	unsigned long getInterval();
private:
	unsigned long setTime;
	unsigned long interval;
	void (*ringer)(void);
};

#endif //__alarmClock_h_
