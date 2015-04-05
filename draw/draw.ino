#include "LPD8806.h"
#include "SPI.h"

// Simple test for 160 (5 meters) of LPD8806-based RGB LED strip
// Not compatible with Trinket/Gemma due to limited RAM

/*****************************************************************************/
/*short LEDMap[16][16] = {{0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15 },
                        {31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16 }, 
                        {32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47 },
                        {63, 62, 61, 60, 59, 58, 57, 56, 55, 54, 53, 52, 51, 50, 49, 48 },
                        {64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79 },
                        {95, 94, 93, 92, 91, 90, 89, 88, 87, 86, 85, 84, 83, 82, 81, 80 },
                        {96, 97, 98, 99, 100,101,102,103,104,105,106,107,108,109,110,111},
                        {127,126,125,124,123,122,121,120,119,118,117,116,115,114,113,112},
                        {128,129,130,131,132,133,134,135,136,137,138,139,140,141,142,143},
                        {159,158,157,156,155,154,153,152,151,150,149,148,147,146,145,144},
                        {160,161,162,163,164,165,166,167,168,169,170,171,172,173,174,175},
                        {191,190,189,188,187,186,185,184,183,182,181,180,179,178,177,176},
                        {192,193,194,195,196,197,198,199,200,201,202,203,204,205,206,207},
                        {223,222,221,220,219,218,217,216,215,214,213,212,211,210,209,208},
                        {224,225,226,227,228,229,230,231,232,233,234,235,236,237,238,239},
                        {255,254,253,252,251,250,249,248,247,246,245,244,243,242,241,240}};*/
//uint32_t GridColors[50];
uint8_t GridColorsR[256];
uint8_t GridColorsG[256];
uint8_t GridColorsB[256];
//int GridColors[256];

int pot1 = A0;
int pot2 = A1;
int pot3 = A2;

int ButtonA = 4;
int ButtonB = 5;
int ButtonUp = 6;
int ButtonDown = 7;
int ButtonLeft = 8;
int ButtonRight = 9;
unsigned int count = 0;
bool selector = false;
bool sOn = false;

unsigned char redIn = 0, greenIn = 0, blueIn = 0;
int x = 0;
int y = 0;

// Data Pin is 11, Clock Pin is 13
//LPD8806 strip = LPD8806(256);
LPD8806 strip = LPD8806(256, A5, A4);
LPD8806 strip2 = LPD8806(2, 2, 3); //# LED, dataPin, ClockPin

byte LEDMap(byte x, byte y)
{
  byte index;
  if (y == 0)
  {
    index = 15 - x;
  }
  else if (y % 2 != 0)
  {
    index = y * 16 + x;
  }
  else
  {
    index = (y * 16 + 15) - x;
  }
  return index;
}

void setup() {
  //pinMode(13, OUTPUT); //For testing
  pinMode(ButtonA, INPUT);
  pinMode(ButtonB, INPUT);
  pinMode(ButtonUp, INPUT);
  pinMode(ButtonDown, INPUT);
  pinMode(ButtonLeft, INPUT);
  pinMode(ButtonRight, INPUT);
  strip.begin();
  strip2.begin();
  Serial.begin (9600);
  
  // Update the strip, to start they are all 'off'
  strip.show();
  strip2.show();
  
  for (int a = 0; a < 256; a++)
    GridColorsR[a] = 0;
  for (int a = 0; a < 256; a++)
    GridColorsG[a] = 0;
  for (int a = 0; a < 256; a++)
    GridColorsB[a] = 0;
}


void loop() {
  if (digitalRead(ButtonB) && (digitalRead(ButtonA)))
  {
    for(int l = 0; l < 16; l ++)
      for(int m = 0; m < 16; m ++)
      {
        short xy = LEDMap(l,m);
        strip.setPixelColor(xy, strip.Color(redIn,blueIn,greenIn));
        GridColorsR[xy] = redIn;
        GridColorsB[xy] = blueIn;
        GridColorsG[xy] = greenIn;
      }
      strip.show();
      delay(200);
  }
  if (digitalRead(ButtonA))
  {
    selector = !selector;
    short xy = LEDMap(x,y);
    strip.setPixelColor(xy, strip.Color(GridColorsR[xy],GridColorsB[xy],GridColorsG[xy]));
    strip.show();
    delay(200);
  }
  if (digitalRead(ButtonB))
  {
    short xy = LEDMap(x,y);
    strip.setPixelColor(xy, strip.Color(redIn,blueIn,greenIn));
    GridColorsR[xy] = redIn;
    GridColorsB[xy] = blueIn;
    GridColorsG[xy] = greenIn;
    strip.show();
  }
  if (digitalRead(ButtonUp)) 
  {
    if(x>0)
    {
      short xy = LEDMap(x,y);
      strip.setPixelColor(xy, strip.Color(GridColorsR[xy],GridColorsB[xy],GridColorsG[xy]));
      strip.show();
      x--;
    }
    delay(200);
  }
  if (digitalRead(ButtonDown)) 
  {
    if(x<15)
    {
      short xy = LEDMap(x,y);
      strip.setPixelColor(xy, strip.Color(GridColorsR[xy],GridColorsB[xy],GridColorsG[xy]));
      strip.show();
      x++;
    }
    delay(200);
  }
  if (digitalRead(ButtonLeft)) 
  {
    if(y>0)
    {
      short xy = LEDMap(x,y);
      strip.setPixelColor(xy, strip.Color(GridColorsR[xy],GridColorsB[xy],GridColorsG[xy]));
      strip.show();
      y--;
    }
    delay(200);
  }
  if (digitalRead(ButtonRight)) 
  {
    if(y<15)
    {
      short xy = LEDMap(x,y);
      strip.setPixelColor(xy, strip.Color(GridColorsR[xy],GridColorsB[xy],GridColorsG[xy]));
      strip.show();
      y++;
    }
    delay(200);
  }
  
  redIn = (analogRead(pot1) / 8); //Takes 0-1024 value down to 0 - 128
  greenIn = (analogRead(pot2) / 8);
  blueIn = (analogRead(pot3) / 8);
 
  redIn = 127 - redIn;
  greenIn = 127 - greenIn;
  blueIn = 127 - blueIn;
  
  strip2.setPixelColor(0, strip.Color(redIn,blueIn,greenIn));
  strip2.show();
  
  count++;
  if (count > 300)
  {
   count = 0;
   sOn = !sOn;
   if (selector)
   {
     if (sOn)
     {
       strip.setPixelColor(LEDMap(x,y), strip.Color(127,30,60));
       strip.show();
     }
     else 
     {
       short xy = LEDMap(x,y);
       strip.setPixelColor(xy, strip.Color(GridColorsR[xy],GridColorsB[xy],GridColorsG[xy]));
       //strip.setPixelColor(LEDMap(x,y), strip.Color(0,0,0));
       strip.show();
     }
   }
  }
  
}




