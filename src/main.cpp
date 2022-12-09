#include <Arduino.h>
#include "mqtt.h"
#include <WiFiSettings.h>
#include <LittleFS.h>

#define MCU_DONE_PIN 12 // D6
unsigned long lastRead = 0;

void setup(){
    pinMode(D0, INPUT); // Workaround for using wrong pin.
    Serial.begin(76800);
    Serial.println("\nBooting... ");
    LittleFS.begin();

    char apName[11];
    sprintf(apName, "ESP-%06X", ESP.getChipId());

    WiFiSettings.hostname = apName;
    WiFiSettings.connect();
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(10);
    }
    initMqtt();
    while(!DeviceState.IsConfigured){
        loopMqtt();
        delay(10);
    }
}
void loop(){
    loopMqtt();
#ifdef USE_GPIO
    DeviceState.ReadGPIOs();
#endif
#ifdef USE_DS18
    DeviceState.ReadTemperatures();
#endif
#ifdef USE_ADC
    DeviceState.ReadADCs();
#endif
#ifdef USE_SCALE
    DeviceState.ReadWeights();
#endif

    if(millis() - lastRead > DeviceState.loopDelay){
        lastRead = millis();
#ifdef USE_GPIO
        publishGPIOData();
#endif
#ifdef USE_DS18
        publishTemperatureData();
#endif
#ifdef USE_ADC
        publishADCData();
#endif
#ifdef USE_SCALE
        publishScalesData();
#endif
        publishDebugInfo();
        loopMqtt();
        finishPublishing();
        //DeviceState.killMyself();
    }
}