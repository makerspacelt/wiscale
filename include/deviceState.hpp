#pragma once
#include "msScale.hpp"
#include "msTemperature.hpp"
#include "msAdc.hpp"

class DeviceState
{

public:
	MS_HX711_Scale Scales[USED_SCALES];
	MS_ADC Adcs[USED_ADC_PINS];
	MS_Ds18b20 Thermometers[USED_TEMPERATURE_SENSORS];
	int Charging = 0;
	int Configured = 0;

	DeviceState(); // This is the constructor
	void ReconfigureScales(Hx711Config scaleConfigs[]);
	void ReconfigureThermometers(Ds18b20Config thermometerConfigs[]);
	void ReconfigureAdc(ADCConfig configs[]);
	void ReadScales(void);
	void ReadThermometers(void);
	void ReadAdc(void);

private:
	// Initializes MS_HX711_Scale objects
	void initializeScales(void);
	void initializeThermometers(void);
	void initializeAdcs(void);
};
