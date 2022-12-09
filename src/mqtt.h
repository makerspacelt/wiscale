#ifndef MQTT_H
#define MQTT_H

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "global.h"

void initMqtt(char *deviceName = "");
void loopMqtt();
void destroyMqtt();
void configCallback(char *topic, byte *payload, unsigned int length);
void publishGPIOData();
void publishTemperatureData();
void publishADCData();
void publishScalesData();
void publishDebugInfo();
void finishPublishing();
#endif