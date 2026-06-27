#ifndef DHT11_SENSOR_MODULE_H
#define DHT11_SENSOR_MODULE_H

#include <DHTesp.h>
#include "project_config.h"

void Dht11SensorModule_InitializeDht11Sensor(DHTesp *dhtSensor);

void Dht11SensorModule_ReadDht11Sensor(DHTesp *dhtSensor,
                                       float *temperatureC,
                                       float *humidityPct,
                                       bool *isValid);

#endif