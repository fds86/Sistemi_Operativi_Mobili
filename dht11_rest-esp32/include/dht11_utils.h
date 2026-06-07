#ifndef DHT11_UTILS_H
#define DHT11_UTILS_H

#include <Arduino.h>
#include <DHTesp.h>
#include <math.h>

#define DHT_PIN 18 /**< GPIO pin used by DHT11 data line. */

#define READ_PERIOD_MS 2000 /**< Minimum sampling period for DHT11 updates in milliseconds. */

/** @brief Global DHTesp instance provided by the application source file. */
extern DHTesp dht;

/**
 * @struct Dht11Data
 * @brief Cached DHT11 values with validity and timestamp metadata.
 */
typedef struct 
{
    bool isDataValid;                /**< True when cached values are valid. */
    float temperature_C;             /**< Cached temperature in Celsius. */
    float humidity_Pct;              /**< Cached relative humidity in percent. */
    unsigned int deltaReadPeriod_Ms; /**< Minimum sampling period for DHT11 updates in milliseconds. */
    unsigned long timestamp_Ms;      /**< Timestamp of the last sampling attempt in milliseconds. */
} Dht11Data;

/**
 * @brief Resets the cached DHT11 data structure.
 * @param data Pointer to the structure to reset.
 */
inline void resetDht11SensorData(Dht11Data *data)
{
    data->isDataValid = false;
    data->temperature_C = NAN;
    data->humidity_Pct = NAN;
    data->timestamp_Ms = 0U;
    data->deltaReadPeriod_Ms = 0U;
}

/**
 * @brief Initializes the DHT11 sensor driver.
 */
inline void initDht11Sensor(Dht11Data *data) 
{
    dht.setup(DHT_PIN, DHTesp::DHT11);
    Serial.println("DHT11 ready");
    resetDht11SensorData(data);
}

/**
 * @brief Performs an immediate DHT11 read.
 * @return Raw temperature and humidity values from the DHT library.
 */
inline TempAndHumidity readDht11Sensor() 
{
    TempAndHumidity values = dht.getTempAndHumidity();
    return values;
}

/**
 * @brief Updates cached DHT11 data if the sampling period has elapsed.
 * @param data Pointer to the destination cache structure.
 * @note The caller must provide a valid non-null pointer.
 */
inline void readDht11SensorWithTimestamp(Dht11Data *data) 
{
    unsigned long now = millis();
    data->deltaReadPeriod_Ms = now - data->timestamp_Ms;
    
    if (data->deltaReadPeriod_Ms >= READ_PERIOD_MS)
    {
        data->timestamp_Ms = now;

        TempAndHumidity values = dht.getTempAndHumidity();
        
        if (false == isnan(values.temperature) && 
            false == isnan(values.humidity)) 
        {
            data->isDataValid = true;
            data->temperature_C = values.temperature;
            data->humidity_Pct = values.humidity;
        }
        else 
        {
            data->isDataValid = false;
        }
    }
}

#endif // DHT11_UTILS_H