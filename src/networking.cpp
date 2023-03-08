#include "globals.h"
#include "credentials.h"

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

void networkingSetup(void (*callback)())
{
    Serial.println("Configuring access point...");

    // WIFI.
    WiFi.mode(WIFI_MODE_APSTA);

    // AP Setup.
    {
        WiFi.softAP(AP_SSID, AP_PASSWORD);
        Serial.print("ESP32 IP as soft AP: ");
        Serial.println(WiFi.softAPIP());
    }

    if (!SPIFFS.begin(true))
    {
        Serial.println("An Error has occurred while mounting SPIFFS");
        return;
    }

    server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");

    server.on("/", HTTP_POST,
              [](AsyncWebServerRequest *request)
              { 

                if (request->hasParam("brightness", true))
                  {
                      String brightness = request->getParam("brightness", true)->value();
                      Serial.print("Brightness: ");
                      Serial.println(brightness);
                      displaySetBrightness(atoi(brightness.c_str()));
                  }

                //

                if (request->hasParam("ssid", true) && request->hasParam("password", true))
                  {
                      String ssid = request->getParam("ssid", true)->value();
                      String password = request->getParam("password", true)->value();

                      preferences.begin("networking");
                      preferences.putString("ssid", ssid);
                      preferences.putString("password", password);
                      preferences.end();

                      Serial.print("ssid: ");
                      Serial.println(ssid);
                      Serial.print("password: ");
                      Serial.println(password.substring(0, 4));

                      Serial.println("Restarting!");
                      delay(500);
                      ESP.restart();
                  }

                if (request->hasParam("message", true))
                  {
                      String message = request->getParam("message", true)->value();
                      int index = request->getParam("index", true)->value().toInt();
                      int speed = request->getParam("speed", true)->value().toInt();
                      int duration = request->getParam("duration", true)->value().toInt();

                      Serial.print("index: ");
                      Serial.println(index);

                      Serial.print("message: ");
                      Serial.println(message);

                      Serial.print("speed: ");
                      Serial.println(speed);

                      Serial.print("speed: ");
                      Serial.println(duration);

                      displaySetMessage(index, message.c_str(), message.length(), speed, duration);

                      //  displaySetRawText(&text[0], text.length());
                      // TODO(SJG): Set message params.
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
        preferences.begin("networking", true);

        if (preferences.isKey("ssid") && preferences.isKey("password"))
        {
            String ssid = preferences.getString("ssid");
            String password = preferences.getString("password");

            Serial.print("ssid: ");
            Serial.println(ssid);
            Serial.print("password: ");
            Serial.println(password.substring(0, 4));

            WiFi.begin(ssid.c_str(), password.c_str());
            WiFi.setAutoReconnect(true);

            while (WiFi.status() != WL_CONNECTED)
            {
                delay(500);
                Serial.println("Connecting to WiFi..");
            }

            Serial.print("ESP32 IP on the WiFi network: ");
            Serial.println(WiFi.localIP());

            // Fire callback.
            callback();
        }
        else
        {
            Serial.println("No WIFI credentials set!");
        }

        preferences.end();
    }
}

void networkingLoop()
{
    ArduinoOTA.handle();
}