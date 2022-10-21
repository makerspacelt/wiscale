#ifndef GPIO_H
#define GPIO_H
#include "Arduino.h"
#include "global.h"

uint8_t ReadGPIO(uint8_t pin);
uint8_t ReadGPIOByName(char* pinName);
void ReconfigureGPIO();

#endif