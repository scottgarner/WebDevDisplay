
#include <Arduino.h>

extern void networkingSetup(void (*callback)());
extern void networkingLoop();

extern void displaySetup();
extern void displayLoop();
extern void displaySetBrightness(byte);
extern void displaySetRawText(const char *, int);
extern void displayUpdatePixelBuffer();
extern void displayProcessRawText();

extern void timeSetup();
extern void timePrint();
extern void timeNow(char *time);