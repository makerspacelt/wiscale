#pragma once
#include "common.h"
#include "ArduinoJson.h"

enum GPIOMode
{
    GPIOIn,
    GPIOOut
};
class GPIO
{

public:
    GPIO(void);
    GPIO(JsonObject doc);
    char name[MAX_NAME_LENGTH] = INVALID_NAME;
    uint8_t pin = 0;
    GPIOMode mode = GPIOOut;
    uint8_t inverted = 0;
    uint8_t defaultValue = 0;
    int value = 0;
    bool IsConfigured = false;

    String getSubscriptionTopic();
    String getPublicationTopic();
    void ReadGPIO();
};
