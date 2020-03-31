#include "config.h"
#include "BLE.h"
#include "DagrQueues.h"
#include "DagrPBConstants.h"
#include <pb_common.h>
#include <pb_decode.h>
#include <pb_encode.h>

int i = 0;
bool _BLEClientConnected = false;

static uint8_t message_buf[DagrPacket_size];

class MyServerCallbacks : public BLEServerCallbacks
{
    void onConnect(BLEServer *pServer)
    {
        _BLEClientConnected = true;
        Serial.println("Device Connected");
    };

    void onDisconnect(BLEServer *pServer)
    {
        _BLEClientConnected = false;
        Serial.println("Device Disconnected");
    }
};

class WriteCharacteristicCallbacks : public BLECharacteristicCallbacks
{
    void onRead(BLECharacteristic *pCharacteristic)
    {
        Serial.println("Got Read Request from Client");
    }

    void onWrite(BLECharacteristic *pCharacteristic)
    {
        Serial.println("Got Write Request from Client");
        std::string current = pCharacteristic->getValue();
        static ChatMessage cm; // this is a static scratch object, any data must be copied elsewhere before returning

        if (pb_decode_from_bytes((const uint8_t *)current.c_str(), current.length(), ChatMessage_fields, &cm))
        {
            DagrQueues::Instance()->sendQueue.push(cm);
            Serial.write("Added to Queue");
            pCharacteristic->setValue("");
        }
    }
};

BLE::BLE(const char *deviceName)
{
    name = std::string(deviceName);
}

void BLE::setup()
{
    BLEDevice::init(name);
    pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks());
    dagrService = pServer->createService("a40a4466-5444-4fab-b012-16f820b749a8");
    readCharacteristic = dagrService->createCharacteristic("d73d98d8-6e1a-46b9-a949-d174d89ee10d",
                                                           BLECharacteristic::PROPERTY_READ |
                                                               BLECharacteristic::PROPERTY_INDICATE);
    writeCharacteristic = dagrService->createCharacteristic("c79c596a-2580-48db-b398-27215023882d",
                                                            BLECharacteristic::PROPERTY_WRITE |
                                                                BLECharacteristic::PROPERTY_INDICATE);

    writeCharacteristic->setCallbacks(new WriteCharacteristicCallbacks());
    readCharacteristic->addDescriptor(new BLE2902());
    writeCharacteristic->addDescriptor(new BLE2902());
    dagrService->start();
    dagrAdvertising = pServer->getAdvertising();
    dagrAdvertising->start();
}

void BLE::loop()
{
    // std::ostringstream testString;
    // testString << "Test " << i++;
    // readCharacteristic->setValue(testString.str());
    // readCharacteristic->indicate();
    ChatMessage last_recieved = ChatMessage();
    if (!DagrQueues::Instance()->sendQueue.empty())
    {
        last_recieved = DagrQueues::Instance()->sendQueue.back();
    }

    // uint8_t *current = writeCharacteristic->getData();
    // pb_istream_t stream = pb_istream_from_buffer(current, sizeof(current));
    // ChatMessage current_message;
    // if (!pb_decode(&stream, ChatMessage_fields, &current_message))
    // {
    //     Serial.println("Decode Failed");

    //     return;
    // }

    // if (!compare_chat_message(&current_message, &last_recieved))
    // {

    //     DagrQueues::Instance()->sendQueue.push(current_message);
    //     Serial.write("Added to Queue");
    //     writeCharacteristic->setValue("");
    // }
    if (!DagrQueues::Instance()->recieveQueue.empty())
    {
        Serial.println("Message Sending to BLE");
        size_t numbytes = pb_encode_to_bytes(message_buf, sizeof(message_buf), ChatMessage_fields, &DagrQueues::Instance()->recieveQueue.front());

        readCharacteristic->setValue(message_buf, numbytes);
        readCharacteristic->indicate();
        DagrQueues::Instance()->recieveQueue.pop();
    }

    // Serial.print("Send Queue: ");
    // DagrQueues::printQueue(DagrQueues::Instance()->sendQueue);
}

bool BLE::isDeviceConnected()
{
    return _BLEClientConnected;
}

BLE::~BLE()
{
}
