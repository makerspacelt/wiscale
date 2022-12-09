#include "global.h"
#include "gpio.hpp"

GPIO::GPIO(void){

}
GPIO::GPIO(JsonObject doc)
{
    // serializeJson(doc, Serial);
    strcpy(name, doc["name"]);
    const char *modeInJson = doc["mode"];
    if (strcmp(modeInJson, "output") == 0)
    {
        mode = GPIOOut;
    }
    else
    {
        mode = GPIOIn;
    }
    pin = doc["pin"];
    defaultValue = doc["defaultValue"];
    inverted = doc["inverted"];
    pinMode(pin, mode == GPIOIn ? INPUT : OUTPUT);
    IsConfigured = true;
}
String GPIO::getSubscriptionTopic()
{
    char topic[254];
    sprintf(topic, "device/%s/gpio/%s/set", DeviceState.name, name);
    return topic;
}
String GPIO::getPublicationTopic()
{
    char topic[254];
    sprintf(topic, "device/%s/gpio/%s/get", DeviceState.name, name);
    return topic;
}
void GPIO::ReadGPIO(){
    value = digitalRead(pin);
}
