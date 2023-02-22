#include "global.h"
#include "scale.hpp"


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
    max_scatter = doc["max_scatter"];
    IsConfigured = true;
    initScale();
}
float Scale::getWeight(){
    return weight;
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
    while(!scale.is_ready()){
        delay(100);
    }
    minRead = 0;
    maxRead = 0;
    if(readings <= 0)
    {
        Serial.println("Readings less than 1.");
        return;
    }
    float grams = 0;
    //float grams = scale.get_units(readings);
    for (size_t i = 0; i < readings; i++)
    {
        long value = scale.read();
        grams += value;
        if(value < minRead)
            minRead = value;
        if(maxRead == 0)
            maxRead = value;
        if(value > maxRead)
            maxRead = value;
    }
    grams = grams / readings;
    pre_offset = grams;
    grams += offset;
    pre_multi = grams;
    grams *= multi;
    weight = grams;
    Serial.printf("Scale %s, weight: %.3f", name, weight);
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
    strcat(result, ", ");
    strcat(result, makeJsonLine("min_read", minRead).c_str());
    strcat(result, ", ");
    strcat(result, makeJsonLine("max_read", maxRead).c_str());
    strcat(result, "}");
    return result;
}
