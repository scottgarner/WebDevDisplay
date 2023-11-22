
#include "globals.h"
#include "time.h"

const char *ntpServer = "pool.ntp.org";
const long gmtOffset = -8 * 60 * 60;
const int daylightOffset = 3600;

void timeCreateTicker(int seconds, void (*callback)())
{
}

void timePrint()
{
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo))
    {
        Serial.println("Failed to obtain time");
        return;
    }

    char time[9];
    strftime(time, 9, "%H:%M:%S", &timeinfo);
    Serial.println(time);
}

void timeNow(char *time)
{
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo))
    {
        strcpy(time, "00:00:00");
    }
    else
    {
        strftime(time, 9, "%H:%M:%S", &timeinfo);
    }
}

void timeSetup()
{
    configTime(gmtOffset, daylightOffset, ntpServer);
    timePrint();
}
