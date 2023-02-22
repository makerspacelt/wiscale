#pragma once
#include "common.h"
#include "ArduinoJson.h"
#include "HX711.h"

class Scale {

public:
    Scale(void);
    Scale(JsonObject doc);
    char name[MAX_NAME_LENGTH] = INVALID_NAME;
    uint8_t pin_sck = 5;
    uint8_t pin_dt = 4;
    uint8_t gain = 32;
    float offset = 185000;
    float multi = 402.8;
    uint8_t readings = 30;
    float weight = 0;
    float minRead = 0;
    float maxRead = 0;
    float max_scatter = 0;
    float pre_offset = 0;
    float pre_multi = 0;
    bool IsConfigured = false;

    String getSubscriptionTopic();
    String getPublicationTopic();
    void ReadWeight();

    String getDebugObject();
private:
    void initScale();
};