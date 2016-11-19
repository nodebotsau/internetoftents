/* Multi-Role ESP8266 MQTT sensor node.

Adapted from work done on ESPlant and example MQTT sketch and refined from there

*/
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ESP8266WebServer.h>
#include <ESP_Onboarding.h>
#include <stdio.h>

#include <ESP_MQTTLogger.h>
//#define MQTT_LOGGER_DEBUG true

#include "./node_modes.h"

// use this to get internal VCC value
ADC_MODE(ADC_VCC);

ESP8266WebServer webserver(9000);
ESP_Onboarding server(&webserver);

// Update these with values suitable for your network if needed.
#define NETWORK_TIMEOUT 30000
#define NETWORK_RETRIES 2

// msec to wait on the loop cycle
#define WAIT_PERIOD 100
String clientname;

uint8_t network_tries = 0;

bool ap_mode = false;
bool mqtt_msg_shown = false;

WiFiClient espClient;
ESP_MQTTLogger logger(espClient, &webserver, 1883);

bool subscribed = false;

long lastMsg = 0;

void setup() {

    // set pins to input to be explicit
    pinMode(0, INPUT);
    pinMode(2, INPUT);

    Serial.begin(115200);
    server.begin();
    logger.begin();

    // set callback for the logger
    logger._client.setCallback(config_subscription);

    Serial.println("\n\n");
    Serial.print("Access Token: ");
    Serial.println(server.getToken());

    clientname = String("ESP_") + String(ESP.getChipId(), 16);
    Serial.print("Board Client ID: ");
    Serial.println(clientname);

    logger.setToken(server.getToken());

    bool configured = server.loadWifiCreds();

    Serial.println("Load onboarding server");
    server.startServer(configured);

    // do we have config
    if (configured) {

        // configure station mode
        WiFi.mode(WIFI_STA);

        String ssid = server.getSSID();
        String pass = server.getPassword();

        Serial.println("");
        Serial.print("Connecting to ");
        Serial.println(ssid);

        // use config to connect
        WiFi.begin(ssid.c_str(), pass.c_str());

        unsigned long start_time = millis();

        while (WiFi.status() != WL_CONNECTED) {
            // provide a mechanism for network timeout
            delay(500);
            if (start_time + NETWORK_TIMEOUT < millis()) {
                // we've timed out

                network_tries++;
                if (network_tries > NETWORK_RETRIES) {
                    Serial.println("\nConnection timed out, set up as AP mode");
                    setup_ap_mode();
                    return;
                } else {
                    Serial.println("\nRestart the wireless module");
                    WiFi.disconnect();
                    delay(300);
                    WiFi.begin(ssid.c_str(), pass.c_str());
                    start_time = millis();
                }
            } else {
                Serial.print(".");
            }
        }

        Serial.print("IP: ");
        Serial.println(WiFi.localIP());

        // Do subscriptions to get the node's config data
        if (! logger.connected() ) {
            logger.connect();
            setup_subscriptions();
            // now set up sensors
            setup_node_peripherals(logger);
        }

        return; // we're done
    }

    // fall back to the AP mode
    setup_ap_mode();
}

void setup_ap_mode() {
  Serial.printf("AP: ESP_%06X\n", ESP.getChipId());

  // fall back to AP mode to enable onboarding
  WiFi.mode(WIFI_AP);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("IP address: ");
  Serial.println(myIP);

  ap_mode = true;
}

void setup_subscriptions() {
    // we use this to grab node subscriptions or resubscribe if
    // there's a failure.

    subscribed = logger.subscribe("id/#");

    // wait until we get the configuration messages or time out
    Serial.print("\nGetting config:");
    unsigned long start_time = millis();
    while (start_time + CONFIG_TIMEOUT > millis() ) {
        delay(100);
        Serial.print(".");
        logger.handleClient();
    }
    Serial.println();

}

void loop() {

    // reconnect back to the MQTT server if needed
    if (WiFi.status() == WL_CONNECTED) {
        if (!logger.connected()) {
            logger.connect();
            if (! logger.connected() && ! mqtt_msg_shown ) {
                Serial.println(F("Can't connect to MQTT server after retry. Will not hibernate now"));
                mqtt_msg_shown = true;
            } else if (logger.connected() ) {
                // we have reconnected
                mqtt_msg_shown = false; // reconnecting resets the message
                setup_subscriptions();
            }
        }

        if (! subscribed) {
            setup_subscriptions();
        }
    }

    server.handleClient();
    logger.handleClient();

    if (! ap_mode && logger.connected()) {

        publish_peripheral_data();

        // now depending on if we hibernate or not determines what we do
        // next. If we hibernate then we drop into deep sleep mode
        if (get_sleep_time() > 0) {

            delay(100);
            WiFi.disconnect();
            delay(100);

            Serial.print("Hibernate: ");
            Serial.print(get_sleep_time());
            ESP.deepSleep(get_sleep_time() * 60 * 1000l * 1000l);
        } else {
            // otherwise we just wait around for a few msec and then go around
            // the loop again
            delay(WAIT_PERIOD);
        }
    }
}
