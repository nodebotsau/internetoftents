#ifndef DHTPeripheral_h
#define DHTPeripheral_h

#include <Arduino.h>
#include <ESP8266WebServer.h>
#include <PubSubClient.h>
#include <ESP_MQTTLogger.h>

#include <Adafruit_Sensor.h>
#include <DHT_U.h>

#include "peripherals.h"


class DHTPeripheral : public Peripheral {
    public:
        DHTPeripheral();
        void begin(ESP_MQTTLogger& l);
        void publish_data();

    private:
        bool _got_sensor = false;
        DHT_Unified * _dht;
};

#endif

