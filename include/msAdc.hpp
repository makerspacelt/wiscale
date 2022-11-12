#pragma once
#include "common.h"

#define USED_ADC_PINS 1

struct ADCConfig
{
    uint8_t pin = 17; // A0
    char name[MAX_NAME_LENGTH] = INVALID_NAME;
    uint32_t readings = 10;
    char mode[MAX_MODE_LENGTH] = "input";
    float offset = 0;
    float multi = 0.0047460937; // How to convert to voltage.
};

struct ADCDebug
{
    int readings = 0;
    float pre_offset = 0;
    float pre_multi = 0;
};

class MS_ADC
{
public:
    ADCConfig config = {};
    float voltage = 0;
    ADCDebug debug = {};
    MS_ADC(void);
    MS_ADC(struct ADCConfig config);
    void initAdc(struct ADCConfig config);
    float readAdc(bool save = false);
};

