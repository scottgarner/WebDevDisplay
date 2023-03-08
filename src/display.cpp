#include "globals.h"

#define REFRESH_INTERVAL 1000
#define MESSAGE_COUNT 4

////
typedef struct
{
    char textBuffer[256] = "HIGH ORDER";
    uint8_t textBufferLength = 10;
    uint8_t duration = 10;
    uint8_t speed = 10;
} message;

message messages[MESSAGE_COUNT];
uint8_t messageIndex = 0;
uint lastMessageCycleTime = 0;

////

#include "I2SClocklessLedDriver.h"

I2SClocklessLedDriver driver;

#define LEDS_PER_STRIP 256
#define STRIPS 12

#define ROWS 8
#define COLUMNS ((LEDS_PER_STRIP * STRIPS) / ROWS)

uint8_t leds[STRIPS * LEDS_PER_STRIP * 3];
int pins[STRIPS] = {
    32, 33, 25, 26, 27, 14,
    19, 18, 05, 17, 16, 04};

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
    char *rawTextBuffer = messages[messageIndex].textBuffer;
    uint8_t rawTextBufferLength = messages[messageIndex].textBufferLength;

    // Process variables.
    String stringBuffer = String(rawTextBuffer, rawTextBufferLength);
    char time[9];
    timeNow(time);
    stringBuffer.replace("{{time}}", time);

    // Write to processed buffer.
    char processedTextBuffer[256];
    int processedTextBufferLength = 0;

    strcpy(processedTextBuffer, stringBuffer.c_str());
    processedTextBufferLength = stringBuffer.length();

    // Update pixel buffer.
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
}

void displaySetBrightness(byte brightness)
{
    driver.setBrightness(brightness);
}

void displayLoadMessages()
{
    preferences.begin("messages");

    size_t bufferLength = preferences.getBytesLength("messages");
    char buffer[bufferLength];

    if (bufferLength == sizeof(messages))
    {
        Serial.println("Loading messages!");
        preferences.getBytes("messages", messages, bufferLength);
    }
    else
    {
        Serial.println("No saved messages!");
        preferences.putBytes("messages", messages, sizeof(messages));
    }

    preferences.end();
}

void displaySaveMessages()
{
    preferences.begin("messages");
    preferences.putBytes("messages", messages, sizeof(messages));
    preferences.end();
}

extern void displaySetMessage(int index, const char *message, int length, int speed, int duration)
{
    messages[index].duration = duration;
    messages[index].speed = speed;
    messages[index].textBufferLength = length;
    strcpy(messages[index].textBuffer, message);
    displaySaveMessages();
}

void displaySetup()
{
    // strcpy(messages[0].textBuffer, "AAAAAAAA");
    // messages[0].textBufferLength = 8;
    // strcpy(messages[1].textBuffer, "{{time}}");
    // messages[1].textBufferLength = 8;
    // messages[1].speed = 0;
    // strcpy(messages[2].textBuffer, "CCCCCCCC");
    // messages[2].textBufferLength = 8;
    // strcpy(messages[3].textBuffer, "DDDDDDDD");
    // messages[3].textBufferLength = 8;
    displayLoadMessages();

    driver.initled(leds, pins, STRIPS, LEDS_PER_STRIP, ORDER_GRB);

    for (int i = 0; i < LEDS_PER_STRIP * STRIPS; i++)
    {
        driver.setPixel(i, 0, 0, 0);
    }

    displaySetBrightness(8);
    displayRefresh();

    driver.showPixels();
}

void displayLoop()
{
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

    // Handle scrolling.
    if (messages[messageIndex].speed > 0)
    {
        if (millis() - lastOffsetTime > (1000.0 / messages[messageIndex].speed))
        {
            offset++;
            lastOffsetTime = millis();
        }
    }

    // Handle swapping.
    if (millis() - lastMessageCycleTime > (messages[messageIndex].duration * 1000.0))
    {
        messageIndex = (messageIndex + 1) % MESSAGE_COUNT;
        offset = 0;
        displayRefresh();
        lastMessageCycleTime = millis();
    }

    // Handle refresh interval.
    if (millis() - lastRefreshTime > REFRESH_INTERVAL)
    {
        displayRefresh();
        lastRefreshTime = millis();
    }

    driver.showPixels();
}