#include "globals.h"
#include "config.h"
#include "lut_8x8.h"

#define STRIPS 1
#include <FastLED.h>

int offsetX = 0;
int offsetY = 0;

int pins[STRIPS] = {32};

uint8_t graphicsBuffer[16385];
uint16_t graphicsBufferWidth = 0;
uint16_t graphicsBufferHeight = 0;

uint8_t physicalBuffer[ROWS * COLUMNS * 3];

bool refresh = false;

long lastScrollTime = 0;
int scrollDelta = 250;

int wrap(int value, int divisor)
{
    return ((value % divisor) + divisor) % divisor;
}

void graphicsToPhysical()
{
    for (int y = 0; y < ROWS; y++)
    {
        for (int x = 0; x < COLUMNS; x++)
        {

            // Sample graphics buffer.
            int graphicsBufferX = wrap(x + offsetX, graphicsBufferWidth);
            int graphicsBufferY = wrap(y + offsetY, graphicsBufferHeight);
            int graphicsIndex = (graphicsBufferY * graphicsBufferWidth + graphicsBufferX) * 3;

            // Get the physical positions from the LUT.
            int logicalIndex = y * COLUMNS + x;
            int physicalIndexRed = lut[logicalIndex] * 3;
            int physicalIndexGreen = lut[logicalIndex] * 3 + 1;
            int physicalIndexBlue = lut[logicalIndex] * 3 + 2;

            uint8_t red = graphicsBuffer[graphicsIndex];
            uint8_t green = graphicsBuffer[graphicsIndex + 1];
            uint8_t blue = graphicsBuffer[graphicsIndex + 2];

            // Copy RGB values to the physical buffer.
            physicalBuffer[physicalIndexRed] = red;
            physicalBuffer[physicalIndexGreen] = green;
            physicalBuffer[physicalIndexBlue] = blue;
            /*
                        Serial.print(" COLUMN: ");
                        Serial.print(x);
                        Serial.print(" ROW: ");
                        Serial.print(y);

                        Serial.print(" Logical Index: ");
                        Serial.print(logicalIndex);
                        Serial.print(" Graphics Index: ");
                        Serial.print(graphicsIndex);

                        Serial.print(" Red: ");
                        Serial.print(red);
                        Serial.print(" Green: ");
                        Serial.print(green);
                        Serial.print(" Blue: ");
                        Serial.print(blue);

                        Serial.print(" Physical Index Red: ");
                        Serial.print(physicalIndexRed);
                        Serial.print(" Physical Index Green: ");
                        Serial.print(physicalIndexGreen);
                        Serial.print(" Physical Index Blue: ");
                        Serial.print(physicalIndexBlue);

                        Serial.println();
            */
        }
    }
    /*
        for (int i = 0; i < ROWS * COLUMNS * 3; i++)
        {
            Serial.print(physicalBuffer[i]);
            Serial.print(',');
        }
        Serial.println();
    */
}

void loadBuffer()
{
    if (!LittleFS.begin(true))
    {
        Serial.println("Failed to mount LittleFS!");
        return;
    }

    PixelDataHeader *header;
    File file = LittleFS.open("/buffer.dat", FILE_READ);

    if (!file)
    {
        Serial.println("Failed to open file for reading");
        return;
    }

    size_t fileSize = file.size();           // Get the total size of the file
    uint8_t *buffer = new uint8_t[fileSize]; // Dynamically allocate memory for the file contents

    if (!buffer)
    {
        Serial.println("Failed to allocate memory for file data");
        file.close();
        return;
    }

    file.read(buffer, fileSize); // Read the entire file into the buffer
    file.close();

    displaySetBuffer(buffer, fileSize, false);

    Serial.println("Buffer successfully loaded from LittleFS");
    delete[] buffer;
}

void saveBuffer(uint8_t *buffer, size_t bufferSize)
{
    Serial.println("Save buffer!");

    if (!LittleFS.begin(true))
    {
        Serial.println("LittleFS Mount Failed");
        return;
    }

    File file = LittleFS.open("/buffer.dat", FILE_WRITE);
    if (!file)
    {
        Serial.println("Failed to open file for writing");
        return;
    }

    file.write(buffer, bufferSize); // Write the buffer to the file
    file.close();
    Serial.println("Buffer saved successfully");
}

void displaySetup()
{
    Serial.println("Display setup...");

    FastLED.addLeds<NEOPIXEL, DATA_PIN>((CRGB *)physicalBuffer, NUM_LEDS_PER_STRIP);
    FastLED.setBrightness(BRIGHTNESS);

    lastScrollTime = millis();

    graphicsBuffer[0] = 0;
    graphicsBuffer[1] = 0;
    graphicsBuffer[2] = 0;
    graphicsBuffer[3] = 0xFF;
    graphicsBuffer[4] = 0;
    graphicsBuffer[5] = 0;
    graphicsBuffer[6] = 0xFF;
    graphicsBuffer[7] = 0;
    graphicsBuffer[8] = 0;
    graphicsBuffer[9] = 0;
    graphicsBuffer[10] = 0;
    graphicsBuffer[11] = 0;

    graphicsBufferWidth = 2;
    graphicsBufferHeight = 2;

    loadBuffer();

    refresh = true;

    Serial.println("Display setup complete...");
}

void displaySetBuffer(uint8_t *buffer, size_t bufferSize, bool save = false)
{
    saveBuffer(buffer, bufferSize);

    PixelDataHeader *header = (PixelDataHeader *)buffer;
    uint8_t *pixelData = buffer + sizeof(PixelDataHeader);
    /*
        Serial.print("Width: ");
        Serial.println(header->width);
        Serial.print("Height: ");
        Serial.println(header->height);
        Serial.print("Depth: ");
        Serial.println(header->bitDepth);
        Serial.println("Display set graphics buffer...");
    */

    // Ensure the buffer is not larger than the memory can handle
    uint32_t pixelBufferSize = header->width * header->height * 3;
    if (pixelBufferSize > sizeof(graphicsBuffer))
    {
        Serial.println("Error: Buffer size exceeds available memory.");
        return;
    }

    memcpy(graphicsBuffer, pixelData, bufferSize);
    graphicsBufferWidth = header->width;
    graphicsBufferHeight = header->height;

    refresh = true;
}

void displayLoop()
{
    if (refresh)
    {
        graphicsToPhysical();
        // driver.showPixels();
        FastLED.show();
        // Serial.println("Refresh!");
        refresh = false;
    }

    if (millis() - lastScrollTime > scrollDelta)
    {
        lastScrollTime = millis();

        if (graphicsBufferWidth > COLUMNS)
        {
            offsetX = (offsetX + 1) % graphicsBufferWidth;
            refresh = true;
        }

        // offsetY = (offsetY + 1) % ROWS;
    }
}
