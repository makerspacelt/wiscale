#include "msScale.hpp"
#include "global.h"

HX711 initHX711Scale(struct Hx711Config config)
{
	HX711 scale;
	scale.begin(config.pin_dt,
				config.pin_sck,
				config.gain);
	scale.power_up();

	scale.set_scale(config.multi);
	scale.set_offset(config.offset);
	return scale;
}

MS_HX711_Scale::MS_HX711_Scale(void)
{
}
MS_HX711_Scale::MS_HX711_Scale(struct Hx711Config scaleConfig)
{
	initScale(scaleConfig);
}
void MS_HX711_Scale::initScale(struct Hx711Config scaleConfig)
{
	Serial.println("Initializing scales");
	config = scaleConfig;
	scale = initHX711Scale(config);
	grams = 0;
}

bool MS_HX711_Scale::isScaleValid(MS_HX711_Scale scale)
{
	return !strcmp(scale.config.name, INVALID_NAME);
}

float MS_HX711_Scale::readScale(bool save)
{
	if (config.readings <= 0)
	{
		Serial.println("Readings invalid.");
		return 0;
	}
	// Serial.println(scale.get_scale());
	// Serial.println(scale.get_offset());
	float _grams = scale.get_units(config.readings);
	Serial.print("Read weight: ");
	Serial.println(grams);
	if (save)
	{
		grams = _grams;
	}

	return grams;
}
