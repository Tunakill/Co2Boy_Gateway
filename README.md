# Co2 Boy

![Logo](image.png)

Gateway for the Co2 Boy Sensor based on NodeMCU ESP32 Bord and RFM9x Lora Module.

This design was part of the [Make@thon](http://opto-cubes.de/?p=100) competition.

The Gateway receives data via LoRa and publishes the data to a MQTT server via WLAN.

## Setup

Follow this [heltec setup guide](https://heltec-automation-docs.readthedocs.io/en/latest/esp32+arduino/quick_start.html) to add the ESP32 board  to your IDE.

Following libraries are used:

- `Heltec ESP32` [HelTecAutomation/Heltec_ESP32](https://github.com/HelTecAutomation/Heltec_ESP32) - For Wifi    

- `LoRa` [sandeepmistry/arduino-LoRa](https://github.com/sandeepmistry/arduino-LoRa) - *Manual installation*

- `ArduinoJson`  [ArduinoJson](https://arduinojson.org/?utm_source=meta&utm_medium=library.properties)

- `PubSubClient` [knolleary/pubsubclient](https://github.com/knolleary/pubsubclient)

## Improvements

Duo to the time limits we were not able to implement all of the planed features.

At this point there is no measure to prevent spoofing. This can be prevented by adding cryptographic algorithms to encrypt or sign the payload. The public key could be used for the device ID instead of the mac address.
