#include "temperature.h"
void readTemperature()
{
	sensors.requestTemperatures(); // Send the command to get temperatures
	State.temperature = sensors.getTempCByIndex(0);
}
void ReconfigureTemperature()
{
	// Start up tempreture
	sensors.begin();
}