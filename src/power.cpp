#include <Arduino.h>
#include "power.h"
#define MCU_DONE_PIN  12 //D6

void selfDestruct() {
    Serial.println("Killing power...");
    pinMode(MCU_DONE_PIN, OUTPUT);
    digitalWrite(MCU_DONE_PIN, HIGH);
    delay(100);
    Serial.println("Going back to sleep.");
    ESP.deepSleep(60e6);
}

void restart(){
    Serial.println("ESP restart initiated.");
    ESP.restart();
}