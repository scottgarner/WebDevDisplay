#include "globals.h"

void setup()
{
  Serial.begin(115200);
  Serial.println("READY");

  dataLoad();
  displaySetup();
  networkingSetup([]()
                  { timeSetup(); });
}

void loop()
{
  displayLoop();
  networkingLoop();
}