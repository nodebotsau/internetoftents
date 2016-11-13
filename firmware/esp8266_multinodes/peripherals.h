/**
 * Used to set up the various peripheral classes that are used for the things
 * we're going to be plugging into the ESP
 */

#ifndef Peripherals_h
#define Peripherals_h

class Peripheral {
    public:
        virtual void config(ESP_MQTTLogger& l);
        virtual void publish_data();
    private:
        ESP_MQTTLogger _logger;
};


class NoPeripheral: public Peripheral {
    public:
        void config(ESP_MQTTLogger& l);
	    void publish_data();

    private:
        uint16_t _vcc;
};



#endif
