#include "globals.h"

#define REFRESH_INTERVAL 1000

////

uint8_t messageIndex = 0;
uint lastMessageCycleTime = 0;

////

// #define NUM_LEDS_PER_STRIP 256
// #define STRIPS 12
#define NUM_LEDS_PER_STRIP 768
#define STRIPS 4

#include "I2SClocklessLedDriver.h"

I2SClocklessLedDriver driver;

#define ROWS 8
#define COLUMNS ((NUM_LEDS_PER_STRIP * STRIPS) / ROWS)

uint8_t leds[STRIPS * NUM_LEDS_PER_STRIP * 3];
int pins[STRIPS] = {32, 33, 25, 26};

// int pins[STRIPS] = {
//     32, 33, 25, 26, 27, 14,
//     19, 18, 05, 17, 16, 04};

////

#include "fonts/HUNTER_font.h"
#define CHAR_WIDTH 8
#define CHAR_HEIGHT 8

////

int offset = 0;
uint lastOffsetTime = 0;

char fontColumns[256 * 8];
int fontColumnIndex = 0;

uint lastRefreshTime = 0;

////

void displayRefresh()
{

    char *rawTextBuffer = messages[messageIndex].text_buffer;
    uint8_t rawTextBufferLength = messages[messageIndex].text_buffer_length;

    // Process variables.
    String stringBuffer = String(rawTextBuffer, rawTextBufferLength);
    char time[9];
    timeNow(time);
    stringBuffer.replace("{{time}}", time);

    Serial.print("Displaying message: ");
    Serial.println(stringBuffer);

    Serial.println("Updating pixel buffer...");
    // Update pixel buffer.
    {
        if (stringBuffer.length() <= 0)
            return;

        fontColumnIndex = 0;

        for (int i = 0; i < stringBuffer.length(); i++)
        {
            // Grab letter from buffer;
            char letter = stringBuffer[i];

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
    }
}

extern void displaySetMessage(int index)
{
    messageIndex = index;
    displayRefresh();
}

void displaySetup()
{
    driver.initled(leds, pins, STRIPS, NUM_LEDS_PER_STRIP, ORDER_GRB);

    for (int i = 0; i < NUM_LEDS_PER_STRIP * STRIPS; i++)
    {
        float a = (i % 2 == 0) ? 128 : 0;
        driver.setPixel(i, a, a, a);
    }

    driver.setBrightness(configuration.brightness);
    driver.showPixels();

    displayRefresh();
}

void displayLoop()
{
    bool redraw = false;

    // Handle scrolling.
    if (messages[messageIndex].speed > 0)
    {
        if (millis() - lastOffsetTime > (1000.0 / messages[messageIndex].speed))
        {
            offset++;
            lastOffsetTime = millis();
            redraw = true;
        }
    }

    // Handle swapping.
    if (messageCount > 0 && millis() - lastMessageCycleTime > (messages[messageIndex].duration * 1000.0))
    {
        messageIndex = (messageIndex + 1) % messageCount;
        // offset = 0;
        displayRefresh();
        lastMessageCycleTime = millis();
        redraw = true;
    }

    // Handle refresh interval.
    if (millis() - lastRefreshTime > REFRESH_INTERVAL)
    {
        displayRefresh();
        lastRefreshTime = millis();
        redraw = true;
    }

    // Set pixels.
    if (redraw)
    {

        for (int x = 0; x < COLUMNS; x++)
        {
            int column = (x + offset) % fontColumnIndex;
            char fontColumn = fontColumns[column];

            for (int y = 0; y < CHAR_HEIGHT; y++)
            {
                int pixelIndex = (x * ROWS) + ((x % 2 == 0) ? y : 7 - y);

                if (fontColumn & (1 << y))
                {
                    uint red = messages[messageIndex].red;
                    uint green = messages[messageIndex].green;
                    uint blue = messages[messageIndex].blue;

                    driver.setPixel(pixelIndex, red, green, blue);
                }
                else
                {
                    driver.setPixel(pixelIndex, 0, 0, 0);
                }
            }
        }

        // Show pixels.
        driver.showPixels();
    }
}