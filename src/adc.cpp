#include "global.h"
#include "adc.hpp"
#include "ArduinoJson.h"

ADC::ADC(void)
{
}
ADC::ADC(JsonObject doc)
{
    strcpy(name, doc["name"]);
    pin = doc["pin"];
    readings = doc["readings"];
    offset = doc["offset"];
    multi = doc["multi"];
    IsConfigured = true;
}
String ADC::getSubscriptionTopic()
{
    char topic[254];
    sprintf(topic, "device/%s/adc/%s/set", DeviceState.name, name);
    return topic;
}
String ADC::getPublicationTopic()
{
    char topic[254];
    sprintf(topic, "device/%s/adc/%s/get", DeviceState.name, name);
    return topic;
}
void ADC::ReadADC()
{
    float sum = 0;
    for(byte i = 0; i < readings; i++){
        sum += analogRead(pin);
        delay(0);
    }
    float val = sum / readings;
    pre_offset = val;
    val += offset;
    pre_multi = val;
    val *= multi;
    adcValue = val;
}
String ADC::getDebugObject(){
    char result[254];
    sprintf(result, "{");
    strcat(result, makeJsonLine("name", name).c_str());
    strcat(result, ", ");
    strcat(result, makeJsonLine("readings", readings).c_str());
    strcat(result, ", ");
    strcat(result, makeJsonLine("pre_offset", pre_offset).c_str());
    strcat(result, ", ");
    strcat(result, makeJsonLine("pre_multi", pre_multi).c_str());
    strcat(result, "}");
    return result;
}