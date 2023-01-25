
#include <Arduino.h>

extern void networkingSetup();
extern void networkingLoop();

extern void displaySetup();
extern void displayLoop();
extern void displayBrightness(byte);
extern void displayText(const char *, int);