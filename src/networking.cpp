#include "globals.h"

#include <ArduinoOTA.h>
#include <ESPmDNS.h>
#include <SPIFFS.h>
#include <WebServer.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiUdp.h>

#include <ESPAsyncWebServer.h>

#define HOSTNAME "scoreboard"
#define OTA_PORT 3232

AsyncWebServer server(80);

bool initialized = false;
void (*onInitalized)();

void networkingSetup(void (*callback)())
{
    Serial.println("Configuring access point...");

    // WIFI.
    // WiFi.mode(WIFI_MODE_APSTA);
    WiFi.mode(WIFI_MODE_STA);

    // AP Setup.
    /*
    {
        WiFi.softAP(configuration.ap_ssid, configuration.ap_password);

        Serial.print("Connecting to: ");
        Serial.println(configuration.ap_ssid);

        Serial.print("ESP32 IP as soft AP: ");
        Serial.println(WiFi.softAPIP());
    }
    */

    if (!SPIFFS.begin(true))
    {
        Serial.println("An Error has occurred while mounting SPIFFS");
        return;
    }

    server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");

    server.on("/data", HTTP_GET,
              [](AsyncWebServerRequest *request)
              {
                  AsyncResponseStream *response = request->beginResponseStream("application/json");
                  serializeJson(dataGetJson(), *response);
                  request->send(response);
              });

    server.on(
        "/data", HTTP_POST,
        [](AsyncWebServerRequest *request) {},
        NULL,
        [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
        {
            Serial.println("Handling post request...");
            dataParseJson(data);
            Serial.println("Sending response...");
            request->send(200, "text/plain", "OK");
        });

    server.onNotFound([](AsyncWebServerRequest *request)
                      { if (request->method() == HTTP_OPTIONS) {
                            request->send(200);
                        } else {
                            request->send(404);
                        } });

    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
    server.begin();

    Serial.println("Server started");

    if (!MDNS.begin(HOSTNAME))
    { // Start the mDNS responder for esp8266.local
        Serial.println("Error setting up MDNS responder!");
    }
    else
    {
        Serial.println("MDNS enabled.");
        MDNS.enableArduino(OTA_PORT, false);
        MDNS.addService("http", "tcp", 80);
    }

    // OTA updates.

    ArduinoOTA.setHostname(HOSTNAME);
    ArduinoOTA.setPort(OTA_PORT);
    ArduinoOTA.setMdnsEnabled(false);

    //   ArduinoOTA.setPassword("admin");

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

    // Station setup.
    {
        Serial.print("Connecting to SSID: ");
        Serial.println(configuration.wifi_ssid);

        WiFi.begin(configuration.wifi_ssid, configuration.wifi_password);
        WiFi.setAutoReconnect(true);
    }

    onInitalized = callback;
}

void networkingLoop()
{
    if (!initialized && WiFi.status() == WL_CONNECTED)
    {
        Serial.print("ESP32 IP on the WiFi network: ");
        Serial.println(WiFi.localIP());

        onInitalized();
        initialized = true;
    }
    else if (!initialized)
    {
        Serial.print(WiFi.status());
    }

    ArduinoOTA.handle();
}