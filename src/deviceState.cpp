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
	for (uint8_t i = 0; i < USED_TEMPERATURE_SENSORS; i++)
	{
		Thermometers[i] = MS_Ds18b20();
	}
}
void DeviceState::ReconfigureThermometers(Ds18b20Config thermometerConfigs[])
{
	Serial.println("Initializing thermometers");

	for (uint8_t i = 0; i < USED_TEMPERATURE_SENSORS; i++)
	{
		MS_Ds18b20 thermometer(thermometerConfigs[i]);
		Thermometers[i] = thermometer;
	}
}
void DeviceState::ReadThermometers(void)
{
	Serial.println("Reading thermometers");
	for (uint16_t i = 0; i < USED_TEMPERATURE_SENSORS; i++)
	{
		// Read and save to ms_scale
		Thermometers[i].readThermometer(0, true);
	}
}