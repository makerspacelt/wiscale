#include "mqtt.h"
#include "ArduinoJson.h"

WiFiClient espClient;
PubSubClient mqtt(espClient);
bool initialized = false;

void initMqtt(char *deviceName){
    if(initialized) return;
    String clientId = WiFi.hostname();    
    mqtt.setServer("broker.lan", 1883);
    mqtt.setCallback(configCallback);
    mqtt.setBufferSize(2048); // must set to larger, as by default it is limited to 256
    char lastWillTopic[64];
    if(strlen(deviceName) > 0){
        sprintf(lastWillTopic, "device/%s/online", deviceName);
        if(!mqtt.connect(clientId.c_str(), "", "", lastWillTopic, 1, false, "0")){
            Serial.printf("Failed to connect to MQTT, state : %d\n", mqtt.state());
        }
        mqtt.publish(lastWillTopic, "1");
        initialized = true;
    }else{
        if(!mqtt.connect(clientId.c_str())){
            Serial.printf("Config failed to connect to MQTT, state : %d\n", mqtt.state());
        }
    }
    if(mqtt.subscribe((char *)("config/" + clientId).c_str(), 1)){
        Serial.printf("Subscribed to topic: config/%s\n", clientId.c_str());
    }
}
void loopMqtt()
{
    mqtt.loop();
}

void destroyMqtt()
{
    mqtt.disconnect();
}
void parseGPIOs(char *deathPinName, JsonArray gpios)
{
    if(gpios.size() > MAX_GPIO_PINS){
        Serial.println("Config contains more GPIO pins that HW is compatible for.");
        return;
    }
    for (uint8_t i = 0; i < gpios.size(); i++)
    {
        JsonObject config = gpios[i];
        GPIO gpio = GPIO(config);
        DeviceState.GPIOs[i] = gpio;
        if (gpio.mode == GPIOOut)
        {
            String subTopic = gpio.getSubscriptionTopic();
            Serial.printf("Subscribing to topic: %s\n", subTopic.c_str());
            mqtt.subscribe(subTopic.c_str(), 1);
        }
        if(strlen(deathPinName) > 0 && strcmp(deathPinName, gpio.name) == 0){
            DeviceState.deathPin = &DeviceState.GPIOs[i];
            digitalWrite(gpio.pin, gpio.defaultValue ^ gpio.inverted);
            Serial.printf("DeathPin assigned - %s\n", DeviceState.deathPin->name);
        }
    }
}
void parseTempSensors(JsonArray sensors)
{
    if (sensors.size() > MAX_TEMP_SENSORS)
    {
        Serial.println("Config contains more Temperature pins that HW is compatible for.");
        return;
    }
    for (uint8_t i = 0; i < sensors.size(); i++)
    {
        JsonObject config = sensors[i];
        Temperature sensor = Temperature(config);
        DeviceState.Sensors[i] = sensor;
    }
}
void parseADCs(JsonArray adcs)
{
    // serializeJson(adcs, Serial);
    if (adcs.size() > MAX_ADC_PINS)
    {
        Serial.println("Config contains more ADC pins that HW is compatible for.");
        return;
    }
    for (uint8_t i = 0; i < adcs.size(); i++)
    {
        JsonObject config = adcs[i];
        ADC adc = ADC(config);
        DeviceState.ADCs[i] = adc;
    }
}
void parseScales(JsonArray scales)
{
    if (scales.size() > MAX_SCALES)
    {
        Serial.println("Config contains more scales that HW is compatible for.");
        return;
    }
    for (uint8_t i = 0; i < scales.size(); i++)
    {
        JsonObject config = scales[i];
        Scale scale = Scale(config);
        DeviceState.Scales[i] = scale;
    }
}
void parseConfig(byte *payload){
    StaticJsonDocument<MAX_JSON_DOCUMENT_LENGTH> doc;
    DeserializationError error = deserializeJson(doc, payload);
    DeviceState.IsConfigured = false;
    DeviceState.deathPin = NULL;
    if (error)
    {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        return;
    }
    strcpy(DeviceState.name, doc["host"]);
    if(!initialized){
        mqtt.disconnect();
        mqtt.flush();
        while(mqtt.state() != -1){
            delay(10); 
        }
        initMqtt(DeviceState.name);
        return;
    }
    char deathPinName[MAX_NAME_LENGTH] = "";
    if(doc.containsKey("death_pin")){
        strcpy(deathPinName, doc["death_pin"]);
    }
    if(doc.containsKey("loop_delay")){
        DeviceState.loopDelay = doc["loop_delay"];
        Serial.printf("Custom loop delay - %d\n", DeviceState.loopDelay);
    }
#ifdef USE_GPIO
    parseGPIOs(deathPinName, doc["gpio"].as<JsonArray>());
#endif
#ifdef USE_DS18
    parseTempSensors(doc["ds18b20"].as<JsonArray>());
#endif
#ifdef USE_ADC
    parseADCs(doc["adc"].as<JsonArray>());
#endif
#ifdef USE_SCALE
    parseScales(doc["hx711"].as<JsonArray>());
#endif
    DeviceState.IsConfigured = true;
}

