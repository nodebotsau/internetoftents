#include <Arduino.h>
#include <FS.h>
#include <stdio.h>
#include "./node_modes.h"

bool _state_config = false;
uint8_t _sleep_period = DEFAULT_SLEEP_MINS;

uint8_t _mode = 0;

void config_subscription(char* topic, byte* payload, unsigned int length) {

    // handles the things around the subscription.
    //
    String t = (String)topic;

    Serial.print("Topic is: ");
    Serial.println(t);

    // grab the data from the payload and get it in a form we can use it.
    char buf[length + 1];
    for (int i = 0; i < length; i++) {
        buf[i] = (char)payload[i];
    }
    buf[length] = '\0';

    String p = String(buf);
    Serial.print("Payload: ");
    Serial.println(p);

    if (t.endsWith("sleep")) {
        set_sleep_time(p.toInt());
    } else if (t.endsWith("mode")) {
        Serial.println("Changing mode");
    }
}

bool state_configured() {
    return _state_config;
}

uint8_t get_sleep_time() {


    return _sleep_period;
}

void set_sleep_time(uint8_t sleep) {
    // here we set how long the system should hibernate for

    _sleep_period = sleep;
    // TODO Write to the file system.
}

bool save_config(String filename, String value) {
    // takes a filename and a string and writes it down.
    File configFile = SPIFFS.open(filename, "w");

    if (!configFile) {
        Serial.println("Error saving to file system");
        return false;
    }

    configFile.print(value);
    return true;
}

String load_config(String filename) {
    // loads a file and returns its contents

    File configFile = SPIFFS.open(filename, "r");

    if (configFile) {
        return configFile.readString();
    }

    Serial.println("Error reading from file");
    return "";
}
