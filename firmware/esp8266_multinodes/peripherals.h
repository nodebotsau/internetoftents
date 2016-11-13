/**
 * Used to set up the various peripheral classes that are used for the things
 * we're going to be plugging into the ESP
 */

#ifndef Peripherals_h
#define Peripherals_h

#include "Arduino.h"
#include <ESP_MQTTLogger.h>

class Peripheral {
    public:
        Peripheral() {};
        virtual ~Peripheral() {};
        virtual void begin(ESP_MQTTLogger& l) {};
        virtual void publish_data() {};
    protected:
        ESP_MQTTLogger _logger;
};

#endif
