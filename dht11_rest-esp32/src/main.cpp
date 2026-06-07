#include <Arduino.h>
#include <DHTesp.h>
#include "dht11_utils.h"
#include "dht11_rest.h"

DHTesp dht;

static Dht11Data sensorData = {false, NAN, NAN, 0};
static unsigned long lastPrintedTs = 0;

void setup()
{
    Serial.begin(9600);
    initDHT();
    initDht11Rest(&sensorData);
}

void loop()
{
    readDHTWithTimestamp(&sensorData);
    handleDht11Rest();

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