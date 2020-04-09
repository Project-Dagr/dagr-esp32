#include "LORA.h"
#include "DagrQueues.h"
#include <iostream>
#include <sstream>
#include "./helper.h"

RH_RF95 rf95(RFM95_CS, RFM95_INT);

#define MAX_RHPACKETLEN 251
static uint8_t radiobuf[MAX_RHPACKETLEN];

LORA::LORA() {}

void LORA::setup()
{
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);

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
}

void LORA::loop()
{
  if (DagrQueues::Instance()->sendQueue.size() > 0)
  {
    // size_t numbytes = pb_encode_to_bytes(radiobuf, sizeof(radiobuf), ChatMessage_fields, &DagrQueues::Instance()->sendQueue.front());
    Serial.println("Sending over LORA");
    std::vector<uint8_t> radio_buffer = DagrQueues::Instance()->sendQueue.front();
    DynamicJsonDocument message(256);
 

    DeserializationError error = deserializeMsgPack(message, radio_buffer);
    Serial.printf("test");
    if (error)
    {
      Serial.print("Deserialization failed with error: ");
      Serial.println(error.c_str());
      return;
    }

    int to = message["to"];
    int from = message["from"];
    std::string payload = message["payload"];

    int res = rf95.send((uint8_t *)radio_buffer.data(), radio_buffer.size());
    assert(res);
    DagrQueues::Instance()->sendQueue.pop();
  }
  if (rf95.available())
  {
    Serial.println("Message Available");
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    if (rf95.waitAvailableTimeout(1000))
    {
      // Should be a reply message for us now
      if (rf95.recv(buf, &len))
      {
        std::vector<uint8_t> v;
        for (size_t i = 0; i < len; i++)
        {
          v.push_back(buf[i]);
        }
        
        DynamicJsonDocument message(256);

        DeserializationError error = deserializeMsgPack(message, v);
        if (error)
        {
          Serial.print("Deserialization failed with error: ");
          Serial.println(error.c_str());
          return;
        }

        int to = message["to"];
        int from = message["from"];
        std::string payload = message["payload"];
        Serial.println(to);
        Serial.println(from);
        Serial.println(payload.c_str());

        // std::string data = convertToString((char *)buf, len);

        // struct DagrQueues::uint8_buf data = {*buf, len};
        DagrQueues::Instance()->recieveQueue.push(v);
        // ChatMessage message_recieved;
        // if (!pb_decode_from_bytes(buf, len, ChatMessage_fields, &message_recieved))
        // {
        //   Serial.println("Decode Failed");
        // }
        // else
        // {
        //   DagrQueues::Instance()->recieveQueue.push(message_recieved);
        // }
        // Serial.print("Got Message: ");
        // Serial.println((char *)buf);
        // Serial.print("RSSI: ");
        // Serial.println(rf95.lastRssi(), DEC);
      }
      else
      {
        Serial.println("Receive failed");
      }
    }
  }
  // Serial.print("Recieve Queue: ");
  // DagrQueues::printQueue(DagrQueues::Instance()->recieveQueue);
}
