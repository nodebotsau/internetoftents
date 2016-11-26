#ifndef LedPeripheral_h
#define LedPeripheral_h

#include <Arduino.h>
#include <ESP8266WebServer.h>
#include <PubSubClient.h>
#include <ESP_MQTTLogger.h>

#include "peripherals.h"

class LedPeripheral : public Peripheral {
    public:
        LedPeripheral();
        void begin(ESP_MQTTLogger& l);
        void publish_data();
        void sub_handler(String topic, String payload);
        void update();

    private:
        uint8_t _state;

};

#endif



