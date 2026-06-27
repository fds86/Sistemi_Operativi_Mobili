#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <stdio.h>
#include <string.h>
#include "mqtt_client_module.h"

static bool MqttClientModule_HasWifiCredentials()
{
    return (strlen(WIFI_SSID) > 0U);
}

static bool MqttClientModule_HasBrokerAddress()
{
    return (strlen(MQTT_BROKER) > 0U);
}

static bool MqttClientModule_BuildTelemetryPayload(bool isValid,
                                                   float temperatureC,
                                                   float humidityPct,
                                                   char *payloadBuffer,
                                                   size_t payloadBufferSize)
{
    bool is_payload_buffer_valid = true;
    int written_chars = 0;

    if (true == isValid)
    {
        written_chars = snprintf(payloadBuffer,
                                 payloadBufferSize,
                                 "{\"temperatureC\":%.1f,\"humidityPct\":%.1f}",
                                 temperatureC,
                                 humidityPct);
    }
    else
    {
        written_chars = snprintf(payloadBuffer,
                                 payloadBufferSize,
                                 "{\"error\":\"invalid_sensor_data\"}");
    }

    if (written_chars <= 0)
    {
        /* snprintf failed or produced an empty payload. */
        is_payload_buffer_valid = false;
        return false;
    }

    if ((size_t)written_chars >= payloadBufferSize)
    {
        /* Payload would be truncated; reject publish attempt. */
        is_payload_buffer_valid = false;
        return false;
    }

    return is_payload_buffer_valid;
}

void MqttClientModule_InitializeNetworkStack(PubSubClient *mqttClient)
{
    bool has_wifi_credentials = MqttClientModule_HasWifiCredentials();
    bool has_broker_address = MqttClientModule_HasBrokerAddress();

    WiFi.mode(WIFI_STA);
    WiFi.setAutoReconnect(true);
    WiFi.persistent(false);

    mqttClient->setServer(MQTT_BROKER, MQTT_PORT);

    if (false == has_wifi_credentials)
    {
        /* Keep running because credentials can be configured later. */
        Serial.println("Set WIFI_SSID and WIFI_PASSWORD in project_config.h");
    }

    if (false == has_broker_address)
    {
        /* Keep running because broker can be configured later. */
        Serial.println("Set MQTT_BROKER in project_config.h");
    }
}

void MqttClientModule_ConnectWifiIfNeeded()
{
    uint8_t wifi_status = WiFi.status();
    bool has_wifi_credentials = MqttClientModule_HasWifiCredentials();

    if (WL_CONNECTED == (wl_status_t)wifi_status)
    {
        /* Wi-Fi is already connected. */
        return;
    }

    if (false == has_wifi_credentials)
    {
        /* Skip connection attempt without credentials. */
        return;
    }

    Serial.print("Connecting WiFi SSID: ");
    Serial.println(WIFI_SSID);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
}

void MqttClientModule_ConnectMqttIfNeeded(PubSubClient *mqttClient)
{
    uint8_t wifi_status = WiFi.status();
    bool has_broker_address = MqttClientModule_HasBrokerAddress();

    if (false == has_broker_address)
    {
        /* Skip MQTT connection until broker is configured. */
        return;
    }

    if (true == mqttClient->connected())
    {
        /* MQTT session is already active. */
        return;
    }

    if (WL_CONNECTED != (wl_status_t)wifi_status)
    {
        /* MQTT connection requires active Wi-Fi. */
        return;
    }

    Serial.print("Connecting MQTT broker: ");
    Serial.print(MQTT_BROKER);
    Serial.print(":");
    Serial.println(MQTT_PORT);

    bool is_connected = mqttClient->connect(MQTT_CLIENT_ID);

    if (true == is_connected)
    {
        Serial.println("MQTT connected");
        mqttClient->publish(STATUS_TOPIC, "online");
    }
    else
    {
        Serial.print("MQTT connect failed. rc=");
        Serial.println(mqttClient->state());
    }
}

void MqttClientModule_PublishTelemetryIfUpdated(PubSubClient *mqttClient,
                                                bool isValid,
                                                float temperatureC,
                                                float humidityPct,
                                                unsigned long timestampMs,
                                                unsigned long *lastPublishedTimestampMs)
{
    bool build_payload_success = false;
    bool is_published = false;
    char payload_buffer[160] = {0};

    if (false == mqttClient->connected())
    {
        /* Skip publish without active broker connection. */
        return;
    }

    if (0UL == timestampMs)
    {
        /* Ignore uninitialized sensor timestamp. */
        return;
    }

    if (timestampMs == *lastPublishedTimestampMs)
    {
        /* Publish only when a newer sample is available. */
        return;
    }

    build_payload_success = MqttClientModule_BuildTelemetryPayload(isValid,
                                                                   temperatureC,
                                                                   humidityPct,
                                                                   payload_buffer,
                                                                   sizeof(payload_buffer));

    if (false == build_payload_success)
    {
        /* Skip publish when JSON payload is invalid. */
        Serial.println("Telemetry payload error");
        return;
    }

    is_published = mqttClient->publish(TELEMETRY_TOPIC, payload_buffer);

    if (true == is_published)
    {
        /* Update cache to avoid republishing the same sample. */
        *lastPublishedTimestampMs = timestampMs;
        Serial.print("MQTT telemetry -> ");
        Serial.println(payload_buffer);
    }
    else
    {
        Serial.println("MQTT publish failed");
    }
}
