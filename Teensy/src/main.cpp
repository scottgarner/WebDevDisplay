#include <Arduino.h>

#include <OctoWS2811.h>

#include "fonts/HUNTER_font.h"
#define CHAR_WIDTH 8
#define CHAR_HEIGHT 8

const int numPins = 1;
byte pinList[numPins] = {2};

const int ledsPerStrip = 512;
const int bytesPerLED = 3;

int rows = 8;
int cols = ledsPerStrip / 8;

int offset = 0;

DMAMEM int displayMemory[ledsPerStrip * numPins * bytesPerLED / 4];
int drawingMemory[ledsPerStrip * numPins * bytesPerLED / 4];

// int pixelIndex = 0;

const int config = WS2811_GRB | WS2811_800kHz;
OctoWS2811 leds(ledsPerStrip, displayMemory, drawingMemory, config, numPins, pinList);

char serialBuffer[256];
byte serialBufferIndex = 0;

char fontColumns[256 * 8];
byte fontColumnIndex = 0;

void processBuffer(char *buffer, int length)
{
  fontColumnIndex = 0;

  for (byte i = 0; i < length; i++)
  {
    // Grab letter from buffer;
    char letter = buffer[i];

    // Change letter into font data index.
    char index = letter & 0x7F;
    if (index < ' ')
    {
      index = 0;
    }
    else
    {
      index -= ' ';
    }

    // Lookup font data.
    const unsigned char *fontData = &font[index][0];

    // Sample font columns;
    for (byte x = 0; x < CHAR_WIDTH; x++)
    {
      fontColumns[fontColumnIndex] = fontData[x];
      fontColumnIndex++;
    }
  }

  Serial.println(fontColumnIndex);
  for (int i = 0; i < fontColumnIndex; i++)
  {
    Serial.println(fontColumns[i], HEX);
  }
}

void setup()
{
  Serial.begin(115200);
  Serial.println("READY");

  leds.begin();
  leds.show();

  processBuffer("ABCDEFGH", 8);

  for (int i = 0; i < ledsPerStrip; i++)
  {
    leds.setPixel(i, 0xffffff);
  }
  leds.show();
}

void loop()
{
  return;
  // Serial

  while (Serial.available() > 0)
  {
    char character = Serial.read();
    if (character != '\n')
    {
      serialBuffer[serialBufferIndex] = character;
      serialBufferIndex++;
    }
    else
    {
      byte length = serialBufferIndex - 1;
      processBuffer(serialBuffer, length);
      serialBufferIndex = 0;
    }
  }

  //

  for (byte x = 0; x < cols; x++)
  {
    int column = (x + offset) % fontColumnIndex;
    char fontColumn = fontColumns[column];

    for (byte y = 0; y < CHAR_HEIGHT; y++)
    {

      int pixelIndex = (x * rows) + ((x % 2 == 0) ? y : 7 - y);

      if (fontColumn & (1 << y))
      {
        leds.setPixel(pixelIndex, 0x040404);
        // Serial.print("1");
      }
      else
      {
        // Serial.print("0");
        leds.setPixel(pixelIndex, 0x000000);
      }
    }
    // Serial.println("");
  }

  offset++;
  leds.show();
  delay(100);
}
