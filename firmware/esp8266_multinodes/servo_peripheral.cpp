#include <Arduino.h>
#include <ESP8266WebServer.h>
#include <PubSubClient.h>
#include <ESP_MQTTLogger.h>

#include "servo_peripheral.h"

#define SERVOPIN  2

ServoPeripheral::ServoPeripheral() {}

void ServoPeripheral::begin(ESP_MQTTLogger& l) {
    // set up the logger
    _logger = l;

    _updateable = true;
    _publishable = false;

    _servo = new Servo();
    _servo->attach(SERVOPIN);

    bool subbed = _logger.subscribe("ic/#");

    if (! subbed) {
        Serial.println("Couldn't subscribe to content messages");
    }
}

void ServoPeripheral::publish_data() {

    // for a servo - this is a NOOP
    Serial.println("Servo, no data to publish");
}

void ServoPeripheral::sub_handler(String topic, String payload) {

    Serial.print("Servo: Topic: ");
    Serial.print(topic);
    Serial.print(" Payload: ");
    Serial.println(payload);

    if ( topic.endsWith("pos") ) {
        // interpret the position.
        // check explicitly for an "0" as str.toInt() returns 0 if type
        // conversion isn't successful
        if (payload == "0") {
            _pos = 0;
        } else {
            // attempt a type conversion
            _pos = payload.toInt();
            if (_pos == 0) {
                // set to center
                Serial.println("Value wasn't recognisable as an int");
                _pos = 90;
            } else if (_pos > 180) {
                // to to 180
                Serial.println("Val was too big");
                _pos = 180;
            }
        }
    }
}

void ServoPeripheral::update() {

    // update the position of the servo if needed.
    _servo->write(_pos);
}
