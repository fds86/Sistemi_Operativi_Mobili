#include <Arduino.h>
#include <DHTesp.h>
#include <math.h>
#include "dht11_sensor_module.h"

void Dht11SensorModule_InitializeDht11Sensor(DHTesp *dhtSensor)
{
    dhtSensor->setup(DHT_PIN, DHTesp::DHT11);
    Serial.println("DHT11 ready");
}

void Dht11SensorModule_ReadDht11Sensor(DHTesp *dhtSensor,
                                       float *temperatureC,
                                       float *humidityPct,
                                       bool *isValid)
{
    TempAndHumidity values = dhtSensor->getTempAndHumidity();

    /* DHT11 may return NaN on transient read errors. */
    *isValid = (false == isnan(values.temperature)) &&
               (false == isnan(values.humidity));

    *temperatureC = values.temperature;
    *humidityPct = values.humidity;
}
