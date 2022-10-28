#include "scale.h"
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
void initScales(struct Hx711Config configs[])
{
	Serial.println("Initializing scales");
	deInitScales();

	for (uint16_t i = 0; i < USED_SCALES; i++)
	{
		State.scales[i].config = configs[i];
		State.scales[i].scale = initScale(configs[i]);
	}
}

void deInitScales()
{
	for (uint16_t i = 0; i < USED_SCALES; i++)
	{
		powerDownScale(State.scales[i].scale); // Not sure if needed
		State.scales[i] = {};
	}
}

bool isScaleValid(struct MS_HX711_Scale scale)
{
	return !strcmp(scale.config.name ,INVALID_NAME);
}

MS_HX711_Scale* getMSScale(struct Hx711Config config)
{
	for (uint16_t i = 0; i < USED_SCALES; i++)
	{
		Serial.println("getMSScale Looking for scale");
		Serial.println(State.scales[i].config.name);
		Serial.println(config.name);
		if (strcmp( State.scales[i].config.name,config.name))
		{
			//Return reference to memory.
			return &State.scales[i];
		}
	}
	Serial.println("getMSScale Scale not found");
	// No scale found
	return {};
}
HX711 getScale(struct Hx711Config config)
{
	return (getMSScale(config)->scale);
}
void saveScaleValue(struct Hx711Config config, float value)
{
	MS_HX711_Scale* scale = getMSScale(config);
	scale->grams = value;
	Serial.println(scale->grams);
}

float readScale(struct Hx711Config config, bool save)
{
	// TODO still does not work.
	if (config.readings<=0)
	{
		Serial.println("Readings invalid.");
		return 0;
	}
	HX711 scale=getScale(config);
	//Serial.println(scale.get_scale());
	//Serial.println(scale.get_offset());
	float grams = scale.get_units(config.readings);
	Serial.print("Read weight: ");
	Serial.println(grams);
	if (save)
	{
		saveScaleValue(config, grams);
	}
	
	return 0;// grams;
}
void powerDownScale(HX711 scale)
{
	scale.power_down();
}