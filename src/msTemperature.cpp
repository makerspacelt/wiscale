#include "msTemperature.hpp"

MS_Ds18b20::MS_Ds18b20()
{
}
MS_Ds18b20::MS_Ds18b20(struct Ds18b20Config deviceConfig)
{
	config = deviceConfig;
	Serial.print("Initializing thermometer Pin ");
	Serial.println(deviceConfig.pin);
	OneWire oneWire(deviceConfig.pin);
	DallasTemperature sensors(&oneWire);
	Serial.print("Starting one wire");
	sensors.begin();
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
	thermometer.requestTemperatures(); // Send the command to get temperatures
	float value = thermometer.getTempCByIndex(sensorId);
	// TODO multiple reads on bus
	if (save)
	{
		temperature = value;
	}
	return value;
}
