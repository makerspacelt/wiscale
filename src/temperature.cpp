#include "temperature.h"
MS_Ds18b20 Thermometers[USED_TEMPERATURE_SENSORS] = {};

DallasTemperature initThermometer(Ds18b20Config config)
{
	OneWire oneWire(config.pin);
	DallasTemperature sensors(&oneWire);
	sensors.begin();
	return sensors;
}

void initThermometers(struct Ds18b20Config configs[])
{
	deInitThermometers();
	if (sizeof(configs) != USED_TEMPERATURE_SENSORS) // TODO sizeof is incorrect
	{
		Serial.println("Themperature count mismatch! Check configs!");
		return;
	}
	for (uint16_t i = 0; i < sizeof(configs); i++)
	{
		Thermometers[i].config = configs[i];
		Thermometers[i].thermometer = initThermometer(configs[i]);
	}
}

// Clears memory from previous configs.
void deInitThermometers()
{
	for (uint16_t i = 0; i < USED_TEMPERATURE_SENSORS; i++)
	{
		Thermometers[i] = {};
	}
}

// Checks if name is valid
bool isScaleValid(MS_Ds18b20 thermometer)
{
	return thermometer.config.name != INVALID_NAME;
}

// Returns thermometer and config container
MS_Ds18b20 getMSThermometer(Ds18b20Config config)
{
	{
		for (uint16_t i = 0; i < USED_TEMPERATURE_SENSORS; i++)
		{
			if (&Thermometers[i].config == &config)
			{
				return Thermometers[i];
			}
		}
		// No scale found
		return {};
	}
}

// Returns regular thermometer
DallasTemperature getThermometer(Ds18b20Config config)
{
	{
		return getMSThermometer(config).thermometer;
	}
}

void saveThermometerValue(Ds18b20Config config, float value)
{
	MS_Ds18b20 thermometer = getMSThermometer(config);
	if (isScaleValid(thermometer))
	{
		thermometer.thermometers = value;
	}
}

// Reads Thermometer value. If save == true, saves it to MS_Ds18b20 grams.
float readThermometer(Ds18b20Config config, uint16_t sensorId, bool save)
{
	// TODO multiple readings
	DallasTemperature sensors = getThermometer(config);
	sensors.requestTemperatures(); // Send the command to get temperatures
	float thermometers = sensors.getTempCByIndex(sensorId);
	// TODO multiple reads on bus
	if (save)
	{
		saveThermometerValue(config, thermometers);
	}
	return thermometers;
}
