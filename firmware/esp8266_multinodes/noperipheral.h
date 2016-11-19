#ifndef NoPeripheral_h
#define NoPeripheral_h

#include <Arduino.h>
#include <ESP8266WebServer.h>
#include <PubSubClient.h>
#include <ESP_MQTTLogger.h>

#include "peripherals.h"

class NoPeripheral : public Peripheral {
    public:
        NoPeripheral();
        NoPeripheral(ESP_MQTTLogger& l);
        void begin(ESP_MQTTLogger& l);
        void publish_data();

    private:
        uint16_t _vcc;
        uint16_t _lastpublish = 0;

};

#endif

