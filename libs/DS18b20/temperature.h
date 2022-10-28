#ifndef TEMPERATURE_H
#define TEMPERATURE_H
#pragma once
#include "global.h"
#include <DallasTemperature.h>
#include <OneWire.h>


// TODO get this from config
#define ONE_WIRE_BUS 14 //D5
#define NUMBER_OF_TEMPERATURE_SENSORS 1

// Reads first temperature sensor
void readTemperature();

Ds18b20Config readTemperature(uint16_t sensorId);
Ds18b20Config[] readAllTemperatureSensors();

void ReconfigureTemperature();
#endif