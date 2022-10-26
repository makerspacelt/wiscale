#include "scale.h"
MS_HX711_Scale Scales[USED_SCALES] = {};

HX711 initScale(Hx711Config config)
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
void initScales(struct Hx711Config configs[])
{
	deInitScales();
	if (sizeof(configs) != USED_SCALES)
	{
		Serial.println("Scale count mismatch! Check configs!");
		return;
	}
	for (uint16_t i = 0; i < sizeof(configs); i++)
	{
		Scales[i].config = configs[i];
		Scales[i].scale = initScale(configs[i]);
	}
}

void deInitScales()
{
	for (uint16_t i = 0; i < USED_SCALES; i++)
	{
		powerDownScale(Scales[i].scale); // Not sure if needed
		Scales[i] = {};
	}
}

bool isScaleValid(MS_HX711_Scale scale)
{
	return scale.config.name != INVALID_NAME;
}

MS_HX711_Scale getMSScale(Hx711Config config)
{
	for (uint16_t i = 0; i < USED_SCALES; i++)
	{
		if (&Scales[i].config == &config)
		{
			return Scales[i];
		}
	}
	// No scale found
	return {};
}
HX711 getScale(Hx711Config config)
{
	return getMSScale(config).scale;
}
void saveScaleValue(Hx711Config config, float value)
{
	MS_HX711_Scale scale = getMSScale(config);
	if (isScaleValid(scale))
	{
		scale.grams = value;
	}
}

float readScale(Hx711Config config, bool save)
{
	float grams = getScale(config).get_units(config.readings);
	if (save)
	{
		saveScaleValue(config, grams);
	}
	return grams;
}
void powerDownScale(HX711 scale)
{
	scale.power_down();
}