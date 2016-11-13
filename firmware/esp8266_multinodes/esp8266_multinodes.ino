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
#include <OneWire.h>
#include <DallasTemperature.h>
#include <stdio.h>

#include <ESP_MQTTLogger.h>
#define MQTT_LOGGER_DEBUG true

#include "./node_modes.h"

// use this to get internal VCC value
ADC_MODE(ADC_VCC);

ESP8266WebServer webserver(9000);
ESP_Onboarding server(&webserver);

// Update these with values suitable for your network if needed.
#define NETWORK_TIMEOUT 30000
#define NETWORK_RETRIES 3
#define WAIT_PERIOD 20000

String clientname;

uint8_t network_tries = 0;

bool ap_mode = false;
bool mqtt_msg_shown = false;

WiFiClient espClient;
ESP_MQTTLogger logger(espClient, &webserver, 1883);

long lastMsg = 0;

#define ONE_WIRE_BUS 2  // DS18B20 pin
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature DS18B20(&oneWire);

bool got_temp = false;

void setup() {
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

    long start_time = millis();

    while (WiFi.status() != WL_CONNECTED) {
      // provide a mechanism for network timeout
      delay(500);
      if (start_time + NETWORK_TIMEOUT < millis()) {
        // we've timed out

        network_tries++;
        if (network_tries > NETWORK_RETRIES) {
          Serial.println("Connection timed out, set up as AP mode");
          setup_ap_mode();
          return;
        } else {
          Serial.println("");
          Serial.println("Restart the wireless module");
          WiFi.disconnect();
          delay(500);
          WiFi.begin(ssid.c_str(), pass.c_str());
          start_time = millis();
        }
      } else {
        Serial.print(".");
      }
    }

    Serial.print("IP: ");
    Serial.println(WiFi.localIP());

    // Do subscriptions for the node's config data
    if (! logger.connected() ) {
        logger.connect();
        logger.subscribe("id/#");

        // wait until we get the configuration messages or time out
        Serial.print("\nGetting config:");
        start_time = millis();
        while (start_time + CONFIG_TIMEOUT > millis() ) {
            delay(100);
            Serial.print(".");
            logger.handleClient();
        }
        Serial.println();

        // now set up sensors
        setup_node_peripherals(logger);
        // locate devices on the bus
        Serial.print("Locating devices...");
        DS18B20.begin();
        Serial.print("Found ");
        Serial.print(DS18B20.getDeviceCount(), DEC);
        Serial.println(" devices.");
        if (DS18B20.getDeviceCount() > 0) {
          got_temp = true;
        }
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

void loop() {

  // reconnect back to the MQTT server if needed
  if (WiFi.status() == WL_CONNECTED) {
    if (!logger.connected()) {
        logger.connect();
        if (! logger.connected() && ! mqtt_msg_shown ) {
            Serial.println(F("Can't connect to MQTT server after retry. Will not hibernate now"));
            mqtt_msg_shown = true;
        } else if (logger.connected() ) {
            mqtt_msg_shown = false; // reconnecting resets the message
        }
    }
  }

  server.handleClient();
  logger.handleClient();

  if (! ap_mode && logger.connected()) {

      Serial.println("Publishing");

      publish_peripheral_data();

      String temp;

      // test whether we should go get sensor readings
      DS18B20.requestTemperatures();
      temp = String(DS18B20.getTempCByIndex(0));

      if (got_temp) {
          logger.publish("temp/c", temp);
      }

      // system related information
      logger.publish("chip/vcc", String(ESP.getVcc()) );
      logger.publish("chip/free_heap", String(ESP.getFreeHeap()) );
      logger.publish("chip/run_time", String(millis()) );

      delay(100);
      WiFi.disconnect();
      delay(100);

      Serial.print("Hibernate: ");
      Serial.print(get_sleep_time());
      ESP.deepSleep(get_sleep_time() * 60 * 1000l * 1000l);
  }
}
