#include "Ticker.h"
#include "adc.h"
#include "mqtt.h"
#include "power.h"
#include <Arduino.h>

#include "deviceState.hpp"
#include "gpio.h"
#include "ConfigManager.h"
#include "msTemperature.hpp"

#define MCU_DONE_PIN  12 //D6
#define CHARGING 13 //D7
#define BAT 17 // A0

struct Config { 
    char name[20]; 
    char password[20];
} config;
ConfigManager configManager;

Ticker timer;

void PrintScaleValues()
{

    for (uint16_t i = 0; i < USED_SCALES; i++)
    {
        char scaleInfo[50];
        MS_HX711_Scale ms_scale = State.Scales[i];
        // Read and save to ms_scale
        sprintf(scaleInfo, "Scale name: %s, value: %3.2f", ms_scale.config.name, ms_scale.grams);
        Serial.println(scaleInfo);
    }
}

// ===============================================

void setup()
{
    pinMode(D0, INPUT); // Workaround for using wrong pin.
    Serial.begin(76800);
    Serial.println("\nBooting... ");

 //   timer.attach(EXECUTION_TIMEOUT, selfDestruct);
    pinMode(MCU_DONE_PIN, OUTPUT);
    digitalWrite(MCU_DONE_PIN, LOW);
    
    char *wifiName = (char *)"";
    sprintf(wifiName, "Scale %s AP", WiFi.hostname().c_str());

    configManager.setAPName(wifiName);
    configManager.setAPPassword("adminadmin");
    configManager.setAPFilename("/index.html");
    configManager.addParameter("name", config.name, 20);
    configManager.addParameter("password", config.password, 20, set);
    configManager.begin(config);
 }

void Reconfigure()
{
    Serial.println("Reconfiguring");
#ifdef USE_DS18
    State.ReconfigureThermometers(Config.thermometers);
#endif // USE_DS18
#ifdef USE_SCALE
    State.ReconfigureScales(Config.scales);
#endif // USE_SCALE
}
void PowerDown()
{
    Serial.print("Shutting down: ");
    for (int i = 0; i < 10; i++)
    {
        loopMqtt();
        delay(10);
        Serial.print(".");
    }
    Serial.println();

    destroyMqtt();
    delay(10);
    selfDestruct();
}
void loop()
{

    configManager.loop();
    Serial.print("Wifi not configured, waiting: ");
    while ( WiFi.status() != WL_CONNECTED ) {
        delay(10);
        configManager.loop();
        Serial.print(".");
    }
    Serial.println();
    Serial.println(WiFi.localIP().toString());
    Serial.println("Connecting to MQTT broker");
    setupMqtt();

    Serial.print("Waiting for configuration: ");
    while (State.Configured == 0)
    {
        delay(10);
        loopMqtt();
        Serial.print(".");
    }
    Reconfigure();

    // Get gpio config
#ifdef USE_GPIO
//
#endif // USE_GPIO

#ifdef USE_ADC

    readBattery();
#endif // USE_ADC

#ifdef USE_SCALE

    State.ReadScales();
#endif // USE_SCALE

#ifdef USE_DS18
    State.ReadThermometers();
#endif // USE_DS18

    PrintScaleValues();
    Serial.printf("Sending message: %.3fV, %.3f C\n", State.Battery, State.Thermometers[0].temperature);
    sendMessage();

   // PowerDown();
}