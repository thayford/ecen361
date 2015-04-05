#include <SPI.h>
//#include <SD.h>
#include <SdFat.h>
#include "LPD8806.h"
SdFat SD;
File file;

// TFT display and SD card will share the hardware SPI interface.
// Hardware SPI pins are specific to the Arduino board type and
// cannot be remapped to alternate pins.  For Arduino Uno,
// Duemilanove, etc, pin 11=MOSI, pin 12=MISO, pin 13=SCK.
#define SD_CS      10   // Chip select for SD card
LPD8806 strip = LPD8806(256, A5, A4);
int pot1 = A0;
int pot2 = A1;
int pot3 = A2;

byte ButtonA = 4;
byte ButtonB = 5;
byte ButtonUp = 6;
byte ButtonDown = 7;
byte ButtonLeft = 8;
byte ButtonRight = 9;
unsigned int count = 0;
bool selector = false;
bool sOn = false;
bool autoCycle = false;
int d1, d2, inDelay;
/*******************************************/
void setup(void) 
{
  //pinMode(A4, OUTPUT);
  //pinMode(A5, OUTPUT);
  strip.begin();
  Serial.begin(57600);
  delay(1000); 
  pinMode(ButtonLeft, INPUT);
  
  
  SD.begin(SD_CS, SPI_FULL_SPEED);
  
  //bmpDraw("0.bmp", 0, 0);
  

}

/****************************************************/
void loop() 
{
  
  if (digitalRead(ButtonLeft) || digitalRead(ButtonRight) || digitalRead(ButtonB) || (autoCycle && (d2 > inDelay)))
  {
   if (file.openNext(SD.vwd(), O_READ)) 
   {
    if (!file.isDir()) {
      bmpDraw(file, 0, 0);
      delay(200); //Crappy debounce
    }
    file.close();
   } else SD.begin(SD_CS, SPI_FULL_SPEED);
  }
  if (digitalRead(ButtonA))
  {
    autoCycle = !autoCycle;
    delay(200); //Crappy debounce
  }
    
 
  
  if (d2 > inDelay)
    d2 = 0;
  //d2++;
  
  if (d1 > 10)
  {
    d2++;
    d1 = 0;
  }
  d1++;
  
  inDelay = analogRead(pot1) * 3;
  
  /*if (digitalRead(ButtonLeft)) 
  {
    if(y>0)
    {
      short xy = LEDMap(x,y);
      strip.setPixelColor(xy, strip.Color(GridColorsR[xy],GridColorsB[xy],GridColorsG[xy]));
      strip.show();
      y--;
    }
    delay(200);
  }*/
}

// This function opens a Windows Bitmap (BMP) file and
// displays it at the given coordinates.  It's sped up
// by reading many pixels worth of data at a time
// (rather than pixel by pixel).  Increasing the buffer
// size takes more of the Arduino's precious RAM but
// makes loading a little faster.  20 pixels seems a
// good balance.

#define BUFFPIXEL 1

