#ifndef DHT11_SENSOR_MODULE_H
#define DHT11_SENSOR_MODULE_H

#include <Arduino.h>
#include <DHTesp.h>
#include <math.h>
#include "project_config.h"

inline void InitializeDht11Sensor(DHTesp *dhtSensor)
{
    dhtSensor->setup(DHT_PIN, DHTesp::DHT11);
    Serial.println("DHT11 ready");
}

inline void ReadDht11Sensor(
    DHTesp *dhtSensor,
    float *temperatureC,
    float *humidityPct,
    bool *isValid)
{
    TempAndHumidity values = dhtSensor->getTempAndHumidity();

    *isValid = (false == isnan(values.temperature)) &&
               (false == isnan(values.humidity));

    *temperatureC = values.temperature;
    *humidityPct = values.humidity;
}

#endif