#pragma once
#include "common.h"
#include "ArduinoJson.h"
#include <DallasTemperature.h>
#include <OneWire.h>

class Temperature
{
    public:
        Temperature(void);
        Temperature(JsonObject doc);
        char name[MAX_NAME_LENGTH] = INVALID_NAME;
        uint8_t pin = 0;
        float offset = 0;
        float multi = 1;
        float temperature = 0;
        float readings = 1;
        float pre_offset = 0;
        float pre_multi = 0;
        bool IsConfigured = false;

        String getSubscriptionTopic();
        String getPublicationTopic();
        void ReadTemperature(uint16_t sensorId = 0);
        String getDebugObject();

    private:
        void initializeSensor();
};