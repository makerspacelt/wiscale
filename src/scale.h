#ifndef SCALE_H
#define SCALE_H
#include "HX711.h"
#include "global.h"
#define READINGS 30

void initScale();
void readScale();
void powerDownScale();
#endif