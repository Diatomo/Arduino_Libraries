#include <SPI.h> //this must be included before the led library only if you are using the direct SPI mode
#include <LED2801.h>
#include <color.h> //this is for Color() and Wheel()

#define numberOfPixels 25

/*
//Here is the slower interface mode that can use any two pins:
// -there can be multipe instances of the library connected to different pins in this mode
// -fairly slow to update for a large number of pixels
#define dataPin 11
#define clockPin 13

LED2801 pixels = LED2801(dataPin, clockPin, numberOfPixels);
*/

//here is the direct (SPI) method:
// -must use pin 13 for clock and pin 11 for data (both without FETs)
// -there can only be on instance of the library in this mode
// -much faster (2MHz data rate)

LED2801 pixels = LED2801(numberOfPixels);

void setup()
{
  pixels.setColorOrder(&_GRB); //call this if the order is different from RGB (square lights are GRB)
  pixels.setAll(255); //full bright white
  delay(500);
  pixels.setAll(0);
  delay(500);
  
  //pixels.autoUpdate = false; //require manually calling .send() (faster for animations)
}

void loop()
{
  word i, j;
  pixels.setAllColor(0xFFFFFF); //white
  delay(500);
  pixels.setAllColor(0xFF0000); //red
  delay(500);
  pixels.setAllColor(0x00FF00); //green
  delay(500);
  pixels.setAllColor(0x0000FF); //blue
  delay(500);
  pixels.setAllColor(0x00FFFF); //cyan
  delay(500);
  pixels.setAllColor(0xFF00FF); //magenta
  delay(500);
  pixels.setAllColor(0xFFFF00); //yellow
  delay(500);
  
  pixels.setAll(0);
  delay(500);
  
  for(i=0; i<256; i++)
  {
    pixels.setAllColor(Wheel(i));
    delay(20);
  }
  
  pixels.setAll(0);
  delay(500);
  
  pixels.autoUpdate = false;
  for(j=0; j < 256; j++)
  {
    for(i=0; i < numberOfPixels; i++)
    {
      pixels.setColor(i, Wheel( (((i<<8) / numberOfPixels) + j) % 256));
    }
    pixels.send(); //needed when autoUpdate is false
    delay(20);
  }
  pixels.autoUpdate = true;
  
  pixels.setAll(0);
  delay(500);
}
