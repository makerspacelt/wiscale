#include <Arduino.h>
#include "Ticker.h"
#include "mqtt.h"
#include "adc.h"

#include "gpio.h"
#include "scale.h"
#include "temperature.h"

#define MCU_DONE_PIN  12 //D6
#define CHARGING 13 //D7
#define BAT 17 // A0



Ticker timer;


void initWifi() {
    //Serial.setDebugOutput(true);
    //system_phy_set_powerup_option(3);
    #ifdef CONFIGURE_WIFI
        WiFi.persistent(true);
        WiFi.mode(WIFI_STA);
        WiFi.begin(WIFI_SSID, WIFI_PASS);
    #endif
}



void selfDestruct() {
    Serial.println("Killing power...");
    pinMode(MCU_DONE_PIN, OUTPUT);
    digitalWrite(MCU_DONE_PIN, HIGH);
    delay(100);
    Serial.println("Going back to sleep.");
    ESP.deepSleep(60e6);
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
    initScale();
}
void PowerDown(){
    powerDownScale();
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

    readScale();
    readBattery();
    readTemperature();

    Serial.printf("Sending message: %.3fg; %.3fV, %.3f C\n", State.grams, State.battery, State.temperature);
    sendMessage();

    PowerDown();
}