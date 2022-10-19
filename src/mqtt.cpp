#include "mqtt.h"
#include "ArduinoJson.h"

WiFiClient espClient;
PubSubClient mqtt(espClient);


void setupMqtt()
{
    String clientId = WiFi.hostname();
    mqtt.setServer("acs.lan", 1883);
    mqtt.setCallback(configCallback);
    mqtt.connect(clientId.c_str());
    mqtt.subscribe((char*)("config/"+clientId).c_str(), 1);
}

void loopMqtt() {
    mqtt.loop();
}

void destroyMqtt() {
    mqtt.disconnect();
}

void sendMessage()
{
    if (mqtt.connected()) {
        char msg[128];
        char topic[64];
        sprintf(msg
            , "{\"host\":\"%s\",\"grams\":%f,\"pieces\":%d,\"battery\":%f,\"configured\":%d}"
            , config.name
            , state.grams
            , state.pieces
            , state.battery
            , state.configured
        );
        sprintf(topic, "scale/%s/data", config.name);
        mqtt.publish(topic, msg, true);
    }
}

void configCallback(char* topic, byte* payload, unsigned int length) {
    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, payload);
    if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        return;
    }
    config.name = doc["name"];
    config.battery_range = doc["battery_range"];
    config.scale_zero = doc["scale_zero"];
    config.scale_cal = doc["scale_cal"];
    config.piece_grams = doc["piece_grams"];
    state.configured = 1;
}

