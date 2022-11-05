#pragma once
#include "msScale.hpp"
#include "msTemperature.hpp"

class DeviceState
{

public:
	MS_HX711_Scale Scales[USED_SCALES];
	float Battery = 0;
	MS_Ds18b20 Thermometers[USED_TEMPERATURE_SENSORS];
	int Charging = 0;
	int Configured = 0;

	DeviceState(); // This is the constructor
	void ReconfigureScales(Hx711Config scaleConfigs[]);
	void ReconfigureThermometers(Ds18b20Config thermometerConfigs[]);
	void ReadScales(void);
	void ReadThermometers(void);

private:
	// Initializes MS_HX711_Scale objects
	void initializeScales(void);
	void initializeThermometers(void);
};