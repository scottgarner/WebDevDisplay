#ifndef GLOBALS_H
#define GLOBALS_H

#include <Arduino.h>
#include <LittleFS.h>

#include "config.h"

#define GRAPHICS_BUFFER_MAX 1 << 14
#define HTTP_PORT 80
#define OTA_PORT 3232

struct PixelDataHeader
{
    uint16_t mode;
    uint16_t width;
    uint16_t height;
    uint16_t scrollSpeedX;
    uint16_t scrollSpeedY;
    uint16_t saveBuffer;
};

extern void networkingSetup();
extern void networkingLoop();

extern void displaySetup();
extern void displayLoop();
extern void displaySetBuffer(uint8_t *buffer, size_t bufferSize);

#endif