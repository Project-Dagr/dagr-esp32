#include <Arduino.h>

#include <SPI.h>
#include <RH_RF95.h>
#include "BluetoothSerial.h"
#include <iostream>
#include <string>

// #define SERVICE_UUID "28974a5b-26be-45e4-af01-d8b4375c98d7"
// #define CHARACTERISTIC_UUID "cda5231e-969e-4d4a-be77-dbad3a518875"

#define RFM95_CS 14  // "E"
#define RFM95_RST 32 // "D"
#define RFM95_INT 33 // "B"

#define RF95_FREQ 915.0

// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);

// Blinky on receipt
#define LED 13

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

BluetoothSerial SerialBT;
uint64_t chipID;

void setup()
{
  chipID = ESP.getEfuseMac();
  String deviceName = "Dagr-";
  deviceName += (uint16_t)(chipID>>32);
  pinMode(LED, OUTPUT);
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);

  Serial.begin(115200);
  delay(100);

  Serial.println("DAGR v0.011");

  // manual reset
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);

  while (!rf95.init())
  {
    Serial.println("LoRa radio init failed");
    Serial.println("Uncomment '#define SERIAL_DEBUG' in RH_RF95.cpp for detailed debug info");
    while (1)
      ;
  }
  Serial.println("LoRa radio init OK!");

  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM
  if (!rf95.setFrequency(RF95_FREQ))
  {
    Serial.println("setFrequency failed");
    while (1)
      ;
  }
  Serial.print("Set Freq to: ");
  Serial.println(RF95_FREQ);

  // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on

  // The default transmitter power is 13dBm, using PA_BOOST.
  // If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then
  // you can set transmitter powers from 5 to 23 dBm:
  rf95.setTxPower(23, false);
  

  SerialBT.begin(deviceName); //Bluetooth device name
}

int16_t packetnum = 0; // packet counter, we increment per xmission
bool detectedTouch = false;

void loop()
{
  if (rf95.available())
  {
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    if (rf95.waitAvailableTimeout(1000))
    {
      // Should be a reply message for us now
      if (rf95.recv(buf, &len))
      {
        Serial.print("Got Message: ");
        Serial.println((char *)buf);
        Serial.print("RSSI: ");
        Serial.println(rf95.lastRssi(), DEC);
        // SerialBT.write((uint8_t) buf);
        SerialBT.println((char *)buf);

        // SerialBT.write((char *)buf);
      }
      else
      {
        Serial.println("Receive failed");
      }
    }
  }
  // if (Serial.available())
  // {
  //   SerialBT.write(Serial.read());
  // }
  if (SerialBT.available())
  {
    // Serial.write(SerialBT.read());
    char x;
    std::string radiopacket = "";
    do
    {
      x = SerialBT.read();
      // Serial.println( x, DEC );
      radiopacket += x;

    } while (x != '\r');
    // radiopacket =  SerialBT.readString();
    // radiopacket = SerialBT.readStringUntil('\0');
    radiopacket.pop_back();
    digitalWrite(LED, HIGH);
    detectedTouch = true;
    Serial.println("Transmitting..."); // Send a message to rf95_server
    // char radiopacket[20] = "Hello World #      ";
    // itoa(packetnum++, radiopacket + 13, 10);
    Serial.print("Sending ");
    Serial.println(radiopacket.c_str());
    // radiopacket[19] = 0;

    Serial.println("Sending...");
    delay(10);
    rf95.send((uint8_t *)radiopacket.c_str(), 20);
    // const uint8_t datapacket = atoi(radiopacket.substring(1, 3).c_str());
    // rf95.send((uint8_t *)datapacket, 20);

    Serial.println("Waiting for packet to complete...");
    delay(10);
    rf95.waitPacketSent(1000);
    SerialBT.flush();
    digitalWrite(LED, LOW);
  }
  delay(20);
}