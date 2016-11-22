# Internet of Tents

This repo contains the documentation, firmware and examples for the
Buzz Conf 2016 Internet of Tents workshop.

## Installation

To get your Node environment along with relevant drivers etc up and running,
please follow the directions in our [standard setup doc](https://github.com/nodebotsau/nbdau/blob/master/setup.md)

Please note at minimum you need the following before going any further

* Git
* A working node install, ideally node 6+
* Drivers for Arduino if you're using Win / Mac

### Installing this repo

Git clone or download and unzip this repo to a folder:

```
git clone git@github.com:nodebotsau/internetoftents.git
cd internetoftents
```

Then install dependencies

```
npm install
```

This will install:

* Node Red server that can be run locally (including contrib modules for Johnny
Five, InfluxDB
* Johnny Five (to interact with a local arduino)
* Interchange (so you can flash firmware to an arduino)
* MQTT js (to talk to MQTT servers)

## Running Node Red

To run node-red, `cd` to the root of your project folder then run:

```
./node-modules/.bin/node-red
```

And you will see Node Red spin up. If there are no errors you can access it
on [Localhost:1880](http://127.0.0.1:1880)

By default, contrib packages have been installed for:

* Johnny Five (GPIO nodes)
* InfluxDB (read and write nodes)

You can install more by simply npm-installing from your project folder and use
the packages that are provided at [http://flows.nodered.org/](http://flows.nodered.org/).

## Running a local MQTT Server

If you've installed the repo dependencies then you'll be able to run your own
node-based MQTT server using `mosca`.

To run the server:

```
./node_modules/.bin/mosca -v --http-port 8883
```

This will pass all the config details to the command line but should set up
the standard MQTT server on port 1883 as well as an HTTP Websockets port on
8883 on your local host.

You can demo this working by subscribing to a topic and then publishing a message.

In a new terminal execute a subscription request to the topic `hello`:

```
./node_modules/.bin/mqtt sub -t "hello" -h "localhost" -v
```

Then in yet *another* new terminal, publish a message to the `hello` topic:

```
./node_modules/.bin/mqtt pub -t 'hello' -h 'localhost' -m 'from MQTT.js'
```

If everything has gone properly then in your server terminal you should see the
message and payload details and in the subscription terminal you should see
the message results.

Congrats, you now have your very own MQTT broker and can publish messages and
subscribe to topics. If you wanted to, you could now point your Node-Red
instance at it using the MQTT publish and subscribe nodes.

Alternatively you can also use the BuzzConf site MQTT broker too - the details
of which are given below.

### Alternative servers

If you have a linux VM or linux machine you can install mosquitto
with simply an `apt-get install mosquitto mosquitto-client` which will give you
a highly configurable and robust MQTT broker to play with. This is what is running
the site infrastructure for BuzzConf.

## Using the BuzzConf MQTT server

The site wide MQTT server is available at `mqtt-broker.local` on the LAN. You
must be connected to the BuzzConf network in order to be able to access it. The
server will not be directly accessible from the Internet, however it is capable
of publishing messages to the Internet and other relay servers.

The port for the MQTT server is the standard MQTT port on `1883`.

### A note about security

This server and network has purposefully not been hardened for the following reasons:

* The infrastructure is transient - only up for the duration of BuzzConf
* The infrastructure is for demo and teaching purposes
* The servers are not directly accessible from the Internet and are blocked by
network level infrastructure.
* Devices on this network are partitioned and can be removed relatively easily.

All this is to say that security is absolutely important in any network service
context however it must also be appropriate to the level of risk. In this instance
that has been assessed and the appropriate levels of control implemented.

## Using an ESP8266 Module

By default, all modules have been flashed with the firmware provided in the
firmware folder. This is a multi-role firmware that allows interaction with:

* 1-Wire temperature sensor
* Barometric pressure sensor
* Humidity sensor
* Standard servo
* NeoPixel strip (up to 64 long)

All modules will have their ID written on them. If they are on the
network they will post messages to the `ESP_XXXXXX/` topic where `XXXXXX` is the
module ID.

If they don't associate to the network then they will create their
own WiFi access point where the SSID will be the same client id above and there
will be no password. If you don't automatically get an IP address, then the
module will be on `192.168.4.1` and has an onboarding server that can take
requests on port `9000`

### Debugging ESP8266 modules

Modules can be debugged simply by watching the serial console. Connect a
USB-Serial cable / module using 3.3V TTL serial to the ESP module and your
computer then connect at 115200 baud.

eg:

```
screen /dev/tty.usbmode1451 115200
```

Serial is read only with this firmware though it is reasonably chatty so it
will give you current status on connectivity, IP address etc.

### Configuring the nodes

Assuming the module is connected to the network if you open a connection to
the topic `ESP_XXXXXX/#` then you should be able to observe the module sending
messages to the MQTT broker.

To configure the module you can post messages to the following topics (which
should have the retain flag set to true).

`ESP_XXXXXX/id/sleep <M>` where M is minutes as an integer - this will tell the
module to go into hibernation mode (assuming you have the pin set for deep sleep)
for <M> minutes. Set to 0 to keep the module permanently awake.

`ESP_XXXXXX/id/mode <mode>` where mode is one of:

* `1wiretemp` - 1 wire temperature module - wired to GPIO 2
* `baro` - BMP180 pressure sensor wired, SDA to GPIO 0, SCL to GPIO 2
* `dht` - DHT22 humidity sensor wired to GPIO 2
* `servo` - Standard servo wired to GPIO 2
* `pixel` - NeoPixel strip wired to GPIO 2

### Using the peripherals

Each of the nodes is broken down in detail below:

#### All peripherals

All nodes report their current status periodically. This comprises at a
minimum their current power reading (very useful in the case of running off battery),
their free memory and their uptime in msec. Topics are:

* `ESP_XXXXXX/`
    * `sys/version` - version and compile date and time of the firmware
    * `chip/`
        * `vcc` - power reading on the chip in approx mVolts
        * `free_heap` - bytes left on the heap
        * `run_time` - msec since starting

#### 1 wire temperature sensor

The 1-wire temp sensor can be used to get temperature good to appoximately
half a degree C.

* `ESP_XXXXXX/`
    * `sys/error` - string error code (`no_sensor` means it can't find the item
                    on the 1-wire bus)
    * `temp/c` - temperature float as degrees C

#### Barometric pressure sensor

The Barometric pressure sensor will provide temperature as well as barometric
pressure in Pascals.

* `ESP_XXXXXX/`
    * `sys/error` - string error code (`no_sensor` means it can't find the I2C
                    item)
    * `temp/c` - temperature float as degrees C
    * `baro/pa` - atmospheric pressure in Pascals divide by 100 for hPa

#### DHT22 humidity sensor

The relative humidity expressed as a percentage, also provides a temperature
reading.

* `ESP_XXXXXX/`
    * `sys/error` - string error code (`no_sensor` means it can't find the I2C
                    item)
    * `temp/c` - temperature float as degrees C
    * `humidity` - relative humidity float as %

#### Standard servo

A standard servo can move in an arc between 0 and 180 degrees with 90 being
the center (neutral) point. The standard servo peripheral publishes no continuous
messages however it can respond to messages sent to it's `input content` topic.

The servo peripheral will send it's status and error messages to the MQTT server
if it discovers a problem and once it's available for messaging.

* `ESP_XXXXXX/`
    * `sys/error` - string error code (`sub_fail` means a subscription to the
                    `ic/#` topic couldn't be achieved (and thus messages sent
                    for position will fail).
    * `oc/status` - string stating presence on the network once it has associated.
                    Will send `available`.
    * `ic/pos` `int` - an integer in the range [0, 180] specifying the position
                        that the servo should move to.

The servo will process position messages almost instantly however there may be
a little lag due to network latency and the message being put in a queue for
updating the servo position.

#### NeoPixel strip

The neopixel strip can orchestrate a group of up to 64 neopixels on a single
strip. Individual pixels can be controlled as can the whole strip and it is
possible to send an entire frame of data as a binary string.

* `ESP_XXXXXX/`
    * `sys/error` - string error code (`sub_fail` means a subscription to the
                    `ic/#` topic couldn't be achieved (and thus messages sent
                    for position will fail).
    * `oc/status` - string stating presence on the network once it has associated.
                    Will send `available`.
    * `ic/strip` `rrggbb` - a hex colour string specifying the colour that the
                            entire strip should be changed to.
    * `ic/pixel/<id>` `rrggbb` - `<id>` is an int representing the zero-indexed
                                pixel in the strip and `rrggbb` is a hex colour
                                string to set that pixel to.
    * `ic/data` `bytestring` - a stream of bytes starting at pixel 0 and extending
                                up to a max of 64 pixels. Each pixel is represented
                                by three bytes encoded in GRB format (note the order!)(1)

(1) Note that the arduino String library has an issue in how it interprets 0x0
as a string (the `NUL` char) and it will stop processing at that point. As such
if sending 0 values for a colour channel on a pixel, send a 0x01 value instead
which will make no material change to the output but allow the string to be
processed.

## Installing firmware on an arduino

Please refer to the [Interchange documentation](https://github.com/johnny-five-io/nodebots-interchange)

The quick version to get firmata(make sure arduino is plugged in and you've installed the
driver previously before you do:

```
./node-modules/.bin/interchange install StandardFirmata -a uno
```

## Using a Raspberry Pi as a server

If you're looking to set up a raspberry pi as your own little hub server then
[this document may be of use](https://docs.google.com/document/d/1llTmqxWmVcaDcGI1X0rxw1OqeWoK4xI7TtUA_eyUONY/edit?usp=sharing).
This has the full installation process that we've used for our set up as well.

## Installing the ESP8266 firmware

This is not the most straightforward exercise but the following is a guide:

* Make sure you have absolute latest Arduino IDE installed
* Install the packages as given in the [ESPlant repo](https://github.com/CCHS-Melbourne/ESPlant/)
* You should now be able to build the firmware from the IDE.

In addition you'll need a USB-Serial converter that can do 3.3V TTL serial and
you have wired that up appropriately. If you're using ESP-01 modules you'll
also need to ground the CH_PD pin when you flash in order to drop the module
into flash mode.

Alternatively, ask Andrew to flash your board.

