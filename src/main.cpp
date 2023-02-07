#include "globals.h"

#include <Ticker.h>

Ticker ticker;

void setup()
{
  Serial.begin(115200);
  Serial.println("READY");

  displaySetup();
  networkingSetup([]()
                  { timeSetup(); });

  displaySetRawText("HIGH ORDER", 8);

  ticker.attach(1, []()
                { displayProcessRawText(); });
}

void loop()
{
  displayLoop();
  networkingLoop();
}