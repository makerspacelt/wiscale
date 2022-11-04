#include "msScale.hpp"
#include "global.h"

HX711 initScale(struct Hx711Config config)
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
void MS_HX711_Scale::initScales(struct Hx711Config configs[])
{
	Serial.println("Initializing scales");
	deInitScales();

	for (uint16_t i = 0; i < USED_SCALES; i++)
	{
		State.Scales[i].config = configs[i];
		State.Scales[i].scale = initScale(configs[i]);
	}
}

bool MS_HX711_Scale::isScaleValid(MS_HX711_Scale scale)
{
	return !strcmp(scale.config.name, INVALID_NAME);
}

MS_HX711_Scale *MS_HX711_Scale::getMSScale(struct Hx711Config config)
{
	MS_HX711_Scale *scales = State.Scales;
	for (uint16_t i = 0; i < USED_SCALES; i++)
	{
		Serial.println("getMSScale Looking for scale");
		Serial.println(State.Scales[i].config.name);
		Serial.println(config.name);
		if (strcmp(State.Scales[i].config.name, config.name))
		{
			// Return reference to memory.
			return &State.Scales[i];
		}
	}
	Serial.println("getMSScale Scale not found");
	// No scale found
	return {};
}
HX711 MS_HX711_Scale::getScale(struct Hx711Config config)
{
	return getMSScale(config)->scale;
}
void MS_HX711_Scale::saveScaleValue(struct Hx711Config config, float value)
{
	MS_HX711_Scale *scale = getMSScale(config);
	scale->grams = value;
	Serial.println(scale->grams);
	free(scale);
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
