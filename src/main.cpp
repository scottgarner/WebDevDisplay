#include "globals.h"

Preferences preferences;

void setup()
{
  Serial.begin(115200);
  Serial.println("READY");

  displaySetup();
  networkingSetup([]()
                  { timeSetup(); });
}

void loop()
{
  displayLoop();
  networkingLoop();
}