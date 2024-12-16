#include "globals.h"

#include <ArduinoOTA.h>
#include <WiFiManager.h>
#include <ESPAsyncWebServer.h>
#include <ESPmDNS.h>

AsyncWebServer server(HTTP_PORT);
AsyncWebSocket ws("/ws");

uint8_t buffer[GRAPHICS_BUFFER_MAX];
int bufferSize = 0;

WiFiManager wm;

void appendBuffer(uint8_t *data, size_t len, size_t index)
{
    if (index == 0)
    {
        bufferSize = 0;
    }

    if (bufferSize + len > GRAPHICS_BUFFER_MAX)
    {
        Serial.println("Buffer overflow detected!");
        return;
    }

    for (size_t i = 0; i < len; i++)
    {
        buffer[bufferSize] = data[i];
        bufferSize++;
    }
}

void networkingSetup()
{
    // Wifi
    {
        // wm.resetSettings();
        wm.setClass("invert");
        wm.setWiFiAutoReconnect(true);

        bool result = wm.autoConnect(HOSTNAME);

        if (!result)
        {
            Serial.println("Failed to connect!");
            // ESP.restart();
        }
        else
        {
            Serial.println("Connected!");
        }
    }

    // MDNS
    {
        if (!MDNS.begin(HOSTNAME))
        {
            Serial.println("Error setting up MDNS responder!");
        }
        else
        {
            Serial.println("MDNS enabled.");
            MDNS.enableArduino(OTA_PORT, false);
            MDNS.addService("http", "tcp", HTTP_PORT);
        }
    }

    // Default headers.
    {
        DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
        DefaultHeaders::Instance().addHeader("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
        DefaultHeaders::Instance().addHeader("Access-Control-Allow-Headers", "Content-Type, X-Requested-With");
    }

    // Server routes.
    {
        server.on(
            "/status", HTTP_GET,
            [](AsyncWebServerRequest *request)
            {
                String jsonString = String() +
                                    "{" +
                                    "\"ssid\":\"" + WiFi.SSID() + "\"," +
                                    "\"ip\":\"" + WiFi.localIP().toString() + "\"," +
                                    "\"rsi\":" + WiFi.RSSI() + "," +
                                    "\"rows\":" + ROWS + "," +
                                    "\"columns\":" + COLUMNS + "," +
                                    "\"freeMemory\":" + ESP.getFreeHeap() +
                                    "}";

                request->send(200, "application/json", jsonString);
            });

        server.on(
            "/buffer", HTTP_POST,
            [](AsyncWebServerRequest *request) {},
            NULL,
            [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
            {
                Serial.printf("Length: %u/%u \n", len, total);
                appendBuffer(data, len, index);

                if (index + len == total)
                {
                    displaySetBuffer(buffer, bufferSize);
                    request->send(200, "text/plain", "OK");
                }
            });

        server.onNotFound(
            [](AsyncWebServerRequest *request)
            {
                Serial.println("URI: " + request->url());
                Serial.println("Method: " + request->method());

                if (request->method() == HTTP_OPTIONS)
                {
                    Serial.println("Options!");
                    request->send(200, "text/plain", "OK");
                }
                else
                {
                    Serial.println("404?");
                    request->send(200, "text/plain", "OK?");
                }
            });
    }

    // Websocket handler.
    {
        ws.onEvent(
            [](AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len)
            {
                if (type == WS_EVT_CONNECT)
                {
                    Serial.println("Client connected.");
                }
                else if (type == WS_EVT_DISCONNECT)
                {
                    Serial.println("Client disconnected.");
                }
                else if (type == WS_EVT_DATA)
                {
                    AwsFrameInfo *info = (AwsFrameInfo *)arg;

                    appendBuffer(data, len, info->index);

                    if (info->final)
                        displaySetBuffer(buffer, bufferSize);
                }
            });
        server.addHandler(&ws);
    }

    // Static files.
    {

        server.serveStatic("/", LittleFS, "/").setDefaultFile("index.html").setCacheControl("max-age=31536000");
    }

    // Server begin.
    Serial.println("Starting server...");
    server.begin();

    // OTA updates.
    {
        ArduinoOTA.setHostname(HOSTNAME);
        ArduinoOTA.setPort(OTA_PORT);
        ArduinoOTA.setMdnsEnabled(false);

        ArduinoOTA
            .onStart([]()
                     { Serial.println("Starting update..."); })
            .onEnd([]()
                   { Serial.println("\nEnd"); })
            .onProgress([](unsigned int progress, unsigned int total)
                        { Serial.printf("Progress: %u%%\r", (progress / (total / 100))); })
            .onError([](ota_error_t error)
                     {
                    Serial.printf("Error[%u]: ", error);
                    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
                    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
                    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
                    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
                    else if (error == OTA_END_ERROR) Serial.println("End Failed"); });

        ArduinoOTA.begin();
    }
}

void networkingLoop()
{
    ws.cleanupClients();
    ArduinoOTA.handle();
}