#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <stdio.h>
#include <string.h>
#include "mqtt_client_module.h"

static unsigned long wifi_last_connect_attempt_ms = 0UL;
static unsigned long mqtt_last_connect_attempt_ms = 0UL;

static bool HasWifiCredentials()
{
    return (strlen(WIFI_SSID) > 0U);
}

static bool HasBrokerAddress()
{
    return (strlen(MQTT_BROKER) > 0U);
}

static bool PublishToTopic(PubSubClient *mqttClient,
                           const char *topic,
                           const char *payload)
{
    bool is_published = mqttClient->publish(topic, payload);

    /* Log broker publish failures to ease runtime diagnostics. */
    if (false == is_published)
    {
        Serial.print("MQTT publish failed: ");
        Serial.println(topic);
    }

    return is_published;
}

static bool BuildTelemetryPayload(bool isValid,
                                  float temperatureC,
                                  float humidityPct,
                                  char *payloadBuffer,
                                  size_t payloadBufferSize)
{
    int written_chars = 0;

    /* Build either telemetry JSON or an explicit sensor-error payload. */
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
        return false;
    }

    if ((size_t)written_chars >= payloadBufferSize)
    {
        /* Payload would be truncated; reject publish attempt. */
        return false;
    }

    return true;
}

void MqttClientModule_InitializeNetworkStack(PubSubClient *mqttClient)
{
    bool has_wifi_credentials = HasWifiCredentials();
    bool has_broker_address = HasBrokerAddress();

    WiFi.mode(WIFI_STA);
    WiFi.setAutoReconnect(true);
    WiFi.persistent(false);

    /* Configure MQTT broker address and port. */
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
    const unsigned long wifi_connect_retry_interval_ms = 5000UL;
    unsigned long now_ms = millis();
    uint8_t wifi_status = WiFi.status();
    bool has_wifi_credentials = HasWifiCredentials();

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

    if ((now_ms - wifi_last_connect_attempt_ms) < wifi_connect_retry_interval_ms)
    {
        /* Avoid repeated WiFi.begin calls while association is in progress. */
        return;
    }

    wifi_last_connect_attempt_ms = now_ms;

    Serial.print("Connecting WiFi SSID: ");
    Serial.println(WIFI_SSID);

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
}

void MqttClientModule_ConnectMqttIfNeeded(PubSubClient *mqttClient)
{
    const unsigned long mqtt_connect_retry_interval_ms = 3000UL;
    unsigned long now_ms = millis();
    uint8_t wifi_status = WiFi.status();
    bool has_broker_address = HasBrokerAddress();

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

    if ((now_ms - mqtt_last_connect_attempt_ms) < mqtt_connect_retry_interval_ms)
    {
        /* Avoid reconnect spam while broker is unavailable. */
        return;
    }

    mqtt_last_connect_attempt_ms = now_ms;

    Serial.print("Connecting MQTT broker: ");
    Serial.print(MQTT_BROKER);
    Serial.print(":");
    Serial.println(MQTT_PORT);

    bool is_connected = mqttClient->connect(MQTT_CLIENT_ID);

    /* Publish online status only after a successful MQTT connect. */
    if (true == is_connected)
    {
        Serial.println("MQTT connected");
        PublishToTopic(mqttClient, STATUS_TOPIC, "online");
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

    /* Build JSON payload for telemetry. */
    build_payload_success = BuildTelemetryPayload(isValid,
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

    /* Publish telemetry to MQTT broker. */
    is_published = PublishToTopic(mqttClient, TELEMETRY_TOPIC, payload_buffer);

    /* Update publish watermark only when broker accept succeeds. */
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
