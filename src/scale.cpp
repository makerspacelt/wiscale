#include "scale.h"
HX711 scale;

void initScale()
{
	scale.begin(Config.scale.pin_dt,
				Config.scale.pin_sck,
				Config.scale.gain);
	scale.power_up();

	scale.set_scale(Config.scale.multi);
	scale.set_offset(Config.scale.multi);
}
void readScale()
{
	State.grams = scale.get_units(READINGS);
}
void powerDownScale()
{
	scale.power_down();
}