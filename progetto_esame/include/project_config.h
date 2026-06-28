#ifndef PROJECT_CONFIG_H
#define PROJECT_CONFIG_H

/**
 * @file project_config.h
 * @brief Shared project configuration macros for ESP32, sensor timing, and MQTT.
 */

/* Pin configuration */
#define DHT_PIN 18U /**< GPIO used by the DHT11 data line. */
#define LED_PIN 2U /**< GPIO used as actuator output (LED). */

/* Sensor configuration */
#define SENSOR_PERIOD_MS 2000U /**< Sensor acquisition period in milliseconds. */

/* Actuator configuration */
#define TEMPERATURE_THRESHOLD_C 30.0F /**< Temperature threshold used to activate the LED actuator. */
#define CONTROL_PERIOD_MS 500U /**< Actuator control period in milliseconds. */

/* Wi-Fi configuration */
#define WIFI_SSID "TP-Link_Novello" /* "SSID_XXX" */ /**< Wi-Fi SSID for station mode connection. */
#define WIFI_PASSWORD "16417595" /* "PASSWORD_XXX" */ /**< Wi-Fi password for station mode connection. */

/* MQTT configuration */
#define MQTT_BROKER "broker.hivemq.com" /**< MQTT broker hostname. */
#define MQTT_PORT 1883U /**< MQTT broker port. */
#define MQTT_CLIENT_ID "esp32-ambient-monitor" /**< Client identifier used by ESP32 on broker connection. */
#define TELEMETRY_TOPIC "sistemi_operativi_mobili/progetto_esame/telemetry" /**< Topic used to publish sensor telemetry. */
#define STATUS_TOPIC "sistemi_operativi_mobili/progetto_esame/status" /**< Topic used to publish connection status messages. */
#define MQTT_PERIOD_MS 1000U /**< MQTT handling period in milliseconds. */

#endif