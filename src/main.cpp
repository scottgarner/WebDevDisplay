#include "globals.h"

void setup()
{
  Serial.begin(115200);

  if (!LittleFS.begin(true))
  {
    Serial.println("An Error has occurred while mounting LittleFS!");
  }

  networkingSetup();
  displaySetup();
}

void loop()
{
  networkingLoop();
  displayLoop();
}