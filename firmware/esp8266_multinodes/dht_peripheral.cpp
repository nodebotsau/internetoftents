#include <Arduino.h>
#include <ESP8266WebServer.h>
#include <PubSubClient.h>
#include <ESP_MQTTLogger.h>

#include "dht_peripheral.h"

#define DHTPIN  2
#define DHTTYPE DHT22

DHTPeripheral::DHTPeripheral() {}

void DHTPeripheral::begin(ESP_MQTTLogger& l) {
    // set up the logger
    _logger = l;

    _dht = new DHT_Unified(DHTPIN, DHTTYPE);
    _dht->begin();

}

void DHTPeripheral::publish_data() {

    sensors_event_t event;
    _dht->temperature().getEvent(&event);
    if (! isnan(event.temperature) ) {
        _logger.publish("temp/c", String(event.temperature));
    }

    _dht->humidity().getEvent(&event);
    if (! isnan(event.relative_humidity) ) {
        _logger.publish("humidity", String(event.relative_humidity));
    }
}
