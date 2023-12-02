#include "globals.h"

// #define configCHECK_FOR_STACK_OVERFLOW 2

// void networkingTask(void *parameter)
// {
//   while (1)
//   {
//     networkingLoop();
//     taskYIELD();
//   }
// }

void setup()
{
  Serial.begin(115200);
  Serial.println("READY");

  dataLoad();
  displaySetup();
  networkingSetup([]()
                  { timeSetup(); });

  // xTaskCreatePinnedToCore(
  //     networkingTask,   /* Task function. */
  //     "networkingTask", /* String with name of task. */
  //     1024 * 8,         /* Stack size in bytes. */
  //     NULL,             /* Parameter passed as input of the task */
  //     4,                /* Priority of the task. */
  //     NULL,             /* Task handle. */
  //     0);               /* CPU core. */
}

void loop()
{
  displayLoop();
  networkingLoop();
}