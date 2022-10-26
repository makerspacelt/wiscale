#ifndef SCALE_H
#define SCALE_H
#include "HX711.h"
#include "global.h"

struct MS_HX711_Scale
{
	Hx711Config config = {};
	HX711 scale;
	float grams = 0;
};
// Sets up pins and gains of a single scale
HX711 initScale(Hx711Config config);
// Sets up pins and gains of all of the scales
void initScales(struct Hx711Config configs[]);

// Clears memory from previous configs.
void deInitScales();

// Checks if name is valid
bool isScaleValid(MS_HX711_Scale scale);

// Returns scale and config container
MS_HX711_Scale getMSScale(Hx711Config config);

// Returns regular scale
HX711 getScale(Hx711Config config);

void saveScaleValue(Hx711Config config, float value);

// Reads scale value. If save == true, saves it to MS_HX711_Scale grams.
float readScale(Hx711Config config, bool save = false);
void powerDownScale(HX711 scale);

// Scale container with its configuration and current measurement

#endif