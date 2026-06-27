#include <Arduino.h>
#include <DHTesp.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include "project_config.h"
#include "dht11_sensor_module.h"
#include "mqtt_client_module.h"
#include "task_manager.h"

struct SharedSensorData
{
    bool valid;
    float temperatureC;
    float humidityPct;
    unsigned long timestampMs;
    bool alarmOn;
};

static DHTesp dht_sensor;
static WiFiClient wifi_client;
static PubSubClient mqtt_client(wifi_client);

static SharedSensorData shared_data = {false, NAN, NAN, 0UL, false};
static SemaphoreHandle_t shared_data_mutex = nullptr;

static void CopySharedData(SharedSensorData *destination)
{
    if (pdTRUE == xSemaphoreTake(shared_data_mutex, portMAX_DELAY))
    {
        *destination = shared_data;
        xSemaphoreGive(shared_data_mutex);
    }
}

static void UpdateSharedData(float temperatureC, float humidityPct, bool isValid)
{
    if (pdTRUE == xSemaphoreTake(shared_data_mutex, portMAX_DELAY))
    {
        shared_data.valid = isValid;
        shared_data.timestampMs = millis();

        if (true == isValid)
        {
            shared_data.temperatureC = temperatureC;
            shared_data.humidityPct = humidityPct;
            shared_data.alarmOn = (temperatureC >= TEMPERATURE_THRESHOLD_C);
        }
        else
        {
            shared_data.alarmOn = false;
        }

        xSemaphoreGive(shared_data_mutex);
    }
}

static void SensorTask(void *parameter)
{
    (void) parameter;

    for (;;)
    {
        float temperature_c = NAN;
        float humidity_pct = NAN;
        bool is_valid = false;

        Dht11SensorModule_ReadDht11Sensor(&dht_sensor, &temperature_c, &humidity_pct, &is_valid);

        UpdateSharedData(temperature_c, humidity_pct, is_valid);

        if (true == is_valid)
        {
            Serial.print("[SensorTask] T=");
            Serial.print(temperature_c);
            Serial.print(" C | H=");
            Serial.print(humidity_pct);
            Serial.println(" %");
        }
        else
        {
            Serial.println("[SensorTask] Invalid DHT11 reading");
        }

        vTaskDelay(pdMS_TO_TICKS(SENSOR_PERIOD_MS));
    }
}

static void ControlTask(void *parameter)
{
    (void) parameter;

    SharedSensorData snapshot_data = {false, NAN, NAN, 0UL, false};

    for (;;)
    {
        CopySharedData(&snapshot_data);
        digitalWrite(LED_PIN, snapshot_data.alarmOn ? HIGH : LOW);

        vTaskDelay(pdMS_TO_TICKS(CONTROL_PERIOD_MS));
    }
}

static void MqttTask(void *parameter)
{
    (void) parameter;

    SharedSensorData snapshot_data = {false, NAN, NAN, 0UL, false};
    unsigned long last_published_timestamp_ms = 0UL;

    for (;;)
    {
        MqttClientModule_ConnectWifiIfNeeded();
        MqttClientModule_ConnectMqttIfNeeded(&mqtt_client);

        if (true == mqtt_client.connected())
        {
            mqtt_client.loop();
        }

        CopySharedData(&snapshot_data);

        MqttClientModule_PublishTelemetryIfUpdated(&mqtt_client,
                                                   snapshot_data.valid,
                                                   snapshot_data.temperatureC,
                                                   snapshot_data.humidityPct,
                                                   snapshot_data.timestampMs,
                                                   &last_published_timestamp_ms);

        vTaskDelay(pdMS_TO_TICKS(MQTT_PERIOD_MS));
    }
}

void TaskManager_SetupTaskManager()
{
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);

    Dht11SensorModule_InitializeDht11Sensor(&dht_sensor);
    MqttClientModule_InitializeNetworkStack(&mqtt_client);

    shared_data_mutex = xSemaphoreCreateMutex();
    if (nullptr == shared_data_mutex)
    {
        Serial.println("Failed to create sharedData mutex");
        for (;;)
        {
            delay(1000);
        }
    }

    xTaskCreatePinnedToCore(SensorTask, "SensorTask", 4096, nullptr, 1, nullptr, 1);
    xTaskCreatePinnedToCore(ControlTask, "ControlTask", 2048, nullptr, 1, nullptr, 1);
    xTaskCreatePinnedToCore(MqttTask, "MqttTask", 4096, nullptr, 1, nullptr, 1);

    Serial.println("Task manager ready");
}
