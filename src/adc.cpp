#include "adc.h"

uint16_t ReadADC(uint8_t pin){
		return analogRead(pin);
}
void readBattery() {
    //  (bat)----[180k]----[220k]--(A0)--[100k]----(GND)
    State.battery = map(analogRead(Config.adc[0].pin), 0, 1024, 0, Config.battery_range)/1000.0;
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