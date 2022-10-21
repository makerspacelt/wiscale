#ifndef ADC_H
#define ADC_H
#include "Arduino.h"
#include "global.h"
#include "stdint.h"
// Reads provided adc pin
uint16_t ReadADC(uint8_t pin);

void readBattery();
// Iterates through Config.adc and sets pin modes
void ReconfigureADC();

#endif