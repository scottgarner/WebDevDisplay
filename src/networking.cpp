#include "globals.h"

#include <ArduinoOTA.h>
#include <ArduinoJson.h>

#include <WiFiManager.h>
#include <ESPAsyncWebServer.h>

WiFiManager wm;

AsyncWebServer server(HTTP_PORT);
AsyncWebSocket ws("/ws");

uint8_t buffer[16385];
int bufferSize = 0;

void processBuffer(uint8_t *buffer, size_t bufferSize)
{
    displaySetBuffer(buffer, bufferSize, true);
}

void networkingSetup()
{
    // wm.resetSettings();
    wm.setConfigPortalBlocking(false);
    wm.setWiFiAutoReconnect(true);
    wm.setHostname(HOSTNAME);

    if (wm.autoConnect(HOSTNAME))
    {
        Serial.println("Connected!");
    }
    else
    {
        Serial.println("No connection. Running config porta.");
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
                StaticJsonDocument<200> jsonDoc;

                jsonDoc["SSID"] = WiFi.SSID();
                jsonDoc["IP"] = WiFi.localIP().toString();
                jsonDoc["RSSI"] = WiFi.RSSI();

                String jsonString;
                serializeJson(jsonDoc, jsonString);

                request->send(200, "application/json", jsonString);
            });

        server.on(
            "/buffer", HTTP_GET,
            [](AsyncWebServerRequest *request)
            {
                request->send(200, "text/plain", "OK");
            });

        server.on(
            "/buffer", HTTP_POST,
            [](AsyncWebServerRequest *request) {},
            NULL,
            [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
            {
                if (index == 0)
                {
                    Serial.printf("BodyStart: %u\n", total);
                    bufferSize = 0;
                }

                Serial.printf("Length: %u/%u \n", len, total);
                for (size_t i = 0; i < len; i++)
                {
                    buffer[bufferSize] = data[i];
                    bufferSize++;
                }

                if (index + len == total)
                {
                    Serial.printf("BodyEnd: %u %u\n", total, bufferSize);
                    processBuffer(buffer, bufferSize);
                    request->send(200, "text/plain", "OK");
                }
            });

        server.onNotFound([](AsyncWebServerRequest *request)
                          { 
                        if (request->method() == HTTP_OPTIONS) {
                            Serial.println("Options!");
                            request->send(200, "text/plain", "OK");
                        } else {
                            Serial.println("404?");
                            request->send(200, "text/plain", "OK?");
                        } });
    }

    // Websocket handler.
    {
        ws.onEvent(
            [](AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len)
            {
                if (type == WS_EVT_CONNECT)
                {
                    Serial.println("Client connected");
                    client->text("Hello Client!");
                }
                else if (type == WS_EVT_DISCONNECT)
                {
                    Serial.println("Client disconnected");
                }
                else if (type == WS_EVT_DATA)
                {
                    AwsFrameInfo *info = (AwsFrameInfo *)arg;

                    if (info->index == 0)
                    {
                        bufferSize = 0;
                    }

                    for (size_t i = 0; i < len; i++)
                    {
                        buffer[bufferSize] = data[i];
                        bufferSize++;
                    }

                    if (info->final)
                    {
                        // Serial.println("Last frame received");
                        // Serial.printf("Received full data: %u bytes\n", bufferSize);
                        processBuffer(buffer, bufferSize);
                    }
                }
            });
        server.addHandler(&ws);
    }

    // Static files.
    {

        if (!LittleFS.begin(true))
        {
            Serial.println("An Error has occurred while mounting LittleFS!");
        }

        server.serveStatic("/assets/index.js", LittleFS, "/assets/index.js").setCacheControl("max-age=31536000");
        server.serveStatic("/assets/index.css", LittleFS, "/assets/index.css").setCacheControl("max-age=31536000");
        server.serveStatic("/", LittleFS, "/").setDefaultFile("index.html").setCacheControl("max-age=31536000");
    }

    // Server begin.
    server.begin();

    // OTA updates.
    {
        ArduinoOTA.setHostname(HOSTNAME);
        ArduinoOTA.setPort(OTA_PORT);
        ArduinoOTA.setMdnsEnabled(true);

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
    wm.process();
    ws.cleanupClients();
    ArduinoOTA.handle();
}