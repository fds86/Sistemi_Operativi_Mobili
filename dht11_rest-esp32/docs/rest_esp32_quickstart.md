# ESP32 REST Quickstart (DHT11)

Guida minima per mettere in piedi una comunicazione REST su ESP32 e leggere i dati da browser.

## 1. Crea il progetto PlatformIO
Descrizione: inizializza un progetto Arduino per ESP32.

```ini
; platformio.ini
[env:esp32doit-devkit-v1]
platform = espressif32
board = esp32doit-devkit-v1
framework = arduino
monitor_speed = 9600
upload_speed = 921600
lib_deps = beegee-tokyo/DHT sensor library for ESPx
```

## 2. Definisci il modello dati del sensore
Descrizione: struttura minima con validita e valori letti.

```cpp
// include/dht11_utils.h (estratto)
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
    bool isDataValid;
    float temperature_C;
    float humidity_Pct;
    unsigned int deltaReadPeriod_Ms;
    unsigned long timestamp_Ms;
} Dht11Data;

inline void initDHTSensor(Dht11Data *data)
{
    dht.setup(DHT_PIN, DHTesp::DHT11);
    data->isDataValid = false;
    data->temperature_C = NAN;
    data->humidity_Pct = NAN;
    data->deltaReadPeriod_Ms = 0U;
    data->timestamp_Ms = 0U;
}

inline void readDHTWithTimestamp(Dht11Data *data)
{
    unsigned long now = millis();
    data->deltaReadPeriod_Ms = now - data->timestamp_Ms;

    if (data->deltaReadPeriod_Ms >= READ_PERIOD_MS)
    {
        data->timestamp_Ms = now;
        TempAndHumidity values = dht.getTempAndHumidity();

        if (!isnan(values.temperature) && !isnan(values.humidity))
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

#endif
```

## 3. Espone API REST e pagina web
Descrizione: ESP32 in Access Point, endpoint JSON e pagina HTML.

```cpp
// include/dht11_rest.h (estratto)
#ifndef DHT11_REST_H
#define DHT11_REST_H

#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include "dht11_utils.h"

#define DHT11_REST_AP_SSID "ESP32-DHT11"
#define DHT11_REST_AP_PASSWORD "esp32dht11"

namespace dht11rest
{
    static WebServer server(80);
    static Dht11Data *dataRef = nullptr;

    inline String makeJsonPayload(const Dht11Data *data)
    {
        String payload = "{";
        payload += "\"valid\":";
        payload += data->isDataValid ? "true" : "false";
        payload += ",\"temperatureC\":";
        payload += data->isDataValid ? String(data->temperature_C, 1) : "null";
        payload += ",\"humidityPct\":";
        payload += data->isDataValid ? String(data->humidity_Pct, 1) : "null";
        payload += ",\"timestampMs\":";
        payload += String(data->timestamp_Ms);
        payload += "}";
        return payload;
    }

    inline void handleApiDht11()
    {
        if (nullptr == dataRef)
        {
            server.send(500, "application/json", "{\"error\":\"sensor not configured\"}");
            return;
        }

        server.send(200, "application/json", makeJsonPayload(dataRef));
    }
}

inline void initDht11Rest(Dht11Data *data)
{
    dht11rest::dataRef = data;

    WiFi.mode(WIFI_AP);
    WiFi.softAP(DHT11_REST_AP_SSID, DHT11_REST_AP_PASSWORD);

    dht11rest::server.on("/api/data", dht11rest::handleApiDht11);
    dht11rest::server.on("/api/dht11", dht11rest::handleApiDht11);
    dht11rest::server.begin();
}

inline void handleDht11Rest()
{
    dht11rest::server.handleClient();
}

#endif
```

## 4. Inizializza tutto in main
Descrizione: setup una volta, loop con lettura sensore + gestione richieste HTTP.

```cpp
// src/main.cpp
#include <Arduino.h>
#include <DHTesp.h>
#include "dht11_utils.h"
#include "dht11_rest.h"

DHTesp dht;

static Dht11Data sensorData = {false, NAN, NAN, 0U, 0U};

void setup()
{
    Serial.begin(9600);
    initDHTSensor(&sensorData);
    initDht11Rest(&sensorData);
}

void loop()
{
    readDHTWithTimestamp(&sensorData);
    handleDht11Rest();
}
```

## 5. Carica firmware e apri il monitor seriale
Descrizione: flash del firmware e controllo avvio AP.

```bash
pio run -e esp32doit-devkit-v1
pio run -e esp32doit-devkit-v1 -t upload
pio device monitor -b 9600
```

Nel monitor seriale cerca l'IP stampato da `WiFi.softAPIP()` (tipicamente `192.168.4.1`).

## 6. Leggi i dati dal browser
Descrizione: connettiti alla Wi-Fi dell'ESP32 e apri endpoint REST.

```text
SSID: ESP32-DHT11
Password: esp32dht11
```

URL utili:

```text
http://192.168.4.1/api/data
http://192.168.4.1/api/dht11
```

Esempio risposta:

```json
{
  "valid": true,
  "temperatureC": 24.2,
  "humidityPct": 56.7,
  "timestampMs": 128450
}
```

## 7. Controllo minimo errori lato web
Descrizione: usa il campo `valid` per capire se il dato e leggibile.

Regola pratica:
- `valid = true`: mostra valori.
- `valid = false`: mostra "Lettura non valida".

---

Questo e il setup minimo per una REST locale su ESP32. Da qui puoi estendere con pagina HTML, storico dati o backend cloud.
