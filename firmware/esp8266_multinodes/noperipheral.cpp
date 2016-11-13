#include <Arduino.h>
#include <ESP8266WebServer.h>
#include <PubSubClient.h>
#include <ESP_MQTTLogger.h>

#include "noperipheral.h"

NoPeripheral::NoPeripheral() {
    _vcc = 0;
}

NoPeripheral::NoPeripheral(ESP_MQTTLogger& l) {
    // set the logger if we know it and call begin
    _logger = l;
    NoPeripheral();
}

void NoPeripheral::begin(ESP_MQTTLogger& l) {
    // set up the logger
    _logger = l;
}

void NoPeripheral::publish_data() {

    // publish the system related information
    _logger.publish("chip/vcc", String(ESP.getVcc()) );
    _logger.publish("chip/free_heap", String(ESP.getFreeHeap()) );
    _logger.publish("chip/run_time", String(millis()) );
}
