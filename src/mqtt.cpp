#include "mqtt.h"
#include "power.h"
#include "ArduinoJson.h"
#include "scale.h"

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

const char *getScaleData()
{

    // compute the required size
    const size_t CAPACITY = JSON_ARRAY_SIZE(USED_SCALES);

    // allocate the memory for the document
    StaticJsonDocument<CAPACITY> doc;

    // create an empty array
    JsonArray array = doc.to<JsonArray>();

    JsonObject scaleObject;
    // add some values
    for (uint8_t i = 0; i < USED_SCALES; i++)
    {
        array.add(getScaleJObject(State.scales[i]));
    }

    // serialize the object and send the result to Serial
    const char *json_string;
    serializeJson(doc, json_string);
    return json_string;
}

const char *getScaleJObject(MS_HX711_Scale scale)
{
    // allocate the memory for the document
    const size_t CAPACITY = JSON_OBJECT_SIZE(1);
    StaticJsonDocument<CAPACITY> doc;
    // create an object
    JsonObject object = doc.to<JsonObject>();
    object[scale.config.name] = scale.grams;

    // serialize the object and send the result to Serial
    const char *json_string;
    serializeJson(doc, json_string);
    return json_string;
}
void sendMessage()
{
    if (mqtt.connected())
    {
        char msg[254];
        char topic[64];
        sprintf(msg, "{\"host\":\"%s\",\"scales\":%s,\"battery\":%f,\"configured\":%d,\"temperature\":%f,\"charging\":%d}", Config.name, getScaleData(), State.battery, State.configured, State.temperature, State.charging);
        sprintf(topic, "scale/%s/data", Config.name);
        mqtt.publish(topic, msg, true);
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

    for (uint8_t i = 0; i < array.size(); i++)
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

    for (uint8_t i = 0; i < array.size(); i++)
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

    Config.temperature.name = json["name"];
    Config.temperature.pin = json["pin"];
    Config.temperature.offset = json["offset"];
    Config.temperature.multi = json["multi"];
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
    Config.name = doc["host"];

    // Get gpio config
    ParseGPIOConfig(doc);

    ParseADCConfig(doc);

    parseHx711Config(doc);
   
    parseDs18b20Config(doc);

    State.configured = 1; 
}
