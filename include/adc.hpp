#pragma once
#include "common.h"
#include "ArduinoJson.h"

class ADC
{

public:
    ADC(void);
    ADC(JsonObject doc);
    char name[MAX_NAME_LENGTH] = INVALID_NAME;
    uint8_t pin = 17;
    uint8_t readings = 10;
    float offset = 0;
    float multi = 0.0047460937;
    float adcValue = 0;
    float pre_offset = 0;
    float pre_multi = 0;
    bool IsConfigured = false;

    String getSubscriptionTopic();
    String getPublicationTopic();
    void ReadADC();
    String getDebugObject();
};