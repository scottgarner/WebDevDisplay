#include "globals.h"

void setup()
{
  Serial.begin(115200);
  Serial.println("READY");

  networkingSetup();
  setupDisplay();

  displayText("ABCDEFGH", 8);
}

void loop()
{
  networkingLoop();
  loopDisplay();
}