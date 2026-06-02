#include <Arduino.h>
#include <DHTesp.h>
#include "dht11_utils.h"

DHTesp dht;

static Dht11Data sensorData = {NAN, NAN, 0, false};
static unsigned long lastPrintedTs = 0;

void setup()
{
    Serial.begin(9600);
    initDHT();
}

void loop()
{
    readDHTWithTimestamp(&sensorData);

    if (true == sensorData.valid && 
        sensorData.timestampMs != lastPrintedTs)
    {
        lastPrintedTs = sensorData.timestampMs;

        Serial.print("Temperature: ");
        Serial.print(sensorData.temperatureC);
        Serial.print(" [C]");
        Serial.print(" | ");

        Serial.print("Humidity: ");
        Serial.print(sensorData.humidityPct);
        Serial.println(" [%]");
    }
}