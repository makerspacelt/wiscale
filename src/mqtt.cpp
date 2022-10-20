#include "mqtt.h"
#include "ArduinoJson.h"

WiFiClient espClient;
PubSubClient mqtt(espClient);


void setupMqtt()
{
    String clientId = WiFi.hostname();
    mqtt.setServer("broker.lan", 1883);
    mqtt.setCallback(configCallback);
    mqtt.setBufferSize(2048); // must set to larger, as by default it is limited to 256
    mqtt.connect(clientId.c_str());
    Serial.println("My ID");
    Serial.println(clientId);
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
        char msg[254];
        char topic[64];
        sprintf(msg
            , "{\"host\":\"%s\",\"grams\":%f,\"pieces\":%d,\"battery\":%f,\"configured\":%d,\"temperature\":%f,\"charging\":%d}"
            , Config.name
            , State.grams
            , State.pieces
            , State.battery
            , State.configured
            , State.temperature
            , State.charging
        );
        sprintf(topic, "scale/%s/data", Config.name);
        mqtt.publish(topic, msg, true);
    }
}

void ParseGPIOConfig(StaticJsonDocument<JSON_BUFFER_SIZE> doc){
    
   JsonArray array=doc["gpio"].as<JsonArray>();

   if (array.size()>MAX_GPIO_PINS)
   {
    Serial.println("Too many pins provided in GPIO config!");
    return;
   }
   
   for (int i = 0; i < array.size(); i++)
   {
     uint8_t pin=array; // todo figure out how to get key
     Config.gpio->name=array[i]["name"];
     Config.gpio->defaultValue=array[i]["deefault"];
     Config.gpio->inverted=array[i]["invert"];
     Config.gpio->mode=array[i]["mode"];
     Config.gpio->pin=pin;          
   }  
    
}


void configCallback(char* topic, byte* payload, unsigned int length) {
    Serial.println("Received from topic");
    StaticJsonDocument<JSON_BUFFER_SIZE> doc;
    DeserializationError error = deserializeJson(doc, payload);
    if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        return;
    }
    // Get hostname
    Config.name = doc["host"];

    // Get gpio config
    ParseGPIOConfig(doc);

    Config.battery_range = doc["battery_range"];
    Config.scale_zero = doc["scale_zero"];
    Config.scale_cal = doc["scale_cal"];
    Config.scale_gain = doc["scale_gain"];
    Config.piece_grams = doc["piece_grams"];
    Config.led_pin = doc["led"];
    State.configured = 1;
}

