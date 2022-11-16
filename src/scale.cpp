#include "global.h"
#include "scale.hpp"

HX711 scale;
Scale::Scale(void){

}
Scale::Scale(JsonObject doc){
    strcpy(name, doc["name"]);
    pin_sck = doc["pin_sck"];
    pin_dt = doc["pin_dt"];
    gain = doc["gain"];
    readings = doc["readings"];
    offset = doc["offset"];
    multi = doc["multi"];
    IsConfigured = true;
    initScale();
}
void Scale::initScale(){
    scale.begin(pin_dt, pin_sck, gain);
    scale.power_up();
}
String Scale::getSubscriptionTopic()
{
    char topic[254];
    sprintf(topic, "device/%s/hx711/%s/set", DeviceState.name, name);
    return topic;
}
String Scale::getPublicationTopic()
{
    char topic[254];
    sprintf(topic, "device/%s/hx711/%s/get", DeviceState.name, name);
    return topic;
}
void Scale::ReadWeight()
{
    if(readings <= 0)
    {
        Serial.println("Readings less than 1.");
        return;
    }
    float grams = scale.get_units(readings);
    pre_offset = grams;
    grams += offset;
    pre_multi = grams;
    grams *= multi;
    weight = grams;
}
String Scale::getDebugObject(){
    char result[254];
    sprintf(result, "{");
    strcat(result, makeJsonLine("name", name).c_str());
    strcat(result, ", ");
    strcat(result, makeJsonLine("readings", readings).c_str());
    strcat(result, ", ");
    strcat(result, makeJsonLine("gain", gain).c_str());
    strcat(result, ", ");
    strcat(result, makeJsonLine("pre_offset", pre_offset).c_str());
    strcat(result, ", ");
    strcat(result, makeJsonLine("pre_multi", pre_multi).c_str());
    strcat(result, "}");
    return result;
}
