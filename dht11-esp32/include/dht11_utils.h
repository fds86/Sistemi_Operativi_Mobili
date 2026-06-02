#ifndef DHT11_UTILS_H
#define DHT11_UTILS_H

#include <Arduino.h>
#include <DHTesp.h>
#include <math.h>

#define DHT_PIN 18

#define READ_PERIOD_MS 2000

extern DHTesp dht;

typedef struct 
{
    bool valid;
    float temperatureC;
    float humidityPct;
    unsigned long timestampMs;
} Dht11Data;

inline void initDHT() 
{
    dht.setup(DHT_PIN, DHTesp::DHT11);
    Serial.println("DHT11 ready");
}

inline TempAndHumidity readDHT() 
{
    TempAndHumidity values = dht.getTempAndHumidity();
    return values;
}

inline void readDHTWithTimestamp(Dht11Data *data) 
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