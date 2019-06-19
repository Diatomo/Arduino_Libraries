#include <alarmClock.h>
#include <digits.h>

#define segData 11
#define segClock 13
#define segLatch 8

#define groupAlength 2
#define groupAoffset 0 // A is the first group on the chain

#define groupBlength 2
#define groupBoffset (groupAlength) // B is the second group on the chain, so the offset is the total number of digits before it in the chain

#define totalDigits (groupAlength + groupBlength) // calculate the total number of digits 

digits dig = digits(segData, segClock, segLatch, totalDigits); // this class takes care of the hardware interface

digitGroup dgA = digitGroup(&dig, groupAoffset, groupAlength); // this class takes care of a single group of digits
digitGroup dgB = digitGroup(&dig, groupBoffset, groupBlength);

repeatAlarm rA = repeatAlarm(updateA);
repeatAlarm rB = repeatAlarm(updateB);

void setup()
{
	rA.setInterval(652); // update group A every 652ms
	rB.setInterval(163); // update group B every 163ms

	digitGroup testGroup = digitGroup(&dig, 0, totalDigits); // create a temporary group containing all of the digits in the chain
	testGroup.segDisp(test);
	delay(1000);
	testGroup.segDisp(foul);
	delay(1000);
	testGroup.segDisp(err);
	delay(1000);
	testGroup.segDisp(dash);
	delay(1000);
	testGroup.segDisp(blank);
	delay(1000);

	for (byte i = 0; i < 25; i++)
	{
		testGroup.chaseAnimation();
		delay(100);
	}
	testGroup.segDisp(blank);
	delay(1000);

	for (int j = 25; j > -25; j--)
	{
		testGroup.segDispSign(j); // signed numbers
		delay(200);
	}

	testGroup.segDisp(blank);
	delay(1000);

	for (int k = 25; k > -25; k--)
	{
		testGroup.segDispSign(k, 3); // signed numbers with a decimal point
		delay(200);
	}

	testGroup.segDisp(blank);
	delay(1000);

	dgA.segDisp(1); // identify group A with a '1'
	dgB.segDisp(2); // identify group B with a '2'
	delay(1000);
}

void loop()
{
	rA.poll();
	rB.poll();
}

void updateA()
{
	static byte i = 0;
	dgA.segDisp(i, 1);
	i++;
	if (i > 99) { i = 0; }
}

void updateB()
{
	static byte i = 99;
	dgB.segDisp(i, 2);
	i--;
	if (i == 0) { i = 99; }
}

