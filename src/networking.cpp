#include "globals.h"
#include "credentials.h"

#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <SPIFFS.h>

#include <ESPAsyncWebServer.h>

AsyncWebServer server(80);

void handlePost()
{
}

void networkingSetup()
{
    Serial.println("Configuring access point...");

    WiFi.mode(WIFI_MODE_APSTA);

    WiFi.softAP(AP_SSID, AP_PASSWORD);
    Serial.print("ESP32 IP as soft AP: ");
    Serial.println(WiFi.softAPIP());

    if (!SPIFFS.begin(true))
    {
        Serial.println("An Error has occurred while mounting SPIFFS");
        return;
    }

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.println("Connecting to WiFi..");
    }

    Serial.print("ESP32 IP on the WiFi network: ");
    Serial.println(WiFi.localIP());

    server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");

    server.on("/", HTTP_POST,
              [](AsyncWebServerRequest *request)
              { 

                if (request->hasParam("brightness", true))
                  {
                      String brightness = request->getParam("brightness", true)->value();
                      Serial.print("Brightness: ");
                      Serial.println(brightness);
                      displayBrightness(atoi(brightness.c_str()));
                  }

                //

                if (request->hasParam("text", true))
                  {
                      String text = request->getParam("text", true)->value();
                      Serial.print("Text: ");
                      Serial.println(text);
                      displayText(&text[0], text.length());
                  }

                  //

                  request->send(200, "text/plain", "OK"); });

    server.onNotFound([](AsyncWebServerRequest *request)
                      { if (request->method() == HTTP_OPTIONS) {
                            request->send(200);
                        } else {
                            request->send(404);
                        } });

    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
    server.begin();

    Serial.println("Server started");

    if (!MDNS.begin("scoreboard"))
    { // Start the mDNS responder for esp8266.local
        Serial.println("Error setting up MDNS responder!");
    }
}

void networkingLoop()
{
}