#include <mutex>
#include "globals.h"

#define NAMESPACE "data"

Preferences preferences;
Configuration configuration;

Message messages[4];
uint messageCount;

std::mutex dataMutex;

void dataLoad()
{
    std::lock_guard<std::mutex> lock(dataMutex);

    preferences.begin(NAMESPACE);

    size_t bufferLength = preferences.getBytesLength("messages");
    char buffer[bufferLength];

    if (bufferLength == sizeof(messages))
    {
        messageCount = preferences.getUInt("messageCount");
        if (messageCount == 0)
            messageCount = 1;

        Serial.print("Loading messages: ");
        Serial.println(messageCount);

        preferences.getBytes("messages", messages, bufferLength);
    }
    else
    {
        Serial.println("No saved messages!");
        preferences.putBytes("messages", messages, sizeof(messages));
        messageCount = 1;
    }

    preferences.end();
}

void dataSave()
{
    // Messages
    preferences.begin(NAMESPACE);
    preferences.putUInt("messageCount", messageCount);
    preferences.putBytes("messages", messages, sizeof(messages));
    preferences.end();
}

void dataParseJson(uint8_t *data)
{
    DynamicJsonDocument doc(1024);
    deserializeJson(doc, data);

    // Messages
    if (doc.containsKey("messages"))
    {
        // Lock resources while updating.
        {
            std::lock_guard<std::mutex> lock(dataMutex);

            JsonArray messagesArray = doc["messages"];
            messageCount = messagesArray.size();
            if (messageCount > 4)
                messageCount = 4;

            for (int i = 0; i < messageCount; i++)
            {
                String text = messagesArray[i]["text"];

                uint speed = messagesArray[i]["speed"];
                uint duration = messagesArray[i]["duration"];

                uint red = messagesArray[i]["red"];
                uint green = messagesArray[i]["green"];
                uint blue = messagesArray[i]["blue"];

                messages[i].text_buffer_length = text.length();
                strcpy(messages[i].text_buffer, text.c_str());

                messages[i].speed = speed;
                messages[i].duration = duration;

                messages[i].red = red;
                messages[i].green = green;
                messages[i].blue = blue;
            }
        }

        dataSave();
    }
}

DynamicJsonDocument dataGetJson()
{

    DynamicJsonDocument doc(1024);

    // Messages
    for (int i = 0; i < messageCount; i++)
    {
        doc["messages"][i]["text"] = messages[i].text_buffer;
        doc["messages"][i]["speed"] = messages[i].speed;
        doc["messages"][i]["duration"] = messages[i].duration;

        doc["messages"][i]["red"] = messages[i].red;
        doc["messages"][i]["green"] = messages[i].green;
        doc["messages"][i]["blue"] = messages[i].blue;
    }

    return doc;
}
