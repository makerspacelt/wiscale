#include "global.h"

State DeviceState;
bool RetainPublications = false;

bool prefix(const char *pre, const char *str)
{
    return strncmp(pre, str, strlen(pre)) == 0;
}
String makeJsonLine(const char *key, uint8_t value){
    char result[64];
    sprintf(result, "\"%s\" : \"%d\"", key, value);
    return result;
}
String makeJsonLine(const char *key, float value){
    char result[64];
    sprintf(result, "\"%s\" : \"%.2f\"", key, value);
    return result;
}
String makeJsonLine(const char *key, const char *value)
{
    char result[64];
    sprintf(result, "\"%s\" : \"%s\"", key, value);
    return result;
}