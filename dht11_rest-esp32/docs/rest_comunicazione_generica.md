# Guida Generica: Comunicazione REST con ESP32

Questa guida e generica: mostra come costruire una API REST minima su ESP32 e come consumarla da web/client.

## Obiettivo
Creare un dispositivo che espone endpoint HTTP JSON e che puo essere interrogato da browser, app o script.

## Prerequisiti
- ESP32
- Framework Arduino (o equivalente)
- Librerie Wi-Fi e WebServer

## 1. Definisci il modello dati
Descrizione minima: crea una struttura neutra con stato e payload.

```cpp
typedef struct
{
    bool valid;
    float value1;
    float value2;
    unsigned long version;
} DeviceData;
```

Note:
- `valid` indica se i dati sono utilizzabili.
- `version` (contatore o timestamp interno) aiuta a capire se il dato e aggiornato.

## 2. Avvia rete e server HTTP
Descrizione minima: porta il device online e apri un server sulla porta 80.

```cpp
#include <WiFi.h>
#include <WebServer.h>

WebServer server(80);

void initNetwork()
{
    WiFi.mode(WIFI_AP); // alternativa: WIFI_STA per rete esistente
    WiFi.softAP("ESP32-REST", "password123");
}
```

## 3. Crea endpoint REST principali
Descrizione minima: esponi almeno un endpoint di lettura e uno stato.

```cpp
String toJson(const DeviceData *d)
{
    String payload = "{";
    payload += "\"valid\":";
    payload += d->valid ? "true" : "false";
    payload += ",\"value1\":";
    payload += d->valid ? String(d->value1, 2) : "null";
    payload += ",\"value2\":";
    payload += d->valid ? String(d->value2, 2) : "null";
    payload += ",\"version\":";
    payload += String(d->version);
    payload += "}";
    return payload;
}

void handleData()
{
    server.send(200, "application/json", toJson(&data));
}

void handleHealth()
{
    server.send(200, "application/json", "{\"status\":\"ok\"}");
}

void initRoutes()
{
    server.on("/api/data", HTTP_GET, handleData);
    server.on("/api/health", HTTP_GET, handleHealth);
    server.begin();
}
```

## 4. Aggiorna i dati nel loop
Descrizione minima: aggiorna lo stato periodicamente e servi i client HTTP.

```cpp
DeviceData data = {false, NAN, NAN, 0};
unsigned long lastUpdate = 0;
const unsigned long UPDATE_MS = 2000;

void updateData()
{
    unsigned long now = millis();
    if (now - lastUpdate >= UPDATE_MS)
    {
        lastUpdate = now;

        // Sostituisci con lettura reale da sensore o logica applicativa
        data.value1 = random(180, 300) / 10.0;
        data.value2 = random(300, 800) / 10.0;
        data.valid = true;
        data.version++;
    }
}

void loop()
{
    updateData();
    server.handleClient();
}
```

## 5. API minima consigliata
Descrizione minima: mantieni semplice e coerente.

- `GET /api/health`: stato servizio.
- `GET /api/data`: ultimo snapshot dati.
- `POST /api/config` (opzionale): aggiornamento parametri runtime.

Esempio risposta standard:

```json
{
  "valid": true,
  "value1": 24.7,
  "value2": 58.2,
  "version": 42
}
```

## 6. Consumo da web (frontend)
Descrizione minima: polling periodico e gestione errori.

```html
<script>
let lastVersion = null;
let unchangedCount = 0;

async function refresh() {
  try {
    const res = await fetch('/api/data');
    const data = await res.json();

    if (lastVersion === data.version) unchangedCount++;
    else { unchangedCount = 0; lastVersion = data.version; }

    if (!data.valid) {
      console.log('Dato non valido');
    } else if (unchangedCount >= 2) {
      console.log('Dato valido ma non aggiornato');
    } else {
      console.log('Dato valido e aggiornato');
    }
  } catch (e) {
    console.log('Errore di comunicazione REST');
  }
}

setInterval(refresh, 2000);
refresh();
</script>
```

## 7. Test rapido endpoint
Descrizione minima: verifica API fuori dal browser.

```bash
curl http://192.168.4.1/api/health
curl http://192.168.4.1/api/data
```

## 8. Regole pratiche (best practice minime)
- Usa sempre `application/json` nelle risposte API.
- Se possibile, mantieni un campo `valid` per qualita dato.
- Aggiungi un campo di avanzamento (`version` o timestamp interno) per rilevare stallo.
- Evita `delay()` bloccanti nel loop; usa finestre temporali con `millis()`.
- Gestisci errore rete e timeout lato client.

## 9. Variante architetturale
- Locale: ESP32 come server REST (browser/app leggono direttamente).
- Cloud: ESP32 come client HTTP/MQTT che invia dati a backend centrale.

Scegli locale per prototipi semplici in LAN; scegli cloud per accesso remoto e storico.
