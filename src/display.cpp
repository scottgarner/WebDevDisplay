#include "globals.h"

////
#include "I2SClocklessLedDriver.h"

I2SClocklessLedDriver driver;

#define LEDS_PER_STRIP 512
#define STRIPS 6

#define ROWS 8
#define COLUMNS ((LEDS_PER_STRIP * STRIPS) / ROWS)

uint8_t leds[STRIPS * LEDS_PER_STRIP * 3];
int pins[STRIPS] = {13, 12, 14, 27, 26, 25};

////

#include "fonts/HUNTER_font.h"
#define CHAR_WIDTH 8
#define CHAR_HEIGHT 8

////

int offset = 0;
int lastOffset = 0;
int offsetTime = 100;

char serialBuffer[256];
int serialBufferIndex = 0;

char rawTextBuffer[256];
int rawTextBufferLength = 0;

char processedTextBuffer[256];
int processedTextBufferLength = 0;

char fontColumns[256 * 8];
int fontColumnIndex = 0;

////

void displayProcessRawText()
{
    // Process variables.
    String stringBuffer = String(rawTextBuffer, rawTextBufferLength);
    char time[9];
    timeNow(time);
    stringBuffer.replace("{{time}}", time);

    // Write to processed buffer.
    strcpy(processedTextBuffer, stringBuffer.c_str());
    processedTextBufferLength = stringBuffer.length();

    // Update pixel buffer.
    displayUpdatePixelBuffer();

    // Print processed text.
    // Serial.println(processedTextBuffer);
}

void displayUpdatePixelBuffer()
{
    if (processedTextBufferLength <= 0)
        return;

    fontColumnIndex = 0;

    for (int i = 0; i < processedTextBufferLength; i++)
    {
        // Grab letter from buffer;
        char letter = processedTextBuffer[i];

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
        for (int x = 0; x < CHAR_WIDTH; x++)
        {
            fontColumns[fontColumnIndex] = fontData[x];
            fontColumnIndex++;
        }
    }

    // Serial.println(fontColumnIndex);
    // for (int i = 0; i < fontColumnIndex; i++)
    // {
    //     Serial.println(fontColumns[i], HEX);
    // }
}

void displaySetRawText(const char *buffer, int length)
{
    // Copy to raw buffer;
    strcpy(rawTextBuffer, buffer);
    rawTextBufferLength = length;

    // Process new text.
    displayProcessRawText();
}

void displaySetBrightness(byte brightness)
{
    driver.setBrightness(brightness);
}

void displaySetup()
{
    driver.initled(leds, pins, STRIPS, LEDS_PER_STRIP, ORDER_GRB);
    displaySetBrightness(8);

    for (int i = 0; i < LEDS_PER_STRIP * STRIPS; i++)
    {
        driver.setPixel(i, 0, 0, 0);
    }

    driver.showPixels();
}

void displayLoop()
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
            displaySetRawText(serialBuffer, length);
            serialBufferIndex = 0;
        }
    }

    //

    for (int x = 0; x < COLUMNS; x++)
    {
        int column = (x + offset) % fontColumnIndex;
        char fontColumn = fontColumns[column];

        for (int y = 0; y < CHAR_HEIGHT; y++)
        {

            int pixelIndex = (x * ROWS) + ((x % 2 == 0) ? y : 7 - y);

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