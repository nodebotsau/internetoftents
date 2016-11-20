#ifndef PixelPeripheral_h
#define PixelPeripheral_h

#include <Arduino.h>
#include <ESP8266WebServer.h>
#include <PubSubClient.h>
#include <ESP_MQTTLogger.h>


#include "peripherals.h"

enum PX_STATES {
    PXP_NONE,
    PXP_START,
    PXP_END,
    PXP_PIXEL,
    PXP_STRIP
};


class PixelPeripheral : public Peripheral {
    public:
        PixelPeripheral();
        void begin(ESP_MQTTLogger& l);
        void publish_data();
        void sub_handler(String topic, String payload);
        void update();

    private:
        uint8_t * _px;
        uint16_t _px_count = 0;
        uint8_t _colour_depth = 3;
        unsigned long _last_update = 0;

        void initialise_pixels(uint16_t num_pixels);
        void set_pixel(uint16_t pixel, uint8_t r, uint8_t g, uint8_t b);
        void set_strip(uint8_t r, uint8_t g, uint8_t b);

};

#endif



