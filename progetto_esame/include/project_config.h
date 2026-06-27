#ifndef PROJECT_CONFIG_H
#define PROJECT_CONFIG_H

#define DHT_PIN 18U
#define LED_PIN 2U

#define SENSOR_PERIOD_MS 2000U
#define CONTROL_PERIOD_MS 500U
#define MQTT_PERIOD_MS 1000U

#define WIFI_RECONNECT_PERIOD_MS 5000U
#define MQTT_RECONNECT_PERIOD_MS 3000U

#define TEMPERATURE_THRESHOLD_C 30.0F

#define WIFI_SSID "SSID_XXX"
#define WIFI_PASSWORD "PASSWORD_XXX"

#define MQTT_BROKER "broker.hivemq.com"
#define MQTT_PORT 1883U
#define MQTT_CLIENT_ID "esp32-ambient-monitor"
#define TELEMETRY_TOPIC "sistemi_operativi_mobili/progetto_esame/telemetry"
#define STATUS_TOPIC "sistemi_operativi_mobili/progetto_esame/status"

#endif