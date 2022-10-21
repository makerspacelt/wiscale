#ifndef TEMPERATURE_H
#define TEMPERATURE_H
#pragma once
#include "global.h"
#include <DallasTemperature.h>
#include <OneWire.h>

// TODO get this from config
#define ONE_WIRE_BUS 14 //D5

void readTemperature();
void ReconfigureTemperature();
#endif