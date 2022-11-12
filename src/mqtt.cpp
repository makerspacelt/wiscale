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
String getTemperatureJObject(MS_Ds18b20 sensor)
{
    char element[64];
    sprintf(element, "{\"%s\":\"%f\"}", sensor.config.name, sensor.temperature);
    return element;
}
String getTemperatureData(MS_Ds18b20 sensors[])
{
    char element[256];
    sprintf(element, "\"temperature\" : [");
    for(uint8_t i = 0; i < USED_TEMPERATURE_SENSORS; i++){
        strcat(element, getTemperatureJObject(sensors[i]).c_str());
        if(i < USED_TEMPERATURE_SENSORS - 1)
            strcat(element, ",");
    }
    strcat(element, "]");
    return element;
}
String getTemperatureDebugJObject(MS_Ds18b20 sensor)
{
    char element[256];
    sprintf(element, "{\"pre_multi\":\"%f\", \"pre_offset\":\"%f\", \"readings\":\"%d\"}", sensor.debug.pre_multi, sensor.debug.pre_offset, sensor.debug.readings);
    return element;
}
String getTemperatureDebugData(MS_Ds18b20 sensors[])
{
    char element[512];
    sprintf(element, "\"temperature\" : [");
    for(uint8_t i = 0; i < USED_TEMPERATURE_SENSORS; i++){
        strcat(element, getTemperatureDebugJObject(sensors[i]).c_str());
        if(i < USED_TEMPERATURE_SENSORS - 1)
            strcat(element, ",");
    }
    strcat(element, "]");
    return element;
}

String getScaleJObject(MS_HX711_Scale scale)
{
    char element[64];
    sprintf(element, "{\"%s\":\"%f\"}", scale.config.name, scale.grams);
    return element;
}
String getScaleData(MS_HX711_Scale scales[])
{
    char element[256];
    sprintf(element, "\"scales\" : [");
    for(uint8_t i = 0; i < USED_SCALES; i++){
        strcat(element, getScaleJObject(scales[i]).c_str());
        if(i < USED_SCALES - 1)
            strcat(element, ",");
    }
    strcat(element, "]");
    return element;
}
String getScaleDebugJObject(MS_HX711_Scale scale)
{
    char element[512];
    sprintf(element, "{\"readings\":\"%d\", \"gain\":\"%d\", \"pre_offset\":\"%f\", \"pre_multi\":\"%f\"}", 
    scale.debug.readings, scale.debug.gain, scale.debug.pre_offset, scale.debug.pre_multi);
    return element;
}
String getScaleDebugData(MS_HX711_Scale scales[])
{
    char element[1024];
    sprintf(element, "\"scales\" : [");
    for(uint8_t i = 0; i < USED_SCALES; i++){
        strcat(element, getScaleDebugJObject(scales[i]).c_str());
        if(i < USED_SCALES - 1)
            strcat(element, ",");
    }
    strcat(element, "]");
    return element;
}
String getAdcJObject(MS_ADC adc)
{
    char element[64];
    sprintf(element, "{\"%s\":\"%f\"}", adc.config.name, adc.voltage);
    return element;
}
String getAdcData(MS_ADC adcs[])
{
    char element[256];
    sprintf(element, "\"adc\" : [");
    for(uint8_t i = 0; i < USED_ADC_PINS; i++){
        strcat(element, getAdcJObject(adcs[i]).c_str());
        if(i < USED_ADC_PINS - 1)
            strcat(element, ",");
    }
    strcat(element, "]");
    return element;
}
String getAdcDebugJObject(MS_ADC adc)
{
    char element[256];
    sprintf(element, "{\"readings\":\"%d\", \"pre_offset\":\"%f\", \"pre_multi\":\"%f\"}", adc.debug.readings, adc.debug.pre_offset, adc.debug.pre_multi);
    return element;
}
String getAdcDebugData(MS_ADC adcs[])
{
    char element[512];
    sprintf(element, "\"adc\" : [");
    for(uint8_t i = 0; i < USED_ADC_PINS; i++){
        strcat(element, getAdcDebugJObject(adcs[i]).c_str());
        if(i < USED_ADC_PINS - 1)
            strcat(element, ",");
    }
    strcat(element, "]");
    return element;
}
String getMqttMsg(){
    char element[1024];
    sprintf(element, "{\"host\":\"%s\",%s,\"configured\":%d,\"charging\":%d, %s, %s}", 
    Config.name,  getAdcData(State.Adcs).c_str(), State.Configured, State.Charging, getScaleData(State.Scales).c_str(), getTemperatureData(State.Thermometers).c_str());
    return element;
}
String getMqttDebugMsg(){
    char element[1024];
    sprintf(element, "{\"host\":\"%s\", %s, %s, %s}", Config.name, getTemperatureDebugData(State.Thermometers).c_str(), getAdcDebugData(State.Adcs).c_str(), getScaleDebugData(State.Scales).c_str());
    return element;
}
void sendMessage()
{
    if (mqtt.connected())
    {
        char topic[64];
        sprintf(topic, "device/%s/data", Config.name);
        mqtt.publish(topic, getMqttMsg().c_str(), false);
    }
}
void sendDebugMessage(){
    if (mqtt.connected())
    {
        char topic[64];
        sprintf(topic, "device/%s/debug", Config.name);
        mqtt.publish(topic, getMqttDebugMsg().c_str(), false);
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
    Serial.println("Parsing adc config");
    JsonArray array = doc["adc"].as<JsonArray>();
    char msg[5];
    sprintf(msg, "Found adc sensors%d", array.size());
    Serial.println(msg);
    for (uint8_t i = 0; i < array.size(); i++)
    {
        JsonObject json = array[i];
        ADCConfig adcConfig = {};
        const char *name = json["name"];
        const char *mode = json["mode"];
        Serial.println(name);
        strcpy(adcConfig.name, name);
        strcpy(adcConfig.mode, mode);
        adcConfig.pin = json["pin"];
        adcConfig.offset = json["offset"];
        adcConfig.readings = json["readings"];
        adcConfig.multi = json["multi"];
        Config.adc[i] = adcConfig;
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
    strcpy(Config.name, doc["host"]);

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
