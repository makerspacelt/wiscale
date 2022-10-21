#include "gpio.h"

uint8_t ReadGPIO(uint8_t pin){
	return digitalRead(pin);
}
uint8_t ReadGPIO(char*  pinName){
	for (uint8_t i = 0; i < MAX_GPIO_PINS; i++)
	{
		if (Config.gpio[i].name==pinName)
        {
          return digitalRead(Config.gpio[i].pin);
	}
	Serial.println("GPIO pin not found");
	return 0;
}
}
void ReconfigureGPIO()
{
    for (uint32_t i = 0; i < MAX_GPIO_PINS; i++)
    {
        if (Config.gpio[i].configured)
        {
          pinMode(Config.gpio[i].pin,Config.gpio[i].mode);
        }
    }
}