#include "configuration.h"
#include <SPI.h>
#include <RH_RF95.h>
#include "BluetoothSerial.h"
#include <dagr.pb.h>




// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

BluetoothSerial SerialBT;



const char *getDeviceName() {
  uint64_t chipID = ESP.getEfuseMac();
  // String deviceName = "Dagr-";
  // deviceName += (uint16_t)(chipID>>32);

  static char name[20];
  sprintf(name, "Dagr_%d", (uint16_t)(chipID>>32));
  return name;

}

void initBluetooth()
{
  DEBUG_MSG("Starting bluetooth\n");
  SerialBT.begin(getDeviceName());

}

void resetRadio () {
    // manual reset
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);
}

void initRadio(){
  while (!rf95.init())
  {
    DEBUG_MSG("LoRa radio init failed");
    DEBUG_MSG("Uncomment '#define SERIAL_DEBUG' in RH_RF95.cpp for detailed debug info");
    while (1)
      ;
  }
  DEBUG_MSG("LoRa radio init OK!\n");
  if (!rf95.setFrequency(RF95_FREQ))
  {
    DEBUG_MSG("setFrequency failed");
    while (1)
      ;
  }
  DEBUG_MSG("Set Freq to: %s", xstr(RF95_FREQ));

    // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on

  // The default transmitter power is 13dBm, using PA_BOOST.
  // If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then
  // you can set transmitter powers from 5 to 23 dBm:
  rf95.setTxPower(23, false);
}

void loopRadio(){
  
}

void setup() {
  
  // Debug
  #ifdef DEBUG_PORT
    DEBUG_PORT.begin(SERIAL_BAUD);
  #endif

#ifdef LED_PIN 
  pinMode(LED_PIN, OUTPUT);
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);
#endif

// Reset Radio


DEBUG_MSG("%s swver=%s, hwver=%s\n", getDeviceName(), xstr(APP_VERSION), xstr(HW_VERSION));

// BLE Service Init
  initBluetooth();

  resetRadio();
  initRadio();
}

void loop(){
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

  if (SerialBT.available())
  {
    // Serial.write(SerialBT.read());
    char x;
    std::string radiopacket = "";
    char *buffer[255];
    size_t length = 255;
    // do
    // {
    //   x = SerialBT.read();
    //   Serial.println( x, DEC );
    //   radiopacket += x;

    // } while (x != '\r');
    SerialBT.readBytes(buffer, length);
    // Serial.println(str(buffer));
    // radiopacket = SerialBT.readStringUntil('\0');
    // radiopacket.pop_back();
    digitalWrite(LED_PIN, HIGH);
    // Serial.println("Transmitting..."); // Send a message to rf95_server
    // // char radiopacket[20] = "Hello World #      ";
    // // itoa(packetnum++, radiopacket + 13, 10);
    // Serial.print("Sending ");
    // Serial.println(radiopacket.c_str());
    // // radiopacket[19] = 0;

    // Serial.println("Sending...");
    // delay(10);
    // rf95.send((uint8_t *)radiopacket.c_str(), radiopacket.length());
    // // const uint8_t datapacket = atoi(radiopacket.substring(1, 3).c_str());
    // // rf95.send((uint8_t *)datapacket, 20);

    // Serial.println("Waiting for packet to complete...");
    // delay(10);
    // rf95.waitPacketSent(1000);
    SerialBT.flush();
    digitalWrite(LED_PIN, LOW);
  }
  delay(20);
}

