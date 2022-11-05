#include "Ticker.h"
#include "adc.h"
#include "mqtt.h"
#include "power.h"
#include <Arduino.h>

#include "deviceState.hpp"
#include "gpio.h"
#include "msTemperature.hpp"

#define MCU_DONE_PIN 12 // D6
#define CHARGING 13     // D7
#define BAT 17          // A0

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

void initWifi()
{
// Serial.setDebugOutput(true);
// system_phy_set_powerup_option(3);
#ifdef CONFIGURE_WIFI
    WiFi.persistent(true);
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASS);
#endif
}

// ===============================================

void setup()
{
    pinMode(D0, INPUT); // Workaround for using wrong pin.
    Serial.begin(76800);
    Serial.println("\nBooting... ");

    // timer.attach(EXECUTION_TIMEOUT, selfDestruct);
    pinMode(MCU_DONE_PIN, OUTPUT);
    digitalWrite(MCU_DONE_PIN, LOW);

    initWifi();
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

    Serial.print("Wifi connecting: ");
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(10);
        Serial.print(".");
    }
    Serial.println();

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

    PowerDown();
}