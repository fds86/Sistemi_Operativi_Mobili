#ifndef MQTT_CLIENT_MODULE_H
#define MQTT_CLIENT_MODULE_H

#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <stdio.h>
#include <string.h>
#include "project_config.h"

inline bool HasWifiCredentials()
{
    return (strlen(WIFI_SSID) > 0U);
}

inline bool HasBrokerAddress()
{
    return (strlen(MQTT_BROKER) > 0U);
}

inline void InitializeNetworkStack(PubSubClient *mqttClient)
{
    WiFi.mode(WIFI_STA);
    WiFi.setAutoReconnect(true);
    WiFi.persistent(false);

    mqttClient->setServer(MQTT_BROKER, MQTT_PORT);

    if (false == HasWifiCredentials())
    {
        Serial.println("Set WIFI_SSID and WIFI_PASSWORD in project_config.h");
    }

    if (false == HasBrokerAddress())
    {
        Serial.println("Set MQTT_BROKER in project_config.h");
    }
}

inline void ConnectWifiIfNeeded(unsigned long nowMs)
{
    (void) nowMs;

    if (WL_CONNECTED == WiFi.status())
    {
        return;
    }

    if (false == HasWifiCredentials())
    {
        return;
    }

    Serial.print("Connecting WiFi SSID: ");
    Serial.println(WIFI_SSID);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
}

inline void ConnectMqttIfNeeded(PubSubClient *mqttClient, unsigned long nowMs)
{
    (void) nowMs;

    if (false == HasBrokerAddress())
    {
        return;
    }

    if (true == mqttClient->connected())
    {
        return;
    }

    if (WL_CONNECTED != WiFi.status())
    {
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

inline bool BuildTelemetryPayload(
    bool isValid,
    float temperatureC,
    float humidityPct,
    unsigned long timestampMs,
    bool alarmOn,
    char *payloadBuffer,
    size_t payloadBufferSize)
{
    (void) timestampMs;
    (void) alarmOn;

    int written = 0;

    if (true == isValid)
    {
        written = snprintf(
            payloadBuffer,
            payloadBufferSize,
            "{\"temperatureC\":%.1f,\"humidityPct\":%.1f}",
            temperatureC,
            humidityPct);
    }
    else
    {
        written = snprintf(payloadBuffer, payloadBufferSize, "{\"error\":\"invalid_sensor_data\"}");
    }

    return (written > 0) && (static_cast<size_t>(written) < payloadBufferSize);
}

inline void PublishTelemetryIfUpdated(
    PubSubClient *mqttClient,
    bool isValid,
    float temperatureC,
    float humidityPct,
    unsigned long timestampMs,
    bool alarmOn,
    unsigned long *lastPublishedTimestampMs)
{
    if (false == mqttClient->connected())
    {
        return;
    }

    if (0UL == timestampMs)
    {
        return;
    }

    if (timestampMs == *lastPublishedTimestampMs)
    {
        return;
    }

    char payload_buffer[160] = {0};

    if (false == BuildTelemetryPayload(
        isValid,
        temperatureC,
        humidityPct,
        timestampMs,
        alarmOn,
        payload_buffer,
        sizeof(payload_buffer)))
    {
        Serial.println("Telemetry payload error");
        return;
    }

    bool is_published = mqttClient->publish(TELEMETRY_TOPIC, payload_buffer);

    if (true == is_published)
    {
        *lastPublishedTimestampMs = timestampMs;
        Serial.print("MQTT telemetry -> ");
        Serial.println(payload_buffer);
    }
    else
    {
        Serial.println("MQTT publish failed");
    }
}

#endif