/**********************************************************/
//void bmpDraw(char *filename, uint8_t x, uint8_t y) 
void bmpDraw(File bmpFile, uint8_t x, uint8_t y)
{
  //File     bmpFile;
  int      bmpWidth, bmpHeight;   // W+H in pixels
  uint8_t  bmpDepth;              // Bit depth (currently must be 24)
  uint32_t bmpImageoffset;        // Start of image data in file
  uint32_t rowSize;               // Not always = bmpWidth; may have padding
  uint8_t  sdbuffer[3*BUFFPIXEL]; // pixel buffer (R+G+B per pixel)
  uint8_t  buffidx = sizeof(sdbuffer); // Current position in sdbuffer
  boolean  goodBmp = false;       // Set to true on valid header parse
  boolean  flip    = true;        // BMP is stored bottom-to-top
  int      w, h, row, col;
  uint8_t  r, g, b;
  uint32_t pos = 0, startTime = millis();


//  Serial.println();
//  Serial.print("Loading image '");
//  Serial.print(filename);
//  Serial.println('\'');

  // Open requested file on SD card
//  if ((bmpFile = SD.open(filename)) == NULL) {
//    Serial.print("File not found");
//    return;
//  }

  // Parse BMP header
  if(read16(bmpFile) == 0x4D42) { // BMP signature
    Serial.print("File size: "); 
    Serial.println(read32(bmpFile));
    (void)read32(bmpFile); // Read & ignore creator bytes
    bmpImageoffset = read32(bmpFile); // Start of image data
    Serial.print("Image Offset: "); 
    Serial.println(bmpImageoffset, DEC);
    // Read DIB header
    Serial.print("Header size: "); 
    Serial.println(read32(bmpFile));
    bmpWidth  = read32(bmpFile);
    bmpHeight = read32(bmpFile);
    if(read16(bmpFile) == 1) { // # planes -- must be '1'
      bmpDepth = read16(bmpFile); // bits per pixel
      Serial.print("Bit Depth: "); Serial.println(bmpDepth);
      if((bmpDepth == 24) && (read32(bmpFile) == 0)) { // 0 = uncompressed

        goodBmp = true; // Supported BMP format -- proceed!
//        Serial.print("Image size: ");
//        Serial.print(bmpWidth);
//        Serial.print('x');
//        Serial.println(bmpHeight);

        // BMP rows are padded (if needed) to 4-byte boundary
        rowSize = (bmpWidth * 3 + 3) & ~3;

        // If bmpHeight is negative, image is in top-down order.
        // This is not canon but has been observed in the wild.
        if(bmpHeight < 0) {
          bmpHeight = -bmpHeight;
          flip      = false;
        }

        // Crop area to be loaded
        w = bmpWidth;
        h = bmpHeight;
        //if((x+w-1) >= 16)  w = 16  - x;
        //if((y+h-1) >= 16) h = 16 - y;

        // Set TFT address window to clipped image bounds
        //tft.setAddrWindow(x, y, x+w-1, y+h-1);
        //clearScreen();
        for (row=0; row<h; row++) { // For each scanline...

          // Seek to start of scan line.  It might seem labor-
          // intensive to be doing this on every line, but this
          // method covers a lot of gritty details like cropping
          // and scanline padding.  Also, the seek only takes
          // place if the file position actually needs to change
          // (avoids a lot of cluster math in SD library).
          if(flip) // Bitmap is stored bottom-to-top order (normal BMP)
            pos = bmpImageoffset + (bmpHeight - 1 - row) * rowSize;
          else     // Bitmap is stored top-to-bottom
            pos = bmpImageoffset + row * rowSize;
          if(bmpFile.position() != pos) { // Need seek?
            bmpFile.seek(pos);
            buffidx = sizeof(sdbuffer); // Force buffer reload
          }

          for (col=0; col<w; col++) { // For each pixel...
            // Time to read more pixel data?
            if (buffidx >= sizeof(sdbuffer)) { // Indeed
              bmpFile.read(sdbuffer, sizeof(sdbuffer));
              buffidx = 0; // Set index to beginning
            }

            // Convert pixel from BMP to TFT format, push to display
            b = sdbuffer[buffidx++];
            g = sdbuffer[buffidx++];
            r = sdbuffer[buffidx++];
            //tft.pushColor(tft.Color565(r,g,b));
            short xy = LEDMap(row,col);
            //strip.setPixelColor(xy, strip.Color(r,b,g));
//            Serial.print("row:");
//            Serial.print(row);
//            Serial.print("col:");
//            Serial.print(col);
//            Serial.print(" red: ");
//            Serial.print(r/2);
//            Serial.print(" green: ");
//            Serial.print(g/3);
//            Serial.print(" blue: ");
//            Serial.println(b/10);
            strip.setPixelColor(xy, strip.Color(r/2,b/10,g/3));
            //strip.setPixelColor(xy, strip.Color(0,0,0));
            
            /*if (row >= bmpHeight - offsetY)
            {
              // black pixel
              strip.setPixelColor(xy, strip.Color(0, 0, 0));
            }
            // offsetY is negative
            else if (row < offsetY * -1)
            {
              // black pixel
              strip.setPixelColor(xy, strip.Color(0, 0, 0));
            }
            // offserX is beyond bmpWidth
            else if (col >= bmpWidth + offsetX)
            {
              // black pixel
              strip.setPixelColor(xy, strip.Color(0, 0, 0));
            }
            // offsetX is positive
            else if (col < offsetX)
            {
              // black pixel
              strip.setPixelColor(xy, strip.Color(0, 0, 0));
            }
            // all good
            else //strip.setPixelColor(getIndex(col+x, row), strip.Color(r, g, b));*/
            //strip.setPixelColor(xy, strip.Color(r, g, b));
            // paint pixel color
          
          } // end pixel
        } // end scanline
        //strip.setPixelColor(0, strip.Color(0,0,0));
        //strip.show();
//        Serial.print("Loaded in ");
//        Serial.print(millis() - startTime);
//        Serial.println(" ms");
      } // end goodBmp
    }
  }

  bmpFile.close();
  //delay(1000);
  //clearScreen();
  strip.show();
  //if(!goodBmp) Serial.println("BMP format not recognized.");
}

// These read 16- and 32-bit types from the SD card file.
// BMP data is stored little-endian, Arduino is little-endian too.
// May need to reverse subscript order if porting elsewhere.

/********************************************************/
uint16_t read16(File &f) 
{
  uint16_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read(); // MSB
  return result;
}

/********************************************************/
uint32_t read32(File &f) 
{
  uint32_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read();
  ((uint8_t *)&result)[2] = f.read();
  ((uint8_t *)&result)[3] = f.read(); // MSB
  return result;
}

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
/*void clearScreen()
{
  for (int i=0; i<256; i++)
  {
    strip.setPixelColor(i, strip.Color(0, 0, 0));
  } 
}*/
