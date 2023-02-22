#include "global.h" 
#include "temperature.hpp"

OneWire wire;
DallasTemperature sensor;

Temperature::Temperature(void){

}
Temperature::Temperature(JsonObject doc)
{
    strcpy(name, doc["name"]);
    pin = doc["pin"];
    offset = doc["offset"];
    multi = doc["multi"];
    readings = doc["readings"];
    IsConfigured = true;
    initializeSensor();
}
void Temperature::initializeSensor(){
    OneWire _wire(pin);
    wire = _wire;
    DallasTemperature _sensor(&wire);
    sensor = _sensor;;
    sensor.begin();
    float val = sensor.getTempCByIndex(0);
    if (val == DEVICE_DISCONNECTED_C)
    {
        Serial.println("Error: Could not read temperature data");
    }
}
String Temperature::getSubscriptionTopic()
{
    char topic[254];
    sprintf(topic, "device/%s/ds18b20/%s/set", DeviceState.name, name);
    return topic;
}
String Temperature::getPublicationTopic()
{
    char topic[254];
    sprintf(topic, "device/%s/ds18b20/%s/get", DeviceState.name, name);
    return topic;
}
void Temperature::ReadTemperature(uint16_t sensorId)
{
    sensor.requestTemperatures();
    // float sum = 0;
    // float value = 0;
    // for(byte i = 0; i < readings; i++){
    //     value = sensor.getTempCByIndex(sensorId);
    //     if (value == DEVICE_DISCONNECTED_C)
    //     {
    //         Serial.printf("Error: Could not read temperature data of device: %s", name);
    //     }
    //     sum += value;
    //     delay(0);
    // }
    float _temperature = sensor.getTempCByIndex(sensorId);
    if (_temperature == DEVICE_DISCONNECTED_C)
    {
        Serial.printf("Error: Could not read temperature data of device: %s", name);
    }
    pre_offset = _temperature;
    _temperature += offset;
    pre_multi = _temperature;
    _temperature *= multi;
    temperature = _temperature;
}
String Temperature::getDebugObject(){
    char result[254];
    sprintf(result, "{");
    strcat(result, makeJsonLine("name", name).c_str());
    strcat(result, ", ");
    strcat(result, makeJsonLine("pre_offset", pre_offset).c_str());
    strcat(result, ", ");
    strcat(result, makeJsonLine("pre_multi", pre_multi).c_str());
    strcat(result, "}");
    return result;
}