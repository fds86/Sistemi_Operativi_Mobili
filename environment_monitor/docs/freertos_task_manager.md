# FreeRTOS nel modulo task_manager

## Obiettivo

Il file [lib/task_manager/task_manager.cpp](../lib/task_manager/task_manager.cpp) centralizza la parte concorrente del progetto `environment_monitor`.
FreeRTOS viene usato per separare tre responsabilita:

1. acquisizione periodica del sensore DHT11
2. aggiornamento dell'attuatore LED
3. gestione della connettivita MQTT e pubblicazione telemetria

In questo modo il firmware evita un unico `loop()` monolitico e mantiene ogni attivita con il proprio periodo di esecuzione.

## Modello di esecuzione

La funzione pubblica `TaskManager_SetupTaskManager()` inizializza i moduli condivisi e crea tre task FreeRTOS:

1. `SensorTask`
2. `ActuatorTask`
3. `MqttTask`

I tre task sono creati con `xTaskCreatePinnedToCore(...)` e vengono fissati sul core `1`.
Ogni task esegue un ciclo infinito `for (;;)` e rilascia periodicamente la CPU tramite `vTaskDelay(...)`.

## Dati condivisi

I task condividono una singola struttura `SharedSensorData`, che contiene:

1. validita dell'ultima misura
2. temperatura
3. umidita
4. timestamp dell'ultimo aggiornamento
5. stato logico dell'allarme/attuatore

L'accesso concorrente a questa struttura e protetto da un mutex FreeRTOS (`SemaphoreHandle_t`).

## Funzioni FreeRTOS usate

### `xTaskCreatePinnedToCore(...)`

Crea il task FreeRTOS e lo associa a un core specifico dell'ESP32.
Nel progetto viene usato per avviare separatamente acquisizione sensore, attuatore e MQTT.

Parametri rilevanti usati qui:

1. funzione task da eseguire
2. nome testuale del task
3. dimensione stack
4. parametro iniziale (`nullptr` in questo progetto)
5. priorita (`1` in questo progetto)
6. handle del task (`nullptr`, non salvato)
7. core target (`1`)

### `vTaskDelay(pdMS_TO_TICKS(...))`

Sospende il task corrente per un intervallo espresso in tick di FreeRTOS.
Nel progetto viene sempre combinata con `pdMS_TO_TICKS(...)` per partire da periodi in millisecondi definiti in [include/project_config.h](../include/project_config.h).

Questo approccio evita busy waiting e lascia tempo CPU agli altri task.

### `pdMS_TO_TICKS(...)`

Converte un valore in millisecondi nel numero di tick richiesto dal kernel FreeRTOS.
Serve per rendere i periodi di campionamento e controllo leggibili e indipendenti dal tick rate interno.

### `xSemaphoreCreateMutex()`

Crea il mutex usato per proteggere `SharedSensorData`.
Se la creazione fallisce, il progetto entra in un loop di errore per evitare accessi concorrenti non protetti.

### `xSemaphoreTake(...)`

Acquisisce il mutex prima di leggere o aggiornare i dati condivisi.
Nel progetto viene usato con `portMAX_DELAY`, quindi il task attende finche il mutex non e disponibile.

### `xSemaphoreGive(...)`

Rilascia il mutex dopo avere completato la sezione critica.
Nel modulo viene chiamato sia dopo la copia dello snapshot sia dopo l'aggiornamento dei dati condivisi.

### `portMAX_DELAY`

Costante usata come timeout massimo nelle attese su mutex.
Qui indica che il task puo aspettare indefinitamente il rilascio del lock.

### `pdTRUE`

Valore di ritorno usato per verificare che l'acquisizione del mutex sia andata a buon fine.

## Task del progetto

### `SensorTask`

Responsabilita:

1. leggere periodicamente il DHT11
2. validare i dati ricevuti dal modulo sensore
3. aggiornare lo snapshot condiviso
4. scrivere log seriali di debug

Periodo:

1. `SENSOR_PERIOD_MS`

Flusso:

1. legge temperatura e umidita da `Dht11SensorModule_ReadDht11Sensor(...)`
2. aggiorna `SharedSensorData` tramite `UpdateSharedData(...)`
3. aspetta il periodo successivo con `vTaskDelay(...)`

### `ActuatorTask`

Responsabilita:

1. leggere uno snapshot coerente dei dati condivisi
2. applicare lo stato dell'allarme al LED su `LED_PIN`

Periodo:

1. `CONTROL_PERIOD_MS`

Flusso:

1. copia i dati condivisi con `CopySharedData(...)`
2. se `alarmOn` e vero accende il LED
3. altrimenti spegne il LED
4. aspetta il periodo successivo con `vTaskDelay(...)`

### `MqttTask`

Responsabilita:

1. mantenere la connessione Wi-Fi
2. mantenere la connessione MQTT
3. eseguire `mqtt_client.loop()` quando connesso
4. pubblicare telemetria solo quando esiste un nuovo campione

Periodo:

1. `MQTT_PERIOD_MS`

Flusso:

1. prova la riconnessione Wi-Fi se necessaria
2. prova la riconnessione MQTT se necessaria
3. esegue la loop MQTT quando la sessione e attiva
4. copia i dati condivisi
5. pubblica i dati tramite `MqttClientModule_PublishTelemetryIfUpdated(...)`

## Perche serve il mutex

`SensorTask` aggiorna i dati mentre `ActuatorTask` e `MqttTask` li leggono.
Senza mutex, un task potrebbe leggere una struttura parzialmente aggiornata, ad esempio con timestamp nuovo ma umidita ancora vecchia.

Il pattern usato nel progetto e:

1. lock del mutex
2. copia o aggiornamento dello snapshot condiviso
3. unlock del mutex

Questo riduce il tempo passato in sezione critica e mantiene il codice semplice da verificare.

## Nota sul `loop()` Arduino

Con questa architettura la logica applicativa non vive nel classico `loop()` Arduino.
Il comportamento periodico e gestito dai tre task FreeRTOS, che offrono una separazione piu chiara delle responsabilita rispetto a un super-loop unico.