void dealWithOutputs(char *topic, byte *payload)
{
    for(uid_t i = 0; i < MAX_GPIO_PINS; i++){
        GPIO gpio = DeviceState.GPIOs[i];
        if (!gpio.IsConfigured)
            continue;
        if (strcmp(topic, gpio.getSubscriptionTopic().c_str()) == 0)
        {
            int val = ((char)payload[0] == '1' ? 1 : 0) ^ gpio.inverted;
            digitalWrite(gpio.pin, val);
            char publishPayload[1];
            sprintf(publishPayload, "%d", val ^ gpio.inverted);
            mqtt.publish(gpio.getPublicationTopic().c_str(), publishPayload, RetainPublications);
        }
    }
}
void finishPublishing(){
    if(DeviceState.deathPin != NULL)
    {
        Serial.println("Trying to publish death gpio");
        Serial.println(DeviceState.deathPin->getSubscriptionTopic().c_str());
        mqtt.publish(DeviceState.deathPin->getSubscriptionTopic().c_str(), "1");
    }
}
void configCallback(char *topic, byte *payload, unsigned int length)
{
    Serial.printf("Received MQTT message. Topic: %s\n", topic, (char*)payload);
    if(prefix("config", topic)){
        parseConfig(payload);
        return;
    }
    dealWithOutputs(topic, payload);
}
void publishGPIOData(){
    for(uint8_t i = 0; i < MAX_GPIO_PINS; i++){
        GPIO gpio = DeviceState.GPIOs[i];
        if(!gpio.IsConfigured || gpio.mode != GPIOIn) continue;
        char payload[10];
        sprintf(payload, "%d", gpio.value ^ gpio.inverted);
        mqtt.publish(gpio.getPublicationTopic().c_str(), payload, RetainPublications);
    }
}
void publishTemperatureData()
{
    for (uint8_t i = 0; i < MAX_TEMP_SENSORS; i++)
    {
        Temperature sensor = DeviceState.Sensors[i];
        if (!sensor.IsConfigured)
            continue;
        char payload[10];
        sprintf(payload, "%.2f", sensor.temperature);
        mqtt.publish(sensor.getPublicationTopic().c_str(), payload, RetainPublications);
    }
}
void publishADCData()
{
    for (uint8_t i = 0; i < MAX_ADC_PINS; i++)
    {
        ADC adc = DeviceState.ADCs[i];
        if (!adc.IsConfigured)
            continue;
  
        char payload[10];
        sprintf(payload, "%.2f", adc.adcValue);
        mqtt.publish(adc.getPublicationTopic().c_str(), payload, RetainPublications);
    }
}
void publishScalesData(){
    for (uint8_t i = 0; i < MAX_SCALES; i++)
    {
        Scale scale = DeviceState.Scales[i];
        if (!scale.IsConfigured)
            continue;

        char payload[10];
        sprintf(payload, "%.2f", scale.weight);
        mqtt.publish(scale.getPublicationTopic().c_str(), payload, RetainPublications);
    }
}
void publishDebugInfo(){
    mqtt.publish(DeviceState.getConfiguredPublicationTopic().c_str(), "1", RetainPublications);
    mqtt.publish(DeviceState.getDebugPublicationTopic().c_str(), DeviceState.getDebugPayload().c_str(), RetainPublications);
}