#include "temperature.h"
// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);
void readTemperature()
{
	long sum = 0;
	for (byte i = 0; i < Config.temperature.readings; i++) {
		sensors.requestTemperatures(); // Send the command to get temperatures
		sum += sensors.getTempCByIndex(0);
		delay(0);
	}
	float temperature = sum / Config.temperature.readings;
	Debug.ds18b20.pre_offset = temperature;
	temperature += Config.temperature.offset;
	Debug.ds18b20.pre_multi = temperature;
	temperature *= Config.temperature.multi;
	State.temperature = temperature;
}
void ReconfigureTemperature()
{
	// Start up tempreture
	sensors.begin();
}