#ifndef SCALE_H
#define SCALE_H
#include "HX711.h"
#include "global.h"
// Sets up pins and gains of all of the scales
void initScales(Hx711Config configs[]);
// Sets up pins and gains of a single scale
HX711 initScale(Hx711Config config);

//Clears memory from previous configs. 
void deInitScales();

// Returns regular scale
HX711 getScale(Hx711Config config);

// Returns scale and config container
MS_HX711_Scale getMS_Scale(Hx711Config config);

// Reads scale value. If save == true, saves it to MS_HX711_Scale grams.
float readScale(Hx711Config config, bool save = false);

void powerDownScale(HX711 scale);

// Scale container with its configuration and current measurement
struct MS_HX711_Scale
{
	Hx711Config config = {};
	HX711 scale;
	float grams = 0;
};
#endif