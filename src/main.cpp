#include "globals.h"

void setup()
{
  Serial.begin(115200);

  displaySetup();
  networkingSetup();
}

void loop()
{
  displayLoop();
  networkingLoop();
}