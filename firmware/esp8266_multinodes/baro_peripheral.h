#ifndef BaroPeripheral_h
#define BaroPeripheral_h

#include <Arduino.h>
#include <Wire.h>

#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085_U.h>

#include <ESP8266WebServer.h>
#include <PubSubClient.h>
#include <ESP_MQTTLogger.h>


#include "peripherals.h"

class BaroPeripheral : public Peripheral {
    public:
        BaroPeripheral();
        void begin(ESP_MQTTLogger& l);
        void publish_data();

    private:
		Adafruit_BMP085_Unified * _bmp; 
		bool _got_sensor = false;
};

#endif

