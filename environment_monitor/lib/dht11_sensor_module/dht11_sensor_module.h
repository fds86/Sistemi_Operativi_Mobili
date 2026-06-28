#ifndef DHT11_SENSOR_MODULE_H
#define DHT11_SENSOR_MODULE_H

#include <DHTesp.h>
#include "project_config.h"

/**
 * @file dht11_sensor_module.h
 * @brief DHT11 acquisition module API.
 */

/**
 * @brief Configures the DHT11 driver on the configured GPIO.
 * @param dhtSensor Pointer to the DHTesp instance used by the caller.
 */
void Dht11SensorModule_InitializeDht11Sensor(DHTesp *dhtSensor);

/**
 * @brief Reads DHT11 values and updates output parameters.
 * @param dhtSensor Pointer to the DHTesp instance used by the caller.
 * @param temperatureC Pointer to output temperature in Celsius.
 * @param humidityPct Pointer to output humidity percentage.
 * @param isValid Pointer to output validity flag.
 */
void Dht11SensorModule_ReadDht11Sensor(DHTesp *dhtSensor,
                                       float *temperatureC,
                                       float *humidityPct,
                                       bool *isValid);

#endif