#ifndef TEMPERATURE_H
#define TEMPERATURE_H
#include "global.h"
#include <OneWire.h>
#include <DallasTemperature.h>

// TODO get this from config
#define ONE_WIRE_BUS 14 //D5
// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

void readTemperature();
void ReconfigureTemperature();
#endif