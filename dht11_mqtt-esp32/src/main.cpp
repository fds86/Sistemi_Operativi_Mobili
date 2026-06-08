#include <Arduino.h>
#include <DHTesp.h>
#include "dht11_utils.h"
#include "dht11_rest.h"

DHTesp dht;

static Dht11Data sensorData = {false, NAN, NAN, 0, 0};
static unsigned long lastPrintedTs = 0;
static bool lastValidity = false;

void setup()
{
    Serial.begin(9600);
    initDht11Sensor(&sensorData);
    initDht11Rest(&sensorData);
}

void loop()
{
    readDht11SensorWithTimestamp(&sensorData);
    handleDht11Rest();

    if (true == sensorData.isDataValid && 
        sensorData.timestamp_Ms != lastPrintedTs)
    {
        lastPrintedTs = sensorData.timestamp_Ms;

        Serial.print("Temperature: ");
        Serial.print(sensorData.temperature_C);
        Serial.print(" [C]");
        Serial.print(" | ");

        Serial.print("Humidity: ");
        Serial.print(sensorData.humidity_Pct);
        Serial.println(" [%]");
    }
    else if (false == sensorData.isDataValid && 
             true == lastValidity)
    {
        Serial.println("Invalid DHT11 reading");
    }

    lastValidity = sensorData.isDataValid;
}