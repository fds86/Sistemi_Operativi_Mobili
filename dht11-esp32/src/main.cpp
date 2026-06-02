#include <Arduino.h>
#include <DHTesp.h>

static const uint8_t DHT_PIN = 4;
static DHTesp dht;

void setup()
{
    Serial.begin(9600);
    delay(1000);

    dht.setup(DHT_PIN, DHTesp::DHT11);
    Serial.println("DHT11 ready");
}

void loop()
{
    TempAndHumidity data = dht.getTempAndHumidity();

    if (isnan(data.temperature) || isnan(data.humidity)) {
        Serial.println("Failed to read from DHT11 sensor");
    } else {
        Serial.print("Temperature: ");
        Serial.print(data.temperature);
        Serial.println(" C");

        Serial.print("Humidity: ");
        Serial.print(data.humidity);
        Serial.println(" %");
    }

    delay(2000);
}
