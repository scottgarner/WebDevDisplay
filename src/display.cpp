#include "globals.h"

#include <FastLED.h>

uint8_t graphicsBuffer[GRAPHICS_BUFFER_MAX];
uint16_t graphicsBufferWidth = 0;
uint16_t graphicsBufferHeight = 0;

uint8_t physicalBuffer[ROWS * COLUMNS * 3];

bool refresh = false;

int scrollSpeedX = 0;
int scrollSpeedY = 0;

int offsetX = 0;
int offsetY = 0;

long lastScrollTimeY = 0;
long lastScrollTimeX = 0;

int wrap(int value, int divisor)
{
    return ((value % divisor) + divisor) % divisor;
}

void graphicsToPhysical()
{
    if (graphicsBufferWidth == 0 || graphicsBufferHeight == 0)
    {
        Serial.println("Invalid graphics buffer dimensions.");
        return;
    }

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
        }
    }
}

void loadBuffer()
{
    Serial.println("Loading saved buffer...");

    File file = LittleFS.open("/buffer.dat", FILE_READ);

    if (!file)
    {
        Serial.println("Failed to open file for reading.");
        return;
    }

    size_t fileSize = file.size();
    uint8_t *buffer = new uint8_t[fileSize];

    if (!buffer)
    {
        Serial.println("Failed to allocate memory for file data.");
        file.close();
        return;
    }

    Serial.println("Reading buffer.");
    file.read(buffer, fileSize);
    file.close();

    displaySetBuffer(buffer, fileSize);

    Serial.println("Buffer successfully loaded from LittleFS");
    delete[] buffer;
}

void saveBuffer(uint8_t *buffer, size_t bufferSize)
{
    Serial.println("Save buffer!");

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

    // 32, 33, 25, 26
    FastLED.addLeds<NEOPIXEL, 32>((CRGB *)physicalBuffer, 0, LEDS_PER_STRIP);
    FastLED.addLeds<NEOPIXEL, 33>((CRGB *)physicalBuffer, 1 * LEDS_PER_STRIP, LEDS_PER_STRIP);
    FastLED.addLeds<NEOPIXEL, 25>((CRGB *)physicalBuffer, 2 * LEDS_PER_STRIP, LEDS_PER_STRIP);
    FastLED.addLeds<NEOPIXEL, 26>((CRGB *)physicalBuffer, 3 * LEDS_PER_STRIP, LEDS_PER_STRIP);

    FastLED.setBrightness(BRIGHTNESS);

    lastScrollTimeX = millis();
    lastScrollTimeY = millis();

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

    Serial.println("Prepping initial buffer.");
    graphicsToPhysical();

    Serial.println("Displaying initial buffer.");
    FastLED.show();

    Serial.println("Display setup complete...");
}

void displaySetBuffer(uint8_t *buffer, size_t bufferSize)
{

    PixelDataHeader *header = (PixelDataHeader *)buffer;
    uint8_t *pixelData = buffer + sizeof(PixelDataHeader);

    Serial.print("Width: ");
    Serial.print(header->width);
    Serial.print(" Height: ");
    Serial.println(header->height);
    Serial.print("\n");

    uint32_t pixelBufferSize = header->width * header->height * 3;
    if (pixelBufferSize > sizeof(graphicsBuffer))
    {
        Serial.println("Error: Buffer size exceeds available memory.");
        return;
    }

    if (header->saveBuffer)
    {
        header->saveBuffer = false;
        saveBuffer(buffer, bufferSize);
    }

    memcpy(graphicsBuffer, pixelData, bufferSize);

    scrollSpeedX = header->scrollSpeedX;
    scrollSpeedY = header->scrollSpeedY;

    graphicsBufferWidth = header->width;
    graphicsBufferHeight = header->height;

    if (scrollSpeedX == 0)
    {
        offsetX = 0;
    }

    if (scrollSpeedY == 0)
    {
        offsetY = 0;
    }

    Serial.print(" Scroll speed X: ");
    Serial.print(scrollSpeedX);
    Serial.print(" Scroll speed Y: ");
    Serial.println(scrollSpeedY);

    refresh = true;
}

void displayLoop()
{
    // X-axis scroll.
    if (scrollSpeedX > 0)
    {
        uint scrollDeltaX = 1000 / scrollSpeedX;
        if (millis() - scrollDeltaX > lastScrollTimeX)
        {
            offsetX = (offsetX + 1) % graphicsBufferWidth;
            lastScrollTimeX = millis();
            refresh = true;
        }
    }

    // Y-axis scroll.
    if (scrollSpeedY > 0)
    {
        uint scrollDeltaY = 1000 / scrollSpeedY;
        if (millis() - scrollDeltaY > lastScrollTimeY)
        {
            offsetY = (offsetY + 1) % graphicsBufferHeight;
            lastScrollTimeY = millis();
            refresh = true;
        }
    }

    // Refresh as needed.
    if (refresh)
    {
        graphicsToPhysical();
        FastLED.show();

        refresh = false;
    }
}