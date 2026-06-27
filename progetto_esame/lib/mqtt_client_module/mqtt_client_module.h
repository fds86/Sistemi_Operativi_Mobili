#ifndef MQTT_CLIENT_MODULE_H
#define MQTT_CLIENT_MODULE_H

#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "project_config.h"

/**
 * @file mqtt_client_module.h
 * @brief MQTT connectivity and telemetry publish API.
 */

/**
 * @brief Initializes Wi-Fi mode and broker server configuration.
 * @param mqttClient Pointer to the PubSubClient instance.
 */
void MqttClientModule_InitializeNetworkStack(PubSubClient *mqttClient);

/**
 * @brief Starts Wi-Fi connection if not connected and credentials are set.
 */
void MqttClientModule_ConnectWifiIfNeeded();

/**
 * @brief Connects to MQTT broker when Wi-Fi is available.
 * @param mqttClient Pointer to the PubSubClient instance.
 */
void MqttClientModule_ConnectMqttIfNeeded(PubSubClient *mqttClient);

/**
 * @brief Publishes telemetry only when new sensor data is available.
 * @param mqttClient Pointer to the PubSubClient instance.
 * @param isValid True if sensor reading is valid.
 * @param temperatureC Current temperature in Celsius.
 * @param humidityPct Current humidity percentage.
 * @param timestampMs Timestamp associated with current reading.
 * @param lastPublishedTimestampMs Pointer to last published timestamp cache.
 */
void MqttClientModule_PublishTelemetryIfUpdated(PubSubClient *mqttClient,
                                                bool isValid,
                                                float temperatureC,
                                                float humidityPct,
                                                unsigned long timestampMs,
                                                unsigned long *lastPublishedTimestampMs);

#endif