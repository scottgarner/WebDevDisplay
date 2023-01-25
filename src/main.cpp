#include "globals.h"

void setup()
{
  Serial.begin(115200);
  Serial.println("READY");

  displaySetup();
  networkingSetup();

  displayText("ABCDEFGH", 8);
}

void loop()
{
  displayLoop();
  networkingLoop();
}