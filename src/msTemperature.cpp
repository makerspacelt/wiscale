#include "msTemperature.hpp"

OneWire wire;
DallasTemperature sensors;

MS_Ds18b20::MS_Ds18b20()
{
}
MS_Ds18b20::MS_Ds18b20(struct Ds18b20Config deviceConfig)
{
	config = deviceConfig;
	Serial.print("Initializing thermometer Pin ");
	Serial.println(deviceConfig.pin);
	OneWire _wire(deviceConfig.pin);
	wire = _wire;
	DallasTemperature _sensors(&wire);
	sensors = _sensors;
	Serial.print("Starting one wire");
	sensors.begin();

	float value = sensors.getTempCByIndex(0);
	if (value == DEVICE_DISCONNECTED_C)
	{
		Serial.println("Error: Could not read temperature data");
	}
	else
	{

		Serial.println("device found");
	}
}

// Checks if name is valid
bool isSensorValid(MS_Ds18b20 thermometer)
{
	return thermometer.config.name != INVALID_NAME;
}

// Reads Thermometer value. If save == true, saves it to MS_Ds18b20 grams.
float MS_Ds18b20::readThermometer(uint16_t sensorId, bool save)
{
	// TODO read multiple on bus
	sensors.requestTemperatures(); // Send the command to get temperatures
	float value = sensors.getTempCByIndex(sensorId);
	if (value == DEVICE_DISCONNECTED_C)
	{
		Serial.println("Error: Could not read temperature data");
	}
	// TODO multiple reads on bus
	debug.pre_offset = value;
	value += config.offset;
	debug.pre_multi = value;
	value *= config.multi;
	if (save)
	{
		temperature = value;
	}
	return value;
}
