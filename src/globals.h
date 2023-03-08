#ifndef GLOBALS_H
#define GLOBALS_H

#include <Arduino.h>
#include <Preferences.h>

extern Preferences preferences;

extern void networkingSetup(void (*callback)());
extern void networkingLoop();

extern void displaySetup();
extern void displayLoop();
extern void displaySetBrightness(byte);
extern void displaySetMessage(int index, const char *message, int length, int speed, int duration);

extern void timeSetup();
extern void timePrint();
extern void timeNow(char *time);

#endif