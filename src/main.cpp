#include <Arduino.h>
#include "Ticker.h"
#include "mqtt.h"
#include "power.h"
#include "adc.h"

#include "gpio.h"
#include "scale.h"
#include "temperature.h"
#include "ConfigManager.h"

#define MCU_DONE_PIN  12 //D6
#define CHARGING 13 //D7
#define BAT 17 // A0

struct Config { 
    char name[20]; 
    char password[20] = "pawa2500";
} config;
ConfigManager configManager;
Ticker timer;

void ReadScales(){
    for (uint16_t i = 0; i < USED_SCALES; i++)
    {
        MS_HX711_Scale sensor=getMSScale(Config.scales[i]);
        // Read and save to ms_scale
        readScale(sensor.config,true);
        State.scales[i]=sensor;
    }    
}
void ReadThermometers(){
    for (uint16_t i = 0; i < USED_TEMPERATURE_SENSORS; i++)
    {
        MS_Ds18b20 sensor=getMSThermometer(Config.thermometers[i]);
        // Read and save to ms_scale
        readThermometer(sensor.config,0,true);
        State.thermometers[i]=sensor;
    }    
}
void PrintScaleValues()
{
    
    for (uint16_t i = 0; i < USED_SCALES; i++)
    {
        char scaleInfo[30];
        MS_HX711_Scale ms_scale = getMSScale(Config.scales[i]);
        // Read and save to ms_scale
        readScale(ms_scale.config, true);
        sprintf(scaleInfo,"Scale name: %s, value: %3.2f",ms_scale.config.name,ms_scale.grams);
        Serial.println(scaleInfo);
    }
}

// ===============================================

void setup() {
    pinMode(D0,INPUT); // Workaround for using wrong pin.
    Serial.begin(76800);
    Serial.println("\nBooting... ");

 //   timer.attach(EXECUTION_TIMEOUT, selfDestruct);
    pinMode(MCU_DONE_PIN, OUTPUT);
    digitalWrite(MCU_DONE_PIN, LOW);

    configManager.setAPName(WiFi.hostname().c_str());
    configManager.setAPFilename("/index.html");
    if(strlen(config.password) > 0)
    {
        configManager.setAPPassword(config.password);
    }
    configManager.addParameter("name", config.name, 20);
    configManager.addParameter("password", config.password, 20, set);
    configManager.begin(config);
 }


void Reconfigure(){
    Serial.println("Reconfiguring");
    initThermometers(Config.thermometers);
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

    configManager.loop();
    Serial.print("Wifi not configured, waiting: ");
    while ( WiFi.status() != WL_CONNECTED ) {
        delay(10);
        configManager.loop();
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
    ReadThermometers();

    PrintScaleValues();
    //Serial.printf("Sending message: %.3fV, %.3f C\n", State.battery, State.thermometers);
    sendMessage();

    PowerDown();
}