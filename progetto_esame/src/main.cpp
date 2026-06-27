#include <Arduino.h>
#include "task_manager.h"

void setup()
{
    Serial.begin(9600);
    TaskManager_SetupTaskManager();
}

void loop()
{
    delay(1000);
}