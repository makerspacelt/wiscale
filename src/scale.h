#ifndef SCALE_H
#define SCALE_H
#include "HX711.h"
#include "global.h"
#define READINGS 30

HX711 scale;

void initScale();
void readScale();
void powerDownScale();
#endif