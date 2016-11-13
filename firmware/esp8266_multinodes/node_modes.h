/** used to define the useful aspects of dealing with the node type stuff.

**/
#include <Arduino.h>

#ifndef NodeMode_h
#define NodeMode_h

#define DEFAULT_SLEEP_MINS 1

// time we're prepared to wait for the config
#define CONFIG_TIMEOUT 2000

enum MODES {
    NONE,
    TEMP_1WIRE,
    BARO,
    DHT
};


uint8_t get_sleep_time();
void set_sleep_time(uint8_t sleep);
void config_subscription(char* topic, byte* payload, unsigned int length);
bool state_configured();
bool save_config(String filename, String value);
String load_config(String filename);



#endif
