#pragma once
#include "gpio.hpp"
#include "temperature.hpp"
#include "adc.hpp"
#include "scale.hpp"
#include "PubSubClient.h"

class State{
    public:
        char name[MAX_NAME_LENGTH] = INVALID_NAME;
        State();
        bool IsConfigured = false;
        GPIO GPIOs[MAX_GPIO_PINS];
        void ReadGPIOs();
        Temperature Sensors[MAX_TEMP_SENSORS];
        void ReadTemperatures();
        ADC ADCs[MAX_ADC_PINS];
        void ReadADCs();
        Scale Scales[MAX_SCALES];
        void ReadWeights();
        String getDebugPublicationTopic();
        String getConfiguredPublicationTopic();
        String getDebugPayload();
        String getADCsDebug();
        String getTemperatureDebug();
        String getScalesDebug();
};