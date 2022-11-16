#ifndef GLOBAL_H
#define GLOBAL_H
#include "stdint.h"
#include "common.h"
#include "state.hpp"

#define USE_GPIO
#define USE_ADC
#define USE_DS18
#define USE_SCALE
#endif

extern State DeviceState;
extern bool RetainPublications;
bool prefix(const char *pre, const char *str);
String makeJsonLine(const char *key, uint8_t value);
String makeJsonLine(const char *key, float value);
String makeJsonLine(const char *key, const char *value);