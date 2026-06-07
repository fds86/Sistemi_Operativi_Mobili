#ifndef DHT11_REST_H
#define DHT11_REST_H

#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include "dht11_utils.h"

#define DHT11_REST_AP_SSID "ESP32-DHT11"
#define DHT11_REST_AP_PASSWORD "esp32dht11"

namespace dht11rest
{
    static WebServer server(80);
    static Dht11Data *dataRef = nullptr;

    inline String makeJsonPayload(const Dht11Data *data)
    {
        String payload = "{";
        payload += "\"valid\":";
        payload += (true == data->isDataValid) ? "true" : "false";
        payload += ",\"temperatureC\":";
        payload += true == data->isDataValid ? String(data->temperature_C, 1) : "null";
        payload += ",\"humidityPct\":";
        payload += true == data->isDataValid ? String(data->humidity_Pct, 1) : "null";
        payload += ",\"timestampMs\":";
        payload += String(data->timestamp_Ms);
        payload += "}";
        return payload;
    }

    inline String makeIndexPage()
    {
        String page;
        page += "<!doctype html><html lang='en'><head><meta charset='utf-8'>";
        page += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
        page += "<title>DHT11 REST Monitor</title>";
        page += "<style>body{margin:0;font-family:Verdana,sans-serif;background:#f4efe7;color:#1f2933;}";
        page += ".wrap{min-height:100vh;display:grid;place-items:center;padding:24px;}";
        page += ".card{width:min(440px,100%);background:#fffdf8;border:1px solid #e7dccb;border-radius:20px;";
        page += "padding:24px;box-shadow:0 18px 50px rgba(92,74,54,.12);}";
        page += "h1{margin:0 0 8px;font-size:28px;}p{margin:0 0 20px;color:#52606d;}";
        page += ".grid{display:grid;grid-template-columns:repeat(2,1fr);gap:12px;}";
        page += ".tile{background:#f7f1e8;border-radius:16px;padding:16px;}";
        page += ".label{font-size:12px;text-transform:uppercase;letter-spacing:.08em;color:#7b8794;}";
        page += ".value{margin-top:6px;font-size:30px;font-weight:700;}";
        page += ".value.small{font-size:22px;}";
        page += ".status{margin-top:16px;font-size:14px;color:#52606d;}";
        page += "</style></head><body><div class='wrap'><section class='card'>";
        page += "<h1>DHT11 Sensor</h1><p>Dati letti via REST dall'ESP32.</p><div class='grid'>";
        page += "<div class='tile'><div class='label'>Temperature</div><div class='value' id='temp'>--.- C</div></div>";
        page += "<div class='tile'><div class='label'>Humidity</div><div class='value' id='hum'>--.- %</div></div>";
        page += "</div>";
        page += "<div class='status' id='status'>Waiting for data...</div></section></div><script>";
        page += "let lastTimestampMs=null;let unchangedCount=0;";
        page += "async function refresh(){const res=await fetch('/api/data');const data=await res.json();";
        page += "const t=document.getElementById('temp');const h=document.getElementById('hum');";
        page += "const s=document.getElementById('status');if(lastTimestampMs===data.timestampMs){unchangedCount++;}";
        page += "else{unchangedCount=0;lastTimestampMs=data.timestampMs;}if(data.valid){t.textContent=data.temperatureC.toFixed(1)+' C';";
        page += "h.textContent=data.humidityPct.toFixed(1)+' %';s.textContent=unchangedCount>=2?";
        page += "'Dato valido ma non aggiornato':'Dato valido e aggiornato regolarmente';}";
        page += "else{t.textContent='n/a';h.textContent='n/a';s.textContent='Lettura non valida';}}";
        page += "refresh();setInterval(refresh,2000);</script></body></html>";
        return page;
    }

    inline void handleIndex()
    {
        server.send(200, "text/html", makeIndexPage());
    }

    inline void handleApiDht11()
    {
        if (nullptr == dataRef)
        {
            server.send(500, "application/json", "{\"error\":\"sensor not configured\"}");
            return;
        }

        server.send(200, "application/json", makeJsonPayload(dataRef));
    }
}

inline void initDht11Rest(Dht11Data *data)
{
    dht11rest::dataRef = data;

    WiFi.mode(WIFI_AP);
    WiFi.softAP(DHT11_REST_AP_SSID, DHT11_REST_AP_PASSWORD);

    dht11rest::server.on("/", dht11rest::handleIndex);
    dht11rest::server.on("/api/dht11", dht11rest::handleApiDht11);
    dht11rest::server.on("/api/data", dht11rest::handleApiDht11);
    dht11rest::server.begin();

    Serial.print("WiFi AP ready. SSID: ");
    Serial.println(DHT11_REST_AP_SSID);
    Serial.print("Open browser at: http://");
    Serial.println(WiFi.softAPIP());
}

inline void handleDht11Rest()
{
    dht11rest::server.handleClient();
}

#endif // DHT11_REST_H