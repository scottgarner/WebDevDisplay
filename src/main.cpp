#include "globals.h"

#define configCHECK_FOR_STACK_OVERFLOW 2

void displayTask(void *parameter)
{
  while (1)
  {
    displayLoop();
  }
}

void networkingTask(void *parameter)
{
  while (1)
  {
    networkingLoop();
  }
}

void setup()
{
  Serial.begin(115200);
  Serial.println("READY");

  dataLoad();
  displaySetup();
  networkingSetup([]()
                  { timeSetup(); });

  xTaskCreate(
      displayTask,   /* Task function. */
      "displayTask", /* String with name of task. */
      1024 * 8,      /* Stack size in bytes. */
      NULL,          /* Parameter passed as input of the task */
      1,             /* Priority of the task. */
      NULL);         /* Task handle. */

  xTaskCreate(
      networkingTask,   /* Task function. */
      "networkingTask", /* String with name of task. */
      1024 * 8,         /* Stack size in bytes. */
      NULL,             /* Parameter passed as input of the task */
      1,                /* Priority of the task. */
      NULL);            /* Task handle. */
}

void loop()
{
  // displayLoop();
  // networkingLoop();
}