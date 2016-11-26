/** used to define the useful aspects of dealing with the node type stuff.

**/
#ifndef NodeMode_h
#define NodeMode_h

#include <Arduino.h>

#define DEFAULT_SLEEP_MINS 0
//#define LIGHT_SLEEP true

// time we're prepared to wait for the config
#define CONFIG_TIMEOUT 2000

enum MODES {
    NONE,
    TEMP_1WIRE,
    BARO,
    DHT,
    SERVO,
    PIXEL,
    LED
};


void setup_node_peripherals(ESP_MQTTLogger& l);
void publish_peripheral_data();
void update_peripheral();

uint8_t get_sleep_time();
void set_sleep_time(uint8_t sleep);

MODES get_mode();
void set_mode(String modename);
void set_mode(MODES modeid);

void subscription_handler(char* topic, byte* payload, unsigned int length);
bool state_configured();

bool save_config(String filename, String value);
String load_config(String filename);



#endif
