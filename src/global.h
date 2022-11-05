#ifndef GLOBAL_H
#define GLOBAL_H
#include "common.h"
#include "deviceState.hpp"
#include "msScale.hpp"
#include "msTemperature.hpp"
#include "stdint.h"

// todo https://ubidots.com/blog/connect-your-esp8266-to-any-available-wi-fi-network/

#define EXECUTION_TIMEOUT 10
#define MAX_GPIO_PINS 24
#define MAX_ADC_PINS 2
#define USE_GPIO //
#define USE_ADC  // Working
#define USE_DS18
#define USE_SCALE

struct GPIOConfig
{
    uint8_t pin = 2;
    char name[MAX_NAME_LENGTH] = INVALID_NAME;
    uint8_t mode = 0; // 0 - input, 1 - output
    uint8_t inverted = 0;
    uint8_t defaultValue = 0;
    uint8_t configured = 0;
};

struct ADCConfig
{
    uint8_t pin = 17; // A0
    char name[MAX_NAME_LENGTH] = INVALID_NAME;
    uint32_t readingsForMean = 10;
    float offset = 0;
    float multiplier = 0.0047460937; // How to convert to voltage.
    uint8_t configured = 0;
};

struct GlobalConfig
{
    int battery_range = 4860;
    char name[MAX_NAME_LENGTH] = "ESP-D4EFA3";
    GPIOConfig gpio[MAX_GPIO_PINS] = {}; // structs must be initialized with {} to prevent garbage data
    ADCConfig adc[MAX_ADC_PINS] = {};
    Hx711Config scales[USED_SCALES] = {};
    Ds18b20Config thermometers[USED_TEMPERATURE_SENSORS];
};

extern DeviceState State;
extern GlobalConfig Config;

#endif // GLOBAL_H