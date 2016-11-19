/**
 * Used to set up the various peripheral classes that are used for the things
 * we're going to be plugging into the ESP
 */

#ifndef Peripherals_h
#define Peripherals_h

#include "Arduino.h"

class Peripheral {
    public:
        Peripheral() {};
        virtual ~Peripheral() {};
        // begin used to do any set up work that's needed
        virtual void begin(ESP_MQTTLogger& l) {};

        // publish is abstract call for publishing data to the server
        // depending on what the node can do
        virtual void publish_data() {};
        // If the node can take input content (eg on clientname/ic/# topic)
        // Then this handler is called by the master caller.
        virtual void sub_handler(String topic, String payload){ ; };
        // This is called every cycle in order to ensure that the peripheral
        // is up to date for eg updating positions etc.
        virtual void update() {};

        // determines if the node is publishable or not.
        bool publishable() { return _publishable; };
        // determines if the node can be updated or not.
        bool updatable() { return _updateable; };

    protected:
        ESP_MQTTLogger _logger;
        bool _publishable = false;;
        bool _updateable = false;
};

#endif
