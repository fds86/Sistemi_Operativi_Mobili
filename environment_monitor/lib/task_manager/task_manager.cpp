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

/**
 * @brief Shared sensor data exchanged across FreeRTOS tasks.
 */
struct SharedSensorData
{
    bool valid; /**< True when the sampled sensor data is valid. */
    float temperatureC; /**< Last sampled temperature in Celsius. */
    float humidityPct; /**< Last sampled relative humidity percentage. */
    unsigned long timestampMs; /**< Timestamp in milliseconds of the last sample update. */
    bool alarmOn; /**< Actuator state derived from threshold control logic. */
};

static DHTesp dht_sensor;
static WiFiClient wifi_client;
static PubSubClient mqtt_client(wifi_client);

static SharedSensorData shared_data = {false, NAN, NAN, 0UL, false};
static SemaphoreHandle_t shared_data_mutex = nullptr;

/**
 * @brief Copies the latest shared sensor data into a destination buffer.
 * @param destination Output pointer receiving the copied shared data.
 */
static void CopySharedData(SharedSensorData *destination);

/**
 * @brief Updates shared sensor data and alarm state under mutex protection.
 * @param temperatureC Last sampled temperature in Celsius.
 * @param humidityPct Last sampled relative humidity percentage.
 * @param isValid True when the latest sensor sample is valid.
 */
static void UpdateSharedData(float temperatureC, float humidityPct, bool isValid);

/**
 * @brief FreeRTOS task that periodically acquires DHT11 samples.
 * @param[in] Unused FreeRTOS task parameter.
 */
static void SensorTask(void *);

/**
 * @brief FreeRTOS task that drives the actuator according to alarm state.
 * @param[in] Unused FreeRTOS task parameter.
 */
static void ActuatorTask(void *);

/**
 * @brief FreeRTOS task that maintains MQTT connectivity and telemetry publish.
 * @param[in] Unused FreeRTOS task parameter.
 */
static void MqttTask(void *);

/**
 * @brief Initializes peripherals, synchronization primitives, and application tasks.
 */
void TaskManager_SetupTaskManager();

static void CopySharedData(SharedSensorData *destination)
{
    /* Proceed only when the shared-data mutex is acquired. */
    if (pdTRUE == xSemaphoreTake(shared_data_mutex, portMAX_DELAY))
    {
        /* Copy a consistent snapshot while holding the shared mutex. */
        *destination = shared_data;
        xSemaphoreGive(shared_data_mutex);
    }
}

static void UpdateSharedData(float temperatureC, float humidityPct, bool isValid)
{
    /* Update shared state atomically under mutex protection. */
    if (pdTRUE == xSemaphoreTake(shared_data_mutex, portMAX_DELAY))
    {
        shared_data.valid = isValid;
        shared_data.timestampMs = millis();

        if (true == shared_data.valid)
        {
            /* Update values and alarm state only when sensor data is valid. */
            shared_data.temperatureC = temperatureC;
            shared_data.humidityPct = humidityPct;

            /* Update alarm state based on temperature threshold. */
            if (temperatureC >= TEMPERATURE_THRESHOLD_C)
            {
                shared_data.alarmOn = true;
            }
            else
            {
                shared_data.alarmOn = false;
            }
        }
        else
        {
            /* On invalid read, force actuator-safe state. */
            shared_data.alarmOn = false;
        }

        xSemaphoreGive(shared_data_mutex);
    }
}

static void SensorTask(void *)
{
    for (;;)
    {
        float temperature_c = 0.0;
        float humidity_pct = 0.0;
        bool is_valid = false;

        /* Read sensor values. */
        Dht11SensorModule_ReadDht11Sensor(&dht_sensor, &temperature_c, &humidity_pct, &is_valid);

        /* Update shared data with the latest sensor values. */
        UpdateSharedData(temperature_c, humidity_pct, is_valid);

        /* Print full telemetry only for valid sensor samples. */
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
            /* Invalid reads are expected occasionally with DHT11. */
            Serial.println("[SensorTask] Invalid DHT11 reading");
        }

        vTaskDelay(pdMS_TO_TICKS(SENSOR_PERIOD_MS));
    }
}

static void ActuatorTask(void *)
{
    SharedSensorData snapshot_data = {false, NAN, NAN, 0UL, false};

    for (;;)
    {
        CopySharedData(&snapshot_data);

        /* Update actuator state based on alarm flag. */
        if (true == snapshot_data.alarmOn)
        {
            digitalWrite(LED_PIN, HIGH);
        }
        else
        {
            digitalWrite(LED_PIN, LOW);
        }

        vTaskDelay(pdMS_TO_TICKS(CONTROL_PERIOD_MS));
    }
}

static void MqttTask(void *)
{
    SharedSensorData snapshot_data = {false, NAN, NAN, 0UL, false};
    unsigned long last_published_timestamp_ms = 0UL;

    for (;;)
    {
        MqttClientModule_ConnectWifiIfNeeded();
        MqttClientModule_ConnectMqttIfNeeded(&mqtt_client);

        /* Run MQTT loop only when the client session is connected. */
        if (true == mqtt_client.connected())
        {
            /* Keep MQTT internal state machine active while connected. */
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
    /* Stop setup if mutex allocation failed to avoid data races. */
    if (nullptr == shared_data_mutex)
    {
        /* Cannot continue safely without mutual exclusion on shared data. */
        Serial.println("Failed to create sharedData mutex");
        for (;;)
        {
            delay(1000);
        }
    }

    xTaskCreatePinnedToCore(SensorTask, "SensorTask", 4096, nullptr, 1, nullptr, 1);
    xTaskCreatePinnedToCore(ActuatorTask, "ActuatorTask", 2048, nullptr, 1, nullptr, 1);
    xTaskCreatePinnedToCore(MqttTask, "MqttTask", 4096, nullptr, 1, nullptr, 1);

    Serial.println("Task manager ready");
}
