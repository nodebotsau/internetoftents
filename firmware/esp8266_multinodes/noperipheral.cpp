#include <Arduino.h>
#include <ESP8266WebServer.h>
#include <PubSubClient.h>
#include <ESP_MQTTLogger.h>

#include "noperipheral.h"

NoPeripheral::NoPeripheral() {
    Serial.println("No Peripheral constructor");
    _vcc = 0;
}

NoPeripheral::NoPeripheral(ESP_MQTTLogger& l) {
    // set the logger if we know it and call begin
    _logger = l;
    NoPeripheral();

}

void NoPeripheral::begin(ESP_MQTTLogger& l) {
    _logger = l;
    _vcc = 1000;
    Serial.println("No Peripheral configurator");
}

void NoPeripheral::publish_data() {
    Serial.print("Publishing data: ");
    Serial.println(_vcc);
}
