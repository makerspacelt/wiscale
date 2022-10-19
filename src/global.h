#ifndef GLOBAL_H
#define GLOBAL_H

//#define CONFIGURE_WIFI true
#define WIFI_SSID "SSID"
#define WIFI_PASS "PASS"

#define EXECUTION_TIMEOUT 10

struct globalConfig {
    int battery_range = 4860;
    float piece_grams = 85.0;
    float scale_zero = 185000;
    float scale_cal = 402.80;
    const char* name = (char *)"noname";
};

struct globalState {
    float grams = 0;
    float battery = 0;
    int pieces = 0;
    int configured = 0;
};

extern globalState state;
extern globalConfig config;


#endif // GLOBAL_H