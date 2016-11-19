#ifndef ServoPeripheral_h
#define ServoPeripheral_h

#include <Arduino.h>
#include <ESP8266WebServer.h>
#include <PubSubClient.h>
#include <ESP_MQTTLogger.h>

#include <Servo.h>

#include "peripherals.h"


class ServoPeripheral : public Peripheral {
    public:
        ServoPeripheral();
        void begin(ESP_MQTTLogger& l);
        void publish_data();
        void sub_handler(String topic, String payload);
        void update();

    private:
        Servo * _servo;
        uint8_t _pos = 90; // holds position of servo
};

#endif


