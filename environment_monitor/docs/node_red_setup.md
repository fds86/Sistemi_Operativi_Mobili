# Node-RED Quick Setup (ESP32 MQTT)

## Procedura completa

1. Installa Node.js LTS da [https://nodejs.org/](https://nodejs.org/).
2. Verifica da terminale che Node.js e npm siano disponibili:

   ```powershell
   node -v
   npm -v
   ```

3. Installa Node-RED:

   ```powershell
   npm install -g --unsafe-perm node-red
   ```

4. Carica il firmware sull'ESP32 con `Upload` da PlatformIO o dal tuo ambiente di sviluppo.
5. Avvia Node-RED dal prompt dei comandi partendo dalla root `C:\`:

   ```powershell
   cd C:\
   node-red
   ```

6. Apri l'editor di Node-RED nel browser: `http://localhost:1880`

7. Installa il pacchetto dashboard da Node-RED:

   - Menu -> Manage palette -> Install
   - cerca `node-red-dashboard`
   - premi `Install`

8. Importa il flow del progetto:

   - Menu -> Import
   - seleziona [node_red_flow.json](node_red_flow.json)

9. Fai `Deploy`.
10. Apri la dashboard nel browser: `http://127.0.0.1:1880/ui`

## Verifica rapida

Nel flow il broker e i topic sono gia impostati:

- Broker: `broker.hivemq.com`
- Porta: `1883`
- Telemetry topic: `sistemi_operativi_mobili/+/telemetry`
- Status topic: `sistemi_operativi_mobili/+/status`

Il wildcard `+` rende la dashboard compatibile sia con `environment_monitor` sia con `progetto_esame`, quindi un rename del progetto non rompe piu la visualizzazione.

Se l'ESP32 e acceso, connesso alla rete prevista dal progetto e sta pubblicando correttamente, nella dashboard vedrai:

- Gauge temperatura
- Gauge umidita
- Grafico trend
- Stato MQTT
