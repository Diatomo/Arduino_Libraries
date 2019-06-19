#include <digits.h>

#define segData 8
#define segClock 9
#define segLatch 10
#define numDigits 4

digits dig = digits(segData, segClock, segLatch, numDigits);

void setup()
{
}

void loop()
{
	static word i;
	byte j;

	for (j = numDigits; j != 0; j--)
	{
		dig.setDigit(j - 1, (i + j) % 10, true);
		delay(125);
		dig.setDigit(j - 1, (i + j) % 10, false);
	}
	i++;
	if (i > 9) { i = 0; }
}
