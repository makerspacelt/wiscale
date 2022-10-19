#include <Arduino.h>
#include "Ticker.h"
#include "HX711.h"
#include "mqtt.h"

#define DOUT  D4
#define CLK  D3
HX711 scale;
#define DONE  D6

Ticker timer;

void initScale() {
    scale.begin(DOUT, CLK);
    scale.power_up();

    scale.set_scale(config.scale_cal);
    scale.set_offset(config.scale_zero);
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

void readScale() {
    state.grams = scale.get_units(30);
    state.pieces = ((config.piece_grams/2)+state.grams)/config.piece_grams;
}
void readBattery() {
    //  (bat)----[180k]----[220k]--(A0)--[100k]----(GND)
    state.battery = map(analogRead(A0), 0, 1024, 0, config.battery_range)/1000.0;
}
void selfDestruct() {
    Serial.println("Killing power...");
    pinMode(DONE, OUTPUT);
    digitalWrite(DONE, HIGH);
    delay(100);
    Serial.println("Going back to sleep.");
    ESP.deepSleep(60e6);
}
// ===============================================

void setup() {
    Serial.begin(76800);
    Serial.println("\nBooting... ");

    timer.attach(EXECUTION_TIMEOUT, selfDestruct);
    pinMode(D6, OUTPUT);
    digitalWrite(D6, LOW);

    initWifi();
    initScale();
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
    while ( state.configured == 0 ) {
        delay(10);
        loopMqtt();
        Serial.print(".");
    }
    Serial.println();

    readScale();
    readBattery();

    Serial.printf("Sending message: %.3fg; %.3fV\n", state.grams, state.battery);
    sendMessage();
    scale.power_down();

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