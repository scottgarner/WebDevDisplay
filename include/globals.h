#ifndef GLOBALS_H
#define GLOBALS_H

#include <Arduino.h>
#include <LittleFS.h>

#include "config.h"

struct PixelDataHeader
{
    uint16_t width;    // Width of the graphic
    uint16_t height;   // Height of the graphic
    uint16_t bitDepth; // Bit depth (e.g., 8, 16, 24)
};

extern void networkingSetup();
extern void networkingLoop();

extern void displaySetup();
extern void displayLoop();
extern void displaySetBuffer(uint8_t *buffer, size_t bufferSize, bool save);

#endif