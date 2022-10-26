#include "scale.h"
MS_HX711_Scale Scales[USED_SCALES] = {};

void deInitScales()
{
	for (uint16_t i = 0; i < USED_SCALES; i++)
	{
		powerDownScale(Scales[i].scale); // Not sure if needed
		Scales[i] = {};
	}
}

void initScales(Hx711Config configs[])
{
	deInitScales();
	if(sizeof(configs)!=USED_SCALES){
        Serial.println("Scale count mismatch! Check configs!");
	   return;
	}
	for (uint16_t i = 0; i < sizeof(configs); i++)
	{
		Scales[i].config=configs[i];
		Scales[i].scale=initScale(configs[i]);
	}
}

HX711 initScale(Hx711Config config)
{
	HX711 scale;
	scale.begin(config.pin_dt,
				config.pin_sck,
				config.gain);
	scale.power_up();

	scale.set_scale(config.multi);
	scale.set_offset(config.offset);
}

void readScale()
{
	State.grams = scale.get_units(READINGS);
}
void powerDownScale(HX711 scale)
{
	scale.power_down();
}