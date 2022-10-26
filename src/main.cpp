#include <Arduino.h>
#include "Ticker.h"
#include "mqtt.h"
#include "power.h"
#include "adc.h"

#include "gpio.h"
#include "scale.h"
#include "temperature.h"

#define MCU_DONE_PIN  12 //D6
#define CHARGING 13 //D7
#define BAT 17 // A0



Ticker timer;

void ReadScales(){
    for (uint16_t i = 0; i < USED_SCALES; i++)
    {
        MS_HX711_Scale ms_scale=getMSScale(Config.scales[i]);
        // Read and save to ms_scale
        readScale(ms_scale.config,true);
        State.scales[i]=ms_scale;
    }    
}
void PrintScaleValues()
{
    
    for (uint16_t i = 0; i < USED_SCALES; i++)
    {
        char* scaleInfo;
        MS_HX711_Scale ms_scale = getMSScale(Config.scales[i]);
        // Read and save to ms_scale
        readScale(ms_scale.config, true);
        sprintf(scaleInfo,"Scale name: %s, value: %3.2f",ms_scale.config.name,ms_scale.grams);
        Serial.println(scaleInfo);
    }
}

void initWifi() {
    //Serial.setDebugOutput(true);
    //system_phy_set_powerup_option(3);
    #ifdef CONFIGURE_WIFI
        WiFi.persistent(true);
        WiFi.mode(WIFI_STA);
        WiFi.begin(WIFI_SSID, WIFI_PASS);
    #endif
}

// ===============================================

void setup() {
    pinMode(D0,INPUT); // Workaround for using wrong pin.
    Serial.begin(76800);
    Serial.println("\nBooting... ");

    timer.attach(EXECUTION_TIMEOUT, selfDestruct);
    pinMode(MCU_DONE_PIN, OUTPUT);
    digitalWrite(MCU_DONE_PIN, LOW);
    
    initWifi();
 }


void Reconfigure(){
    Serial.println("Reconfiguring");
    ReconfigureTemperature();
    initScales(Config.scales);
}
void PowerDown(){
    deInitScales();
    Serial.print("Shutting down: ");
    for (int i=0; i<10; i++) {
        loopMqtt();
        delay(10);
        Serial.print(".");
    }
    Serial.println();

    destroyMqtt();
    delay(10);
    selfDestruct();
}
void loop() {

    Serial.print("Wifi connecting: ");
    while ( WiFi.status() != WL_CONNECTED ) {
        delay(10);
        Serial.print(".");
    }
    Serial.println();

    Serial.println("Connecting to MQTT broker");
    setupMqtt();

    Serial.print("Waiting for configuration: ");
    while ( State.configured == 0 ) {
        delay(10);
        loopMqtt();
        Serial.print(".");
    }    
    Reconfigure();

    ReadScales();
    readBattery();
    readTemperature();

    PrintScaleValues();
    Serial.printf("Sending message: %.3fV, %.3f C\n", State.battery, State.temperature);
    sendMessage();

    PowerDown();
}