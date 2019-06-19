/*
 * alarmClock.cpp
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
#include "alarmClock.h"

/** Constructor
 * @param ringer Function pointer to the event handler funcion. Must be a function witout parameters or a return. This can also be set to NULL to just use the timer.
 */
alarmClock::alarmClock(void (*ringer)(void))
{
	this->ringer = ringer;
	set = false;
}

/** Constructor
 * 
 */
alarmClock::alarmClock()
{
	ringer = NULL;
	set = false;
}

/** Checks if the alarm is active.
 * @return true if active
 */
boolean alarmClock::isSet()
{
	return set;
}

/** Sets the alarm time (resets) and activates the timer
 * @param Time set time in ms from now
 */
void alarmClock::setAlarm(unsigned long Time)
{
	setTime = Time + millis();
	set = true;
}

/** Deactivates the timer.
 *
 */
void alarmClock::unSetAlarm()
{
	set = false;
}

/** Polling function, this should be called as often as possible ( normally from loop() ).
 * The call to the event handler occurs inside this.
 * @return true if the alarm occured
 */
boolean alarmClock::poll()
{
	if ((set == true) && (setTime <= millis()))
	{
		set = false;
		if (ringer != NULL) { (*ringer)(); }
		return true;
	}
	return false;
}

/** Gets the time remaining time.
 * @return Time in milliseconds from now
 */
unsigned long alarmClock::getRemainingTime()
{
	unsigned long millisTemp = millis();
	if (!set)
	{
		return 0;
	}
	if (setTime > millisTemp)
	{
		return (setTime - millis());
	}
	return 0;
}

/** Constructor
 * @param ringer Function pointer to the event handler funcion. Must be a function witout parameters or a return.
 */
repeatAlarm::repeatAlarm(void (*ringer)(void))
{
	this->ringer = ringer;
}

/** Sets the interval time without changing the current time remaining.
 * @param interval The interval time in milliseconds
 */
void repeatAlarm::setInterval(unsigned long interval)
{
	this->interval = interval;
}

/** Sets the interval time and resets the time remaining to the new interval time.
 * @param interval The interval time in milliseconds
 */
void repeatAlarm::setIntervalReset(unsigned long interval)
{
	this->interval = interval;
	setTime = millis() + interval;
}

/** Reset the time remaining to the current interval time.
 *
 */
void repeatAlarm::reset()
{
	setTime = millis() + interval;
}

/** Polling function, this should be called as often as possible ( normally from loop() ).
 * The call to the event handler occurs inside this.
 * @return true if the alarm occured
 */
boolean repeatAlarm::poll()
{
	unsigned long millisTemp = millis();
	if (setTime <= millisTemp)
	{
		setTime = interval + millisTemp;
		(*ringer)();
		return true;
	}
	return false;
}

/** Gets the time remaining time.
 * @return Time in milliseconds from now
 */
unsigned long repeatAlarm::getRemainingTime()
{
	unsigned long millisTemp = millis();
	if (setTime > millisTemp)
	{
		return (setTime - millis());
	}
	return 0;
}

/** Gets the current interval time.
 * @return Time in milliseconds
 */
unsigned long repeatAlarm::getInterval()
{
	return interval;
}