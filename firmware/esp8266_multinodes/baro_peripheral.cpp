#include <Arduino.h>

#include <ESP8266WebServer.h>
#include <PubSubClient.h>
#include <ESP_MQTTLogger.h>

#include "baro_peripheral.h"

#define DATA_PIN 0
#define CLK_PIN 2

BaroPeripheral::BaroPeripheral() {}

void BaroPeripheral::begin(ESP_MQTTLogger& l) {
    // set up the logger
    _logger = l;
	_bmp = new Adafruit_BMP085_Unified(10085);

	// TODO refactor this because it might be on alt pins

	Wire.begin(DATA_PIN, CLK_PIN);
    delay(100); // just wait for the I2C stuff all get sorted
	if(! _bmp->begin() ) {
		/* There was a problem detecting the BMP085 ... check your connections */
		Serial.println("Ooops, no BMP085 detected ... Check your wiring or I2C ADDR!");
	} else {
		_got_sensor = true;
	}

}

void BaroPeripheral::publish_data() {

    // get the sensor event and load it up/
    if (_got_sensor) {
        sensors_event_t event;
        _bmp->getEvent(&event);

        if (event.pressure) {

            float pressure;
            _bmp->getPressure(&pressure);
            _logger.publish("baro/hpa", String(pressure));

            float temperature;
            _bmp->getTemperature(&temperature);
            _logger.publish("temp/c", String(temperature));
        }
    }

}
