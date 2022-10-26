#include "mqtt.h"
#include "power.h"
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
    mqtt.subscribe((char *)("config/" + clientId).c_str(), 1);
}

void loopMqtt()
{
    mqtt.loop();
}

void destroyMqtt()
{
    mqtt.disconnect();
}

void  sendMessage()
{
    if (mqtt.connected())
    {
        char msg[512];
        char topic[256];
        StaticJsonDocument<JSON_BUFFER_SIZE> doc;
        doc["host"] = Config.name;
        doc["grams"] = State.grams;
        doc["pieces"] = State.pieces;
        doc["battery"] = State.battery;
        doc["configured"] = State.configured;
        doc["temperature"] = State.temperature;
        doc["charging"] = State.charging;
        serializeJson(doc, msg);
        snprintf(topic, 256, "device/%s/data", Config.name);
        mqtt.publish(topic, msg, false);
    }
}
void sendDebugMessage()
{
    if(mqtt.connected())
    {
        char msg[512];
        char topic[256];
        StaticJsonDocument<JSON_BUFFER_SIZE> doc;
        doc["host"] = Config.name;
        JsonObject adc = doc.createNestedObject("adc");
        adc["readings"] = Debug.adc[0].readings;
        adc["pre_offset"] = Debug.adc[0].pre_offset;
        adc["pre_multi"] = Debug.adc[0].pre_multi;
        JsonObject hx711 = doc.createNestedObject("hx711");
        hx711["readings"] = Debug.hx711.readings;
        hx711["gain"] = Debug.hx711.gain;
        hx711["pre_offset"] = Debug.hx711.pre_offset;
        hx711["pre_multi"] = Debug.hx711.pre_multi;
        JsonObject ds18b20 = doc.createNestedObject("dx18b20");
        ds18b20["readings"] = Debug.ds18b20.readings;
        ds18b20["pre_offset"] = Debug.ds18b20.pre_offset;
        ds18b20["pre_multi"] = Debug.ds18b20.pre_multi;
        doc["configured"] = Debug.configured;
        serializeJson(doc, msg);
        snprintf(topic, 256, "device/%s/debug", Config.name);
        mqtt.publish(topic, msg, false);
    }
}

void ParseGPIOConfig(StaticJsonDocument<JSON_BUFFER_SIZE> doc)
{
    Serial.println("Parsing GPIO config");
    JsonArray array = doc["gpio"].as<JsonArray>();

    if (array.size() > MAX_GPIO_PINS)
    {
        Serial.println("Too many pins provided in GPIO config!");
        return;
    }

    for (uint i = 0; i < array.size(); i++)
    {
        Config.gpio[i].name = array[i]["name"];
        Config.gpio[i].defaultValue = array[i]["default"];
        Config.gpio[i].inverted = array[i]["invert"];
        Config.gpio[i].mode = array[i]["mode"];
        Config.gpio[i].pin = array[i]["pin"];
        Config.gpio[i].configured = 1;
    }
}
void ParseADCConfig(StaticJsonDocument<JSON_BUFFER_SIZE> doc)
{
    Serial.println("Parsing ADC config");
    JsonArray array = doc["adc"].as<JsonArray>();

    if (array.size() > MAX_ADC_PINS)
    {
        Serial.println("Too many pins provided in ADC config!");
        return;
    }

    for (uint i = 0; i < array.size(); i++)
    {
        Config.adc[i].name = array[i]["name"];
        Config.adc[i].readingsForMean = array[i]["readings"];
        Config.adc[i].offset = array[i]["offset"];
        Config.adc[i].multiplier = array[i]["multi"];
        Config.adc[i].pin = array[i]["pin"];
        Config.adc[i].configured = 1;
    }
}
/// @brief Gets first JSon object from array. Can be used  only if there is a single element.
/// @param deviceName
/// @param doc
/// @return
JsonObject getDeviceConfig(char *deviceName, StaticJsonDocument<JSON_BUFFER_SIZE> doc)
{
    JsonObject json = doc[deviceName].as<JsonArray>()[0];
    return json;
}

void parseHx711Config(StaticJsonDocument<JSON_BUFFER_SIZE> doc)
{
    Serial.println("Parsing scale sensor config");
    JsonArray array = doc["hx711"].as<JsonArray>();
    JsonObject json = array[0]; // todo parse multiple scales

    Config.scale.name = json["name"];
    Config.scale.pin_sck = json["pin_sck"];
    Config.scale.pin_dt = json["pin_dt"];
    Config.scale.gain = json["gain"]; 
    Config.scale.offset = json["offset"];
    Config.scale.multi = json["multi"];
}
void parseDs18b20Config(StaticJsonDocument<JSON_BUFFER_SIZE> doc)
{
    Serial.println("Parsing tempreture sensor config");
    JsonArray array = doc["ds18b20"].as<JsonArray>();
    JsonObject json = array[0]; // todo parse multiple temp sensors
    Serial.println("Array init");

    Config.temperature.name = json["name"];
    Config.temperature.pin = json["pin"];
    Config.temperature.offset = json["offset"];
    Config.temperature.multi = json["multi"];
    Serial.println("Values assigned");

}

void configCallback(char *topic, byte *payload, unsigned int length)
{
    if(State.configured == 1){
        Serial.println("New config received. Restart");
        restart();
    }
    Serial.println("Received from topic");
    StaticJsonDocument<JSON_BUFFER_SIZE> doc;
    DeserializationError error = deserializeJson(doc, payload);
    if (error)
    {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        return;
    }
    
    // Get hostname
    strcpy(Config.name, doc["host"]);
    //strcpy(Debug.host, doc["host"]);
    // Get gpio config
    ParseGPIOConfig(doc);

    ParseADCConfig(doc);

    parseHx711Config(doc);
   
    parseDs18b20Config(doc);

    State.configured = 1; 
}
