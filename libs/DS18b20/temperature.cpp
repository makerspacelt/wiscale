#include "temperature.h"
// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);
void readTemperature()
{
	sensors.requestTemperatures(); // Send the command to get temperatures
	State.temperature = sensors.getTempCByIndex(0);
}


Ds18b20Config readTemperature(uint16_t sensorId){
	float temperature== sensors.getTempCByIndex(0);
	Ds18b20Config readings={};

}
Ds18b20Config[] readAllTemperatureSensors();



void ReconfigureTemperature()
{
	// Start up tempreture
	sensors.begin();
}