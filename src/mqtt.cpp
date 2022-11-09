#include "mqtt.h"
#include "ArduinoJson.h"
#include "power.h"

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
String getTemperatureData()
{
    // TODO get thermometers data
    return "";
}
char *getTemperatureJObject(MS_HX711_Scale scale)
{
    // TODO get thermometers object
    return "";
}
String getScaleJObject(MS_HX711_Scale scale)
{
    // allocate the memory for the document
    const size_t CAPACITY = JSON_OBJECT_SIZE(2);
    StaticJsonDocument<CAPACITY> doc;
    // create an object
    JsonObject object = doc.to<JsonObject>();
    object[scale.config.name] = scale.grams;

    // serialize the object and send the result to Serial
    String json_string;
    serializeJson(doc, json_string);
    serializeJson(doc, Serial);
    return json_string;
}
String getScaleData(MS_HX711_Scale scales[])
{
      // allocate the memory for the document
    DynamicJsonDocument doc(2048);
    
    // create an empty array
    JsonArray array = doc.to<JsonArray>();

    JsonObject scaleObject;
    // add some values
    for (uint8_t i = 0; i < USED_SCALES; i++)
    {
        array.add(getScaleJObject(scales[i]));
    }

    // serialize the object and send the result to Serial
    String json_string;
    serializeJson(doc, json_string);
    return json_string;
}
String getMqttMsg(){
  // allocate the memory for the document
    DynamicJsonDocument doc(2048);

    JsonObject object = doc.to<JsonObject>();
    object["host"] = Config.name;
    object["scales"] = getScaleData(State.Scales);
    object["battery"]= State.Battery;
    object["configured"]=State.Configured;
    object["temperatures"]=getTemperatureData();
    object["charging"]=State.Charging;

   String json_string;
    serializeJson(doc, json_string);
    serializeJson(doc, Serial);
    return json_string;
}
void sendMessage()
{
    if (mqtt.connected())
    {
        char msg[254];
        char topic[64];
        sprintf(topic, "scale/%s/data", Config.name);
        mqtt.publish(topic, getMqttMsg().c_str(), true);
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
        const char *name = array[i]["name"];
        strcpy(Config.gpio[i].name, name);
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
        const char *name = array[i]["name"];
        strcpy(Config.adc[i].name, name);
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
    char msg[5];
    sprintf(msg, "Found scale sensors %d", array.size());
    Serial.println(msg);
    for (uint8_t i = 0; i < array.size(); i++)
    {
        JsonObject json = array[i];
        Hx711Config scaleConfig = {};
        const char *name = json["name"];
        Serial.println(name);
        strcpy(scaleConfig.name, name);
        scaleConfig.pin_sck = json["pin_sck"];
        scaleConfig.pin_dt = json["pin_dt"];
        scaleConfig.gain = json["gain"];
        scaleConfig.offset = json["offset"];
        scaleConfig.multi = json["multi"];
        scaleConfig.readings = json["readings"];
        Config.scales[i] = scaleConfig;
    }
}
void parseDs18b20Config(StaticJsonDocument<JSON_BUFFER_SIZE> doc)
{
    Serial.println("Parsing tempreture sensor config");
    JsonArray array = doc["ds18b20"].as<JsonArray>();
    char msg[5];
    sprintf(msg, "Found temperature sensors%d", array.size());
    Serial.println(msg);
    for (uint8_t i = 0; i < array.size(); i++)
    {
        JsonObject json = array[i];
        Ds18b20Config thermometerConfig = {};
        const char *name = json["name"];
        Serial.println(name);
        strcpy(thermometerConfig.name, name);
        thermometerConfig.pin = json["pin"];
        thermometerConfig.offset = json["offset"];
        thermometerConfig.multi = json["multi"];
        Config.thermometers[i] = thermometerConfig;
    }
}

void configCallback(char *topic, byte *payload, unsigned int length)
{
    if (State.Configured == 1)
    {
        Serial.println("New config received. Restart");
        restart();
    }
    Serial.println("Received from topic");
    StaticJsonDocument<JSON_BUFFER_SIZE> doc;
    DeserializationError error = deserializeJson(doc, payload);
    serializeJson(doc, Serial);
    if (error)
    {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        return;
    }
    // Get hostname
    Serial.println("Host name before");
    Serial.println(Config.name);
    strcpy(Config.name, doc["host"]);
    Serial.println("Host name");
    Serial.println(Config.name);
    delay(2000);
// Get gpio config
#ifdef USE_GPIO
    ParseGPIOConfig(doc);
#endif // USE_GPIO

#ifdef USE_ADC

    ParseADCConfig(doc);
#endif // USE_ADC

#ifdef USE_SCALE

    parseHx711Config(doc);
#endif // USE_SCALE

#ifdef USE_DS18
    parseDs18b20Config(doc);
#endif // USE_DS18

    State.Configured = 1;
}
