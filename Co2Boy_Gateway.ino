/*
     ______     ___      ____
    / ____/___ |__ \    / __ )____  __  __
   / /   / __ \__/ /   / __  / __ \/ / / /
  / /___/ /_/ / __/   / /_/ / /_/ / /_/ /
  \____/\____/____/  /_____/\____/\__, /
                                 /____/

  Gateway for Co2 Boy SDC30 Lora Sensor
  By: Co2 Boy on Make@Thon
  Date: 15.11.2020

  Receives data via LoRa and publishes the data to an MQTT server via WLAN
*/

 /*
  -- Pinout
  https://learn.adafruit.com/adafruit-rfm69hcw-and-rfm96-rfm95-rfm98-lora-packet-padio-breakouts/pinouts
  ESP -> Lora Module RFM9x
  3V3 -> Vin
  GND -> GND
  G14 -> RST
  G2 -> G0
  G5 -> CS
  G18 -> SCK
  G19 -> MISO
  G23 -> MOSI
 */

 /*
  -- Additional libraries -- 
  LoRa               https://github.com/sandeepmistry/arduino-LoRa
  ArduinoJson        https://arduinojson.org/?utm_source=meta&utm_medium=library.properties
 */

#include <SPI.h>
#include <LoRa.h>

#include "LoraReceiver.h"

#include <ArduinoJson.h> // Payloud parser

#include <WiFi.h>         // ESP Wifi
#include <PubSubClient.h> //MQTT Client

#define WIFI_SSID "xxx"           // WiFi SSID
#define WIFI_PASSWORD "xxxx"  // WiFi Secret

#define MQTT_SERVER "xxx.xxx.xxx.xxx" // MQTT Broker Server
#define MQTT_PORT 1883                // MQTT Broker Port

WiFiClient espClient;
PubSubClient client(espClient); // MQTT Node




// Lora Module setup
bool initLora() {
  SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_SS);   // SPI interface
  LoRa.setPins(LORA_SS, LORA_RST, LORA_DI0);            // Lora Module Pin setup

  return LoRa.begin(LoRa_BAND);
}

// Sendet Daten über MQTT an den Server
void sendMqtt(String dev_id, String channel, String payloud) {

  do {
    Serial.print(".");
    // Try to connect - if fail repeat
    if (client.connect("ESP32_Gateway" + random(111, 999))) {

      String topic = "raw/" + dev_id + "/" + channel;   // Topic = ID + Channel
      Serial.println("MQTT publish to topic " + topic); // Debug
      client.publish(topic.c_str(), payloud.c_str());   // Send Data
    }
    delay(500);
  } while (!client.connected());

}

// -- Setup --
void setup() {

  Serial.begin(9600);
  Serial.println("CoBoy LoRa Receiver");

  client.setServer(MQTT_SERVER, MQTT_PORT); // MQTT Client

  // Connect to WLAN
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.print("WiFi connected. IP address: ");
  Serial.println(WiFi.localIP());
  delay(1500);

  // Lora Module init
  if (!initLora()) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }

}

// ------
// Main loop
void loop() {

  int packetSize = LoRa.parsePacket();

  // Try till receive package
  if (packetSize) {
    // received a packet
    Serial.print("Received packet '");

    int payloud_size = LoRa.available();
    char buffer[payloud_size];

    Serial.print("Payloud size: ");
    Serial.println(payloud_size);



    // read packet into buffer
    for (int i = 0; (i < payloud_size) &&  LoRa.available(); i++) {
      buffer[i] = (char) LoRa.read();
    }

    // TODO: AES128 signatur

    // deserialize ArduinoJson
    StaticJsonDocument<JSON_OBJECT_SIZE(4)> doc;
    // Deserialize the JSON document
    DeserializationError error = deserializeJson(doc, buffer);

    // Test if parsing succeeds.
    if (error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
      return;
    }

    // Save deserialized Json into variables
    int device_id = doc["device_id"];
    String id_hex =  String(device_id, HEX); // Parse MacAdress mumber into string in hex representation
    uint16_t co2 = doc["co2"];
    float temperature = doc["temperature"];
    float humidity = doc["humidity"];

    //    Serial.println(device_id);
    //    Serial.println(String(device_id, HEX));
    //    Serial.println(co2);
    //    Serial.println(temperature);
    //    Serial.println(humidity);

    // Send data over MQTT
    sendMqtt(id_hex, "co2", String(co2));
    sendMqtt(id_hex, "temp", String(temperature));
    sendMqtt(id_hex, "humidity", String(humidity));



  }

}
