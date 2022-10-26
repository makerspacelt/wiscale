#include "adc.h"

uint16_t ReadADC(uint8_t pin){
		return analogRead(pin);
}
void readBattery() {
    //  (bat)----[180k]----[220k]--(A0)--[100k]----(GND)
    long sum = 0;
	for (byte i = 0; i < Config.adc[0].readings; i++) {
		sum += analogRead(Config.adc[0].pin);
		delay(0);
	}
	float batteryAvg = sum / Config.adc[0].readings;
    Debug.adc[0].pre_offset = batteryAvg;
    batteryAvg += Config.adc[0].offset;
    Debug.adc[0].pre_multi = batteryAvg;
    batteryAvg *= Config.adc[0].multiplier;
    State.battery = batteryAvg;
}
void ReconfigureADC(){
    for (uint32_t i = 0; i < MAX_ADC_PINS; i++)
    {
        if (Config.adc[i].configured)
        {
          pinMode(Config.adc[i].pin,INPUT);
        }
    }
}