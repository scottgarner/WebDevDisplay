#include "globals.h"

////
#include "I2SClocklessLedDriver.h"

I2SClocklessLedDriver driver;

#define LEDS_PER_STRIP 512
#define STRIPS 6

uint8_t leds[STRIPS * LEDS_PER_STRIP * 3];
int pins[STRIPS] = {4, 16, 17, 5, 18, 19};

////

#include "fonts/HUNTER_font.h"
#define CHAR_WIDTH 8
#define CHAR_HEIGHT 8

////

int rows = 8;
int columns = LEDS_PER_STRIP / 8;

int offset = 0;
int lastOffset = 0;
int offsetTime = 100;

char serialBuffer[256];
byte serialBufferIndex = 0;

char fontColumns[256 * 8];
byte fontColumnIndex = 0;

////

void displayText(char *buffer, int length)
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

void setupDisplay()
{
    driver.initled(leds, pins, STRIPS, LEDS_PER_STRIP, ORDER_GRB);
    driver.setBrightness(10);
}

void loopDisplay()
{
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
            displayText(serialBuffer, length);
            serialBufferIndex = 0;
        }
    }

    //

    for (byte x = 0; x < columns; x++)
    {
        int column = (x + offset) % fontColumnIndex;
        char fontColumn = fontColumns[column];

        for (byte y = 0; y < CHAR_HEIGHT; y++)
        {

            int pixelIndex = (x * rows) + ((x % 2 == 0) ? y : 7 - y);

            if (fontColumn & (1 << y))
            {
                driver.setPixel(pixelIndex, 255, 255, 255);
                // Serial.print("1");
            }
            else
            {
                // Serial.print("0");
                driver.setPixel(pixelIndex, 0, 0, 0);
            }
        }
        // Serial.println("");
    }

    if (millis() - lastOffset > offsetTime)
    {
        offset++;
        lastOffset = millis();
    }

    driver.showPixels();
}