#include <Arduino.h>
#include "task_manager.h"

/**
 * @brief Initializes serial output and starts the task manager.
 */
void setup();

/**
 * @brief Keeps the Arduino loop alive while FreeRTOS tasks run.
 */
void loop();

void setup()
{
    Serial.begin(9600);
    TaskManager_SetupTaskManager();
}

void loop()
{
    delay(1000);
}