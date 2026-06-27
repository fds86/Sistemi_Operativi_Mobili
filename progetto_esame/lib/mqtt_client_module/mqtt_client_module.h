#ifndef MQTT_CLIENT_MODULE_H
#define MQTT_CLIENT_MODULE_H

#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "project_config.h"

void MqttClientModule_InitializeNetworkStack(PubSubClient *mqttClient);

void MqttClientModule_ConnectWifiIfNeeded();

void MqttClientModule_ConnectMqttIfNeeded(PubSubClient *mqttClient);

void MqttClientModule_PublishTelemetryIfUpdated(PubSubClient *mqttClient,
                                                bool isValid,
                                                float temperatureC,
                                                float humidityPct,
                                                unsigned long timestampMs,
                                                unsigned long *lastPublishedTimestampMs);

#endif