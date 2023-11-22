#ifndef GLOBALS_H
#define GLOBALS_H

#include <Arduino.h>
#include <Preferences.h>

#include "ArduinoJson.h"
#include "credentials.h"

extern Preferences preferences;

extern void networkingSetup(void (*callback)());
extern void networkingLoop();

extern void displaySetup();
extern void displayLoop();
extern void displaySetBrightness(byte);
extern void displaySetMessage(int index);

extern void dataLoad();
extern void dataSave();
extern DynamicJsonDocument dataGetJson();
extern void dataParseJson(uint8_t *data);

extern void timeSetup();
extern void timePrint();
extern void timeNow(char *time);

typedef struct
{
    char text_buffer[128] = "HIGH ORDER ";
    uint8_t text_bufer_length = 11;
    uint8_t duration = 10;
    uint8_t speed = 10;
    uint8_t red = 255;
    uint8_t green = 255;
    uint8_t blue = 255;
} Message;

typedef struct
{
    char ap_ssid[32] = AP_SSID;
    char ap_password[256] = AP_PASSWORD;
    char wifi_ssid[32] = WIFI_SSID;
    char wifi_password[256] = WIFI_PASSWORD;
    uint8_t brightness = 32;
} Configuration;

extern Configuration configuration;
extern Message messages[4];
extern uint messageCount;

#endif