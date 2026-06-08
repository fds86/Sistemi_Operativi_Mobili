# Lettura Dati MQTT via Web - Guida Passo Passo

## Obiettivo
Visualizzare i dati pubblicati da ESP32 su broker MQTT direttamente da browser.

## Prerequisiti
- ESP32 acceso e gia configurato con:
  - broker MQTT corretto
  - topic telemetria e status corretti
- firmware caricato con successo
- accesso a Internet dal browser

## 1) Verifica configurazione ESP32
Nel file `include/dht11_mqtt.h` verifica:
- broker: `DHT11_MQTT_BROKER`
- topic dati: `DHT11_MQTT_TELEMETRY_TOPIC`
- topic stato: `DHT11_MQTT_STATUS_TOPIC`

Esempio tipico:
- broker: `test.mosquitto.org`
- telemetria: `sistemi_operativi_mobili/dht11/telemetry`
- status: `sistemi_operativi_mobili/dht11/status`

## 2) Apri un client web MQTT
Apri:
- https://www.hivemq.com/demos/websocket-client/

Nota: HiveMQ Web Client puo connettersi a broker esterni, incluso Mosquitto.

## 3) Configura la connessione WebSocket
Inserisci questi valori:
- Host: `test.mosquitto.org`
- Porta: `8081`
- SSL/TLS: ON
- Path: `/mqtt`
- Client ID: valore univoco, ad esempio `web-franc-dht11-01`
- Username/Password: vuoti (se broker pubblico senza auth)

Se non funziona:
- prova porta `8080` con SSL/TLS OFF
- lascia il path `/mqtt`

## 4) Esegui la connessione
Clicca Connect.

Se ok, il client mostra stato connesso.

## 5) Sottoscrivi i topic
Aggiungi Subscribe ai topic:
- `sistemi_operativi_mobili/dht11/telemetry`
- `sistemi_operativi_mobili/dht11/status`

## 6) Interpreta i messaggi ricevuti
Su topic status puoi ricevere:
- `online`: ESP32 connesso al broker
- `offline`: disconnessione inattesa (LWT)

Su topic telemetria ricevi JSON tipo:

```json
{"valid":true,"temperatureC":24.3,"humidityPct":56.0,"timestampMs":123456}
```

Campi:
- `valid`: true se campione sensore valido
- `temperatureC`: temperatura in gradi C
- `humidityPct`: umidita relativa in percentuale
- `timestampMs`: tempo interno ESP32 (millis)

## 7) Controlli rapidi in caso di problemi
1. Apri monitor seriale di PlatformIO.
2. Verifica messaggi:
   - `Connecting WiFi SSID: ...`
   - `MQTT connected`
   - `MQTT telemetry -> ...`
3. Controlla coerenza broker/topic tra ESP32 e client web.
4. Assicurati che il `Client ID` del browser sia diverso da quello ESP32.

## 8) Buone pratiche
- usa topic univoci, per esempio:
  - `sistemi_operativi_mobili/franc/dht11/telemetry`
  - `sistemi_operativi_mobili/franc/dht11/status`
- evita di pubblicare credenziali Wi-Fi su repository pubblici
- per produzione usa broker con TLS e autenticazione

## 9) Differenza pratica REST vs MQTT
- REST: browser legge direttamente da ESP32 (ESP32 server HTTP)
- MQTT: browser legge dal broker; ESP32 pubblica, browser sottoscrive

Quindi con MQTT il browser non deve raggiungere direttamente l'IP di ESP32, ma deve raggiungere il broker.
