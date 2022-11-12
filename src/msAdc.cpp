#include "global.h"
#include "msAdc.hpp"

MS_ADC::MS_ADC(void){

}
MS_ADC::MS_ADC(struct ADCConfig config){
    initAdc(config);
}

void MS_ADC::initAdc(struct ADCConfig adcConfig){
    Serial.println("Initializing adc");
	config = adcConfig;
	voltage = 0;
}

float MS_ADC::readAdc(bool save){
    long sum = 0;
	for (byte i = 0; i < config.readings; i++) {
		sum += analogRead(config.pin);
		delay(0);
	}
    float _voltage = sum / config.readings;
    debug.pre_offset = _voltage;
    _voltage += config.offset;
    debug.pre_multi = _voltage;
    _voltage *= config.multi;
    if(save){
        voltage = _voltage;
    }
    return _voltage;
}