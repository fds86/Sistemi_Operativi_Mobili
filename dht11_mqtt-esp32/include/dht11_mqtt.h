#ifndef DHT11_MQTT_H
#define DHT11_MQTT_H

#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "dht11_utils.h"

#define DHT11_WIFI_SSID "SSID_XXX"
#define DHT11_WIFI_PASSWORD "PASSWORD_XXX"

#define DHT11_MQTT_BROKER "test.mosquitto.org"
#define DHT11_MQTT_PORT 1883
#define DHT11_MQTT_CLIENT_ID "esp32-dht11-client"
#define DHT11_MQTT_TELEMETRY_TOPIC "sistemi_operativi_mobili/dht11/telemetry"
#define DHT11_MQTT_STATUS_TOPIC "sistemi_operativi_mobili/dht11/status"

#define WIFI_RECONNECT_PERIOD_MS 5000U
#define MQTT_RECONNECT_PERIOD_MS 3000U

namespace dht11mqtt
{
    static WiFiClient wifiClient;
    static PubSubClient mqttClient(wifiClient);
    static Dht11Data *dataRef = nullptr;

    static unsigned long lastWifiAttemptMs = 0U;
    static unsigned long lastMqttAttemptMs = 0U;
    static unsigned long lastPublishedTimestampMs = 0U;

    inline bool hasWifiCredentials()
    {
        return (strlen(DHT11_WIFI_SSID) > 0U);
    }

    inline bool hasBrokerAddress()
    {
        return (strlen(DHT11_MQTT_BROKER) > 0U);
    }

    inline void connectWifiIfNeeded()
    {
        if (WL_CONNECTED == WiFi.status())
        {
            return;
        }

        if (false == hasWifiCredentials())
        {
            return;
        }

        unsigned long now = millis();
        if ((now - lastWifiAttemptMs) < WIFI_RECONNECT_PERIOD_MS)
        {
            return;
        }

        lastWifiAttemptMs = now;
        Serial.print("Connecting WiFi SSID: ");
        Serial.println(DHT11_WIFI_SSID);
        WiFi.begin(DHT11_WIFI_SSID, DHT11_WIFI_PASSWORD);
    }

    inline void publishOnlineStatus()
    {
        mqttClient.publish(DHT11_MQTT_STATUS_TOPIC, "online", true);
    }

    inline void connectMqttIfNeeded()
    {
        if (false == hasBrokerAddress())
        {
            return;
        }

        if (false == mqttClient.connected() && WL_CONNECTED == WiFi.status())
        {
            unsigned long now = millis();
            if ((now - lastMqttAttemptMs) < MQTT_RECONNECT_PERIOD_MS)
            {
                return;
            }

            lastMqttAttemptMs = now;

            Serial.print("Connecting MQTT broker: ");
            Serial.print(DHT11_MQTT_BROKER);
            Serial.print(":");
            Serial.println(DHT11_MQTT_PORT);

            bool connected = mqttClient.connect(
                DHT11_MQTT_CLIENT_ID,
                DHT11_MQTT_STATUS_TOPIC,
                1,
                true,
                "offline");

            if (true == connected)
            {
                Serial.println("MQTT connected");
                publishOnlineStatus();
            }
            else
            {
                Serial.print("MQTT connect failed. rc=");
                Serial.println(mqttClient.state());
            }
        }
    }

    inline bool makeJsonPayload(const Dht11Data *data, char *buffer, size_t bufferSize)
    {
        if (nullptr == data || nullptr == buffer || 0U == bufferSize)
        {
            return false;
        }

        int written = 0;

        if (true == data->isDataValid)
        {
            written = snprintf(
                buffer,
                bufferSize,
                "{\"valid\":true,\"temperatureC\":%.1f,\"humidityPct\":%.1f,\"timestampMs\":%lu}",
                data->temperature_C,
                data->humidity_Pct,
                data->timestamp_Ms);
        }
        else
        {
            written = snprintf(
                buffer,
                bufferSize,
                "{\"valid\":false,\"temperatureC\":null,\"humidityPct\":null,\"timestampMs\":%lu}",
                data->timestamp_Ms);
        }

        return (written > 0) && (static_cast<size_t>(written) < bufferSize);
    }

    inline void publishTelemetryIfUpdated()
    {
        if (nullptr == dataRef || false == mqttClient.connected())
        {
            return;
        }

        if (0U == dataRef->timestamp_Ms)
        {
            return;
        }

        if (dataRef->timestamp_Ms == lastPublishedTimestampMs)
        {
            return;
        }

        char payload[128] = {0};
        if (false == makeJsonPayload(dataRef, payload, sizeof(payload)))
        {
            Serial.println("Telemetry payload error");
            return;
        }

        bool isPublished = mqttClient.publish(DHT11_MQTT_TELEMETRY_TOPIC, payload);
        if (true == isPublished)
        {
            lastPublishedTimestampMs = dataRef->timestamp_Ms;
            Serial.print("MQTT telemetry -> ");
            Serial.println(payload);
        }
        else
        {
            Serial.println("MQTT publish failed");
        }
    }
}

inline void initDht11Mqtt(Dht11Data *data)
{
    dht11mqtt::dataRef = data;

    WiFi.mode(WIFI_STA);
    WiFi.setAutoReconnect(true);
    WiFi.persistent(false);

    dht11mqtt::mqttClient.setServer(DHT11_MQTT_BROKER, DHT11_MQTT_PORT);

    if (false == dht11mqtt::hasWifiCredentials())
    {
        Serial.println("Set DHT11_WIFI_SSID and DHT11_WIFI_PASSWORD in dht11_mqtt.h");
    }

    if (false == dht11mqtt::hasBrokerAddress())
    {
        Serial.println("Set DHT11_MQTT_BROKER in dht11_mqtt.h");
    }

    dht11mqtt::connectWifiIfNeeded();
}

inline void handleDht11Mqtt()
{
    dht11mqtt::connectWifiIfNeeded();
    dht11mqtt::connectMqttIfNeeded();

    if (true == dht11mqtt::mqttClient.connected())
    {
        dht11mqtt::mqttClient.loop();
    }

    dht11mqtt::publishTelemetryIfUpdated();
}

#endif // DHT11_MQTT_H
