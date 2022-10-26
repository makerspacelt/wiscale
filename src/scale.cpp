#include "scale.h"
HX711 scale;

void initScale()
{
	scale.begin(Config.scale.pin_dt,
				Config.scale.pin_sck,
				Config.scale.gain);
	scale.power_up();
}
void readScale()
{
	float weightValue =  scale.get_units(Config.scale.readings);
	Debug.hx711.gain = Config.scale.gain;
	Debug.hx711.readings = Config.scale.readings;
	Debug.hx711.pre_offset = weightValue;
	weightValue += Config.scale.offset;
	Debug.hx711.pre_multi = weightValue;
	weightValue *= Config.scale.multi;
	State.grams = weightValue;
}
void powerDownScale()
{
	scale.power_down();
}