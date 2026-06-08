# Implementazione Libreria MQTT (ESP32 + DHT11)

## Obiettivo
Questa implementazione permette a ESP32 di:
- leggere periodicamente i dati del sensore DHT11
- connettersi a una rete Wi-Fi in modalita STA
- connettersi a un broker MQTT
- pubblicare telemetria in formato JSON
- pubblicare stato online/offline con meccanismo LWT

Il codice principale della libreria e definito nel file `include/dht11_mqtt.h`.

## Dipendenze
Nel progetto sono usate queste librerie:
- DHT sensor library for ESPx (lettura sensore)
- PubSubClient (client MQTT)
- WiFi.h (stack di rete ESP32)

## Parametri di configurazione
La libreria usa macro per la configurazione:
- `DHT11_WIFI_SSID`
- `DHT11_WIFI_PASSWORD`
- `DHT11_MQTT_BROKER`
- `DHT11_MQTT_PORT`
- `DHT11_MQTT_CLIENT_ID`
- `DHT11_MQTT_TELEMETRY_TOPIC`
- `DHT11_MQTT_STATUS_TOPIC`
- `WIFI_RECONNECT_PERIOD_MS`
- `MQTT_RECONNECT_PERIOD_MS`

## Struttura interna
Nel namespace `dht11mqtt` sono definiti:
- `WiFiClient wifiClient`: trasporto TCP
- `PubSubClient mqttClient`: client MQTT
- `Dht11Data *dataRef`: puntatore ai dati aggiornati dal layer sensore
- `lastWifiAttemptMs`: timestamp ultimo tentativo Wi-Fi
- `lastMqttAttemptMs`: timestamp ultimo tentativo MQTT
- `lastPublishedTimestampMs`: timestamp ultimo payload pubblicato

Queste variabili evitano ritentativi continui e pubblicazioni duplicate.

## Funzioni principali

### 1) `initDht11Mqtt(Dht11Data *data)`
Responsabilita:
- salva il puntatore a `Dht11Data`
- imposta ESP32 in modalita `WIFI_STA`
- abilita `WiFi.setAutoReconnect(true)`
- imposta il broker nel client MQTT (`setServer`)
- avvia un primo tentativo di connessione Wi-Fi

Questa funzione va chiamata una sola volta in `setup()`.

### 2) `handleDht11Mqtt()`
Responsabilita:
- gestire riconnessione Wi-Fi non bloccante
- gestire riconnessione MQTT non bloccante
- eseguire `mqttClient.loop()` quando connesso
- pubblicare telemetria solo se il campione e nuovo

Questa funzione va chiamata a ogni iterazione di `loop()`.

### 3) `connectWifiIfNeeded()`
Logica:
- se Wi-Fi gia connesso: ritorna
- se credenziali mancanti: ritorna
- se non e ancora trascorso `WIFI_RECONNECT_PERIOD_MS`: ritorna
- altrimenti prova `WiFi.begin(...)`

Approccio non bloccante: niente `delay` o wait attivi.

### 4) `connectMqttIfNeeded()`
Logica:
- procede solo se Wi-Fi e connesso
- limita i tentativi con `MQTT_RECONNECT_PERIOD_MS`
- usa `mqttClient.connect(...)` con Last Will:
  - topic: `DHT11_MQTT_STATUS_TOPIC`
  - payload LWT: `offline`
  - retained: true
- su connessione riuscita pubblica `online` retained

Risultato: i client remoti vedono sempre l'ultimo stato dispositivo.

### 5) `makeJsonPayload(...)`
Costruisce payload JSON con due casi:
- dato valido:
  - `{"valid":true,"temperatureC":...,"humidityPct":...,"timestampMs":...}`
- dato non valido:
  - `{"valid":false,"temperatureC":null,"humidityPct":null,"timestampMs":...}`

Usa `snprintf` per evitare overflow e restituisce `false` se il buffer non basta.

### 6) `publishTelemetryIfUpdated()`
Regole di pubblicazione:
- pubblica solo se MQTT connesso
- ignora dati con timestamp 0
- evita duplicati confrontando `timestamp_Ms` con `lastPublishedTimestampMs`
- pubblica su `DHT11_MQTT_TELEMETRY_TOPIC`

Questo riduce traffico e rumore sul broker.

## Flusso completo del dato
1. `readDht11SensorWithTimestamp(...)` aggiorna `Dht11Data`.
2. `handleDht11Mqtt()` gestisce rete e broker.
3. Se il campione e nuovo, viene serializzato in JSON.
4. JSON pubblicato su topic telemetria.
5. I subscriber (web app, script, dashboard) ricevono i dati.

## Vantaggi rispetto a REST locale
- ESP32 non deve esporre una pagina web locale
- i client leggono dati da broker anche fuori LAN (se broker raggiungibile)
- architettura publish/subscribe scalabile a piu client

## Limiti e attenzione
- senza TLS su porta 1883 i dati non sono cifrati
- broker pubblico: topic condivisi, possibile rumore da altri client
- `DHT11_MQTT_CLIENT_ID` deve essere univoco per evitare disconnessioni tra client con stesso ID

## Miglioramenti consigliati
- usare topic univoci per utente/gruppo
- usare broker con autenticazione e TLS
- spostare credenziali Wi-Fi in file locale escluso da git
- aggiungere subscribe comandi (es. modifica intervallo campionamento)
