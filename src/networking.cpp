#include "globals.h"

#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>

const char *ssid = "Scoreboard";
const char *password = "12345678";

WebServer server(80);

void handleRoot()
{
    Serial.print("HELLO: ");
    if (server.hasArg("hello"))
    {
        Serial.print(server.arg("hello"));
    }

    server.send(200, "text/plain", "hello from esp32!");
}

void handleNotFound()
{
    String message = "File Not Found\n\n";
    message += "URI: ";
    message += server.uri();
    message += "\nMethod: ";
    message += (server.method() == HTTP_GET) ? "GET" : "POST";
    message += "\nArguments: ";
    message += server.args();
    message += "\n";
    for (uint8_t i = 0; i < server.args(); i++)
    {
        message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
    }
    server.send(404, "text/plain", message);
}

void networkingSetup()
{
    Serial.println("Configuring access point...");

    // You can remove the password parameter if you want the AP to be open.
    WiFi.softAP(ssid, password);
    IPAddress myIP = WiFi.softAPIP();

    Serial.print("AP IP address: ");
    Serial.println(myIP);

    server.on("/", handleRoot);

    server.on("/inline", []()
              { server.send(200, "text/plain", "this works as well"); });

    server.onNotFound(handleNotFound);

    server.begin();

    Serial.println("Server started");

    if (!MDNS.begin("scoreboard"))
    { // Start the mDNS responder for esp8266.local
        Serial.println("Error setting up MDNS responder!");
    }
}

void networkingLoop()
{
    server.handleClient();
}