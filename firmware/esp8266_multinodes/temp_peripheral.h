#ifndef TempPeripheral_h
#define TempPeripheral_h

#include <Arduino.h>
#include <ESP8266WebServer.h>
#include <PubSubClient.h>
#include <ESP_MQTTLogger.h>

#include <OneWire.h>
#include <DallasTemperature.h>

#include "peripherals.h"

class TempPeripheral : public Peripheral {
    public:
        TempPeripheral();
        void begin(ESP_MQTTLogger& l);
        void publish_data();

    private:
        bool _got_temp = false;
        OneWire * _one_wire;
        DallasTemperature * _DS18B20;
};

#endif

