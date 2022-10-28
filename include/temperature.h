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
    const char *name = (char *)INVALID_NAME;
    uint8_t pin = 1;
    float offset = 0;
    float multi = 1;
    /// @brief As this is one wire bus, it can connect up to 127 sensors
    uint8_t sensorsInBus = 1;
};

struct MS_Ds18b20
{
    Ds18b20Config config = {};
    DallasTemperature thermometer;
    float temperature = 0;

#ifdef USES_MULTIPLE_ON_BUS
    float temperatures[];
#endif // DEBUG
};

// Sets up one wire bus with a single scale
DallasTemperature initThermometer(Ds18b20Config config);

// Sets up pins and gains of all of the thermometers
void initThermometers(struct Ds18b20Config configs[]);

// Clears memory from previous configs.
void deInitThermometers();

// Checks if name is valid
bool isScaleValid(MS_Ds18b20 thermometer);

// Returns thermometer and config container
MS_Ds18b20 getMSThermometer(Ds18b20Config config);

// Returns regular thermometer
DallasTemperature getThermometer(Ds18b20Config config);

void saveThermometerValue(Ds18b20Config config, float value);

// Reads Thermometer value. If save == true, saves it to MS_Ds18b20 grams.
float readThermometer(Ds18b20Config config, uint16_t sensorId = 0, bool save = false);
#endif