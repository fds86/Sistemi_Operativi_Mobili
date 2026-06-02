#include <Arduino.h>
#include <DHTesp.h>

static const uint8_t DHT_PIN = 18;
static DHTesp dht;

void setup()
{
    Serial.begin(9600);
    delay(1000);

    dht.setup(DHT_PIN, DHTesp::DHT11);
    Serial.println("DHT11 ready");
}

unsigned long lastRead = 0;
const unsigned long READ_PERIOD_MS = 2000;

float lastTemp = NAN, lastHum = NAN;
bool sensorValid = false;

void loop()
{
    unsigned long now = millis();
    if (now - lastRead >= READ_PERIOD_MS) {
        lastRead = now;

        // lettura sensore (DHT11 resta relativamente lenta, ma senza delay espliciti)
        float t = dht.getTemperature();
        float h = dht.getHumidity();

        if (!isnan(t) && !isnan(h)) {
            lastTemp = t;
            lastHum = h;
            sensorValid = true;
        } else {
            sensorValid = false;
        }
    }

    if (sensorValid) 
    {
        Serial.print("Temperature: ");
        Serial.print(lastTemp);
        Serial.print(" [C]");
        Serial.print(" | ");

        Serial.print("Humidity: ");
        Serial.print(lastHum);
        Serial.println(" [%]");
    }

    delay(2000);
}
