#include "Ticker.h"
#include <Arduino.h>
#include "ConfigManager.h"
#include "mqtt.h"

#define MCU_DONE_PIN  12 //D6

struct Config { 
    char name[20]; 
    char password[20];
} config;
ConfigManager configManager;

Ticker timer;

void setup()
{
    pinMode(D0, INPUT); // Workaround for using wrong pin.
    Serial.begin(76800);
    Serial.println("\nBooting... ");

 //   timer.attach(EXECUTION_TIMEOUT, selfDestruct);
    pinMode(MCU_DONE_PIN, OUTPUT);
    digitalWrite(MCU_DONE_PIN, LOW);
    
    char *wifiName = (char *)"";
    sprintf(wifiName, "ESP-%06X", ESP.getChipId());

    configManager.setAPName(wifiName);
    configManager.setAPPassword("adminadmin");
    configManager.setAPFilename("/index.html");
    configManager.addParameter("name", config.name, 20);
    configManager.addParameter("password", config.password, 20, set);
    configManager.begin(config);
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
    initMqtt();

    Serial.print("Waiting for configuration: ");
    // while (State.IsConfigured == 0)
    // {
    //     delay(10);
    //    
    //     Serial.print(".");
    // }
    loopMqtt();
}