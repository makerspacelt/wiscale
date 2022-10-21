#include <Arduino.h>
#include "Ticker.h"
#include "HX711.h"
#include "mqtt.h"
#include <OneWire.h>
#include <DallasTemperature.h>

#define DOUT  4 //D2
#define CLK  5 //D1
HX711 scale;
#define MCU_DONE_PIN  12 //D6
#define CHARGING 13 //D7
#define BAT 17 // A0
#define ONE_WIRE_BUS 14 //D5

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

Ticker timer;

void initScale() {
    scale.begin(
        Config.scale.pin_dt, 
        Config.scale.pin_sck,
        Config.scale.gain
        );
    scale.power_up();

    scale.set_scale(Config.scale.multi);
    scale.set_offset(Config.scale.multi);
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
    State.grams = scale.get_units(30);
}
void readBattery() {
    //  (bat)----[180k]----[220k]--(A0)--[100k]----(GND)
    State.battery = map(analogRead(BAT), 0, 1024, 0, Config.battery_range)/1000.0;
}
void readTemperature(){
      sensors.requestTemperatures(); // Send the command to get temperatures
      State.temperature= sensors.getTempCByIndex(0);
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
void ReconfigureGPIO(){
    for (uint32_t i = 0; i < MAX_GPIO_PINS; i++)
    {
        if (Config.gpio[i].configured)
        {
          pinMode(Config.gpio[i].pin,Config.gpio[i].mode);
        }
    }
}

void ReconfigureADC(){
    for (uint32_t i = 0; i < MAX_ADC_PINS; i++)
    {
        if (Config.adc[i].configured)
        {
          pinMode(Config.adc[i].pin,INPUT);
        }
    }
}
void ReconfigureTemperature(){
        // Start up tempreture
    sensors.begin();
}
void Reconfigure(){
    Serial.println("Reconfiguring");
    ReconfigureTemperature();
    initScale();
}
void PowerDown(){
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