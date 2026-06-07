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
    bool valid;               /**< True when cached values are valid. */
    float temperatureC;       /**< Cached temperature in Celsius. */
    float humidityPct;        /**< Cached relative humidity in percent. */
    unsigned long timestampMs;/**< Timestamp of the last sampling attempt in milliseconds. */
} Dht11Data;

/**
 * @brief Initializes the DHT11 sensor driver.
 */
inline void initDht11Sensor() 
{
    dht.setup(DHT_PIN, DHTesp::DHT11);
    Serial.println("DHT11 ready");
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
    
    if (now - data->timestampMs >= READ_PERIOD_MS)
    {
        data->timestampMs = now;

        TempAndHumidity values = dht.getTempAndHumidity();
        
        if (false == isnan(values.temperature) && 
            false == isnan(values.humidity)) 
        {
            data->valid = true;
            data->temperatureC = values.temperature;
            data->humidityPct = values.humidity;
        }
        else 
        {
            data->valid = false;
        }
    }
}

#endif // DHT11_UTILS_H