#include <Arduino.h>
#include <DHTesp.h>
#include <math.h>
#include "dht11_sensor_module.h"

void Dht11SensorModule_InitializeDht11Sensor(DHTesp *dhtSensor)
{
    /* Initialize the DHT11 pin. */
    dhtSensor->setup(DHT_PIN, DHTesp::DHT11);
    Serial.println("DHT11 ready");
}

void Dht11SensorModule_ReadDht11Sensor(DHTesp *dhtSensor,
                                       float *temperatureC,
                                       float *humidityPct,
                                       bool *isValid)
{
    *isValid = false;
    TempAndHumidity values = {NAN, NAN};

    /* Get sensor values. */
    values = dhtSensor->getTempAndHumidity();

    /* DHT11 may return NaN on transient read errors. */
    if ((false == isnan(values.temperature)) &&
        (false == isnan(values.humidity)))
    {
        *isValid = true;
    }

    /* Return sensor values. */
    *temperatureC = values.temperature;
    *humidityPct = values.humidity;
}
