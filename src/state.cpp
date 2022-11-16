#include "state.hpp"


State::State(){

}
void State::ReadGPIOs()
{
    for(uint8_t i = 0; i < MAX_GPIO_PINS; i++){
        GPIO *gpio = &GPIOs[i];
        if(!gpio->IsConfigured || gpio->mode == GPIOOut) continue;
        gpio->ReadGPIO();
    }
}
void State::ReadTemperatures(){
    for (uint8_t i = 0; i < MAX_TEMP_SENSORS; i++)
    {
        Temperature *sensor = &Sensors[i];
        if (!sensor->IsConfigured)
            continue;
        sensor->ReadTemperature();
    }
}
void State::ReadADCs(){
    for (uint8_t i = 0; i < MAX_ADC_PINS; i++)
    {
        ADC *adc = &ADCs[i];
        if (!adc->IsConfigured)
            continue;
        adc->ReadADC();
    }
}
void State::ReadWeights(){
    for(uint8_t i = 0; i < MAX_SCALES; i++){
        Scale *scale = &Scales[i];
        if(!scale->IsConfigured) 
            continue;
        scale->ReadWeight();
    }
}
String State::getDebugPublicationTopic()
{
    char topic[254];
    sprintf(topic, "device/%s/debug", name);
    return topic;
}
String State::getConfiguredPublicationTopic()
{
    char topic[254];
    sprintf(topic, "device/%s/configured", name);
    return topic;
}
String State::getADCsDebug(){
    char result[254];
    sprintf(result, "[");
    for(uint8_t i = 0; i < MAX_ADC_PINS; i++){
        ADC *adc = &ADCs[i];
        if(!adc->IsConfigured)
            continue;
        strcat(result, adc->getDebugObject().c_str());
        if(i < MAX_ADC_PINS - 1 && ADCs[i + 1].IsConfigured)
            strcat(result, ", ");
    }
    strcat(result, "]");
    return result;
}
String State::getTemperatureDebug(){
    char result[254];
    sprintf(result, "[");
    for (uint8_t i = 0; i < MAX_TEMP_SENSORS; i++)
    {
        Temperature *sensor = &Sensors[i];
        if (!sensor->IsConfigured)
            continue;
        strcat(result, sensor->getDebugObject().c_str());
        if (i < MAX_TEMP_SENSORS - 1 && Sensors[i + 1].IsConfigured)
            strcat(result, ", ");
    }
    strcat(result, "]");
    return result;
}
String State::getScalesDebug(){
    char result[254];
    sprintf(result, "[");
    for (uint8_t i = 0; i < MAX_SCALES; i++)
    {
        Scale *scale = &Scales[i];
        if (!scale->IsConfigured)
            continue;
        strcat(result, scale->getDebugObject().c_str());
        if (i < MAX_SCALES - 1 && Scales[i + 1].IsConfigured)
            strcat(result, ", ");
    }
    strcat(result, "]");
    return result;
}
String State::getDebugPayload(){
    char payload[1024];
    sprintf(payload, "{\"adc\" : %s, \"hx711\": %s, \"ds18b20\" : %s}", getADCsDebug().c_str(), getTemperatureDebug().c_str(), getScalesDebug().c_str());
    return payload;
}