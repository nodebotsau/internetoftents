#include <Arduino.h>
#include <ESP8266WebServer.h>
#include <PubSubClient.h>
#include <ESP_MQTTLogger.h>

#include "led_peripheral.h"

#define LED_PIN  2


LedPeripheral::LedPeripheral() {}

void LedPeripheral::begin(ESP_MQTTLogger& l) {
    // set up the logger
    _logger = l;

    _updateable = true;
    _publishable = false;

    pinMode(LED_PIN, OUTPUT);
    _state = false;

    bool subbed = _logger.subscribe("ic/#");

    if (! subbed) {
        Serial.println("Couldn't subscribe to content messages");
        _logger.publish("sys/error", "sub_fail");
    } else {
        _logger.publish("oc/status", "available");
    }
}

void LedPeripheral::publish_data() {

    // for an LED - this is a NOOP
}

void LedPeripheral::sub_handler(String topic, String payload) {

    if ( topic.endsWith("led") ) {
        // interpret the position.
        // check explicitly for an "0" as str.toInt() returns 0 if type
        // conversion isn't successful
        if (payload == "1" || payload == "on") {
            _state = true;
            Serial.println("Turn on");
        } else {
            _state = false;
            Serial.println("Turn off");
        }
    }

}

void LedPeripheral::update() {

    // update the state of the LED
    digitalWrite(LED_PIN, _state);

}

