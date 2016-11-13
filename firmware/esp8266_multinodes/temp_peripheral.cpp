#include <Arduino.h>
#include <ESP8266WebServer.h>
#include <PubSubClient.h>
#include <ESP_MQTTLogger.h>

#include "temp_peripheral.h"

#define ONE_WIRE_BUS 2  // DS18B20 pin

TempPeripheral::TempPeripheral() {}

void TempPeripheral::begin(ESP_MQTTLogger& l) {
    // set up the logger
    _logger = l;

    _one_wire = new OneWire(ONE_WIRE_BUS);
    _DS18B20 = new DallasTemperature(_one_wire);
    // locate devices on the bus

    _DS18B20->begin();

    if (_DS18B20->getDeviceCount() > 0) {
        _got_temp = true;
    } else {
        Serial.println("No devices found on 1-Wire bus");
    }
}

void TempPeripheral::publish_data() {

    // publish temperature data from the sensor
    String temp;

    _DS18B20->requestTemperatures();

    temp = String(_DS18B20->getTempCByIndex(0));

    if (_got_temp) {
      _logger.publish("temp/c", temp);
    }
}
