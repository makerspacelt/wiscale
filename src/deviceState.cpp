#include "deviceState.hpp"

DeviceState::DeviceState()
{
	initializeScales();
}
void DeviceState::ReconfigureScales(Hx711Config scaleConfigs[])
{
	for (uint8_t i = 0; i < USED_SCALES; i++)
	{
		MS_HX711_Scale scale(scaleConfigs[i]);
		Scales[i] = scale;
	}
}
void DeviceState::ReadScales(void)
{
	Serial.println("Reading scales");
	for (uint8_t i = 0; i < USED_SCALES; i++)
	{
		// Read and save to ms_scale
		Scales[i].readScale(true);
		Serial.print("Scale value ");
		Serial.print(Scales[i].grams);
	}
}
void DeviceState::initializeScales(void)
{
	for (uint8_t i = 0; i < USED_SCALES; i++)
	{
		Scales[i] = MS_HX711_Scale();
	}
}
void DeviceState::initializeThermometers(void)
{
}