//#include <SPI.h> //this must be included before the led library only if you are using the direct SPI mode
#include <LED2801.h>
#include <color.h> //this is for Color() and Wheel()

#define numberOfPixelGroups 5 // this is the number of pixel groups, physical pixels = (pixel groups) * (group size)
#define pixelGroupSize 4

//Here is the slower interface mode that can use any two pins:
// -there can be multipe instances of the library connected to different pins in this mode
// -fairly slow to update for a large number of pixels

#define dataPin 11
#define clockPin 13
LED2801 pixels = LED2801(dataPin, clockPin, numberOfPixelGroups, pixelGroupSize);

//here is the direct (SPI) method:
// -must use pin 13 for clock and pin 11 for data (both without FETs) on UNO boards
// -must use dedicated SPI pins PB2(MOSI) for data and PB1(SCLK) for clock on Leonardo boards
// -there can only be on instance of the library in this mode
// -much faster (2MHz data rate)

//LED2801 pixels = LED2801(numberOfPixelGroups, pixelGroupSize);

void setup()
{
  pixels.setColorOrder(&_BRG); //call this if the order is different from RGB (square lights are GRB or BRG)
  pixels.autoUpdate = false;
}

void loop()
{
  word i, j;
 
  for(j=0; j < 256; j++)
  {
    for(i=0; i < numberOfPixelGroups; i++)
    {
      pixels.setColor(i, Wheel( (((i<<8) / numberOfPixelGroups) + j) % 256));
    }
    pixels.send(); //needed when autoUpdate is false
    delay(20);
  }
}
