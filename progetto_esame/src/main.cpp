#include <Arduino.h>
#include "task_manager.h"

void setup()
{
    Serial.begin(9600);
    SetupTaskManager();
}

void loop()
{
    delay(1000);
}