#ifndef GLOBAL_H
#define GLOBAL_H
#include "stdint.h"
#include "common.h"
#include "scale.h"

//#define CONFIGURE_WIFI true
#define WIFI_SSID "SSID"
#define WIFI_PASS "PASS"
// todo https://ubidots.com/blog/connect-your-esp8266-to-any-available-wi-fi-network/

#define EXECUTION_TIMEOUT 10
#define MAX_GPIO_PINS 24
#define MAX_ADC_PINS 2

struct GPIOConfig
{
    uint8_t pin = 2;
    const char *name = (char *)INVALID_NAME;
    uint8_t mode = 0; // 0 - input, 1 - output
    uint8_t inverted = 0;
    uint8_t defaultValue = 0;
    uint8_t configured = 0;
};

struct ADCConfig
{
    uint8_t pin = 17; // A0
    const char *name = (char *)INVALID_NAME;
    uint32_t readingsForMean = 10;
    float offset = 0;
    float multiplier = 0.0047460937; // How to convert to voltage.
    uint8_t configured = 0;
};



struct Ds18b20Config
{
    const char *name = (char *)INVALID_NAME;
    uint8_t pin = 1;
    float offset = 0;
    float multi = 1;
};

struct GlobalConfig
{
    int battery_range = 4860;
    const char *name = (char *)"ESP-D4EFA3";
    GPIOConfig gpio[MAX_GPIO_PINS] = {}; // structs must be initialized with {} to prevent garbage data
    ADCConfig adc[MAX_ADC_PINS] = {};
    Hx711Config scales[USED_SCALES] = {};
    Ds18b20Config temperature = {};
};

struct GlobalState
{
    MS_HX711_Scale scales[USED_SCALES] = {};
    float battery = 0;
    float temperature = 0;
    int charging = 0;
    int configured = 0;
};

extern GlobalState State;
extern GlobalConfig Config;

#endif // GLOBAL_H