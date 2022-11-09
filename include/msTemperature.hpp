#ifndef TEMPERATURE_H
#define TEMPERATURE_H
#pragma once
#include "common.h"
#include <DallasTemperature.h>
#include <OneWire.h>

#define USED_TEMPERATURE_SENSORS 1
// TODO multiple sensors on bus functionality
//#define USES_MULTIPLE_ON_BUS
struct Ds18b20Config
{
    char name[MAX_NAME_LENGTH] = INVALID_NAME;
    uint8_t pin = 1;
    float offset = 0;
    float multi = 1;
    /// @brief As this is one wire bus, it can connect up to 127 sensors
    uint8_t sensorsInBus = 1;
};

class MS_Ds18b20
{
public:
    Ds18b20Config config = {};
    float temperature = 0;

#ifdef USES_MULTIPLE_ON_BUS
    float temperatures[];
#endif // DEBUG

    MS_Ds18b20(void);
    MS_Ds18b20(struct Ds18b20Config deviceConfig);

    // Checks if name is valid
    bool isSensorValid(MS_Ds18b20 thermometer);

    // Reads Thermometer value. If save == true, saves it to MS_Ds18b20 grams.
    float readThermometer(uint16_t sensorId = 0, bool save = false);
};

#endif