#include "config.h"
#include "BLE.h"
#include "DagrQueues.h"
#include <ArduinoJson.h>

int i = 0;
bool _BLEClientConnected = false;
const size_t capacity = JSON_OBJECT_SIZE(3) + 30;

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
        DynamicJsonDocument message(256);

        DeserializationError error = deserializeMsgPack(message, current);
        if (error)
        {
            Serial.print("Deserialization failed with error: ");
            Serial.println(error.c_str());
            return;
        }

        int to = message["to"];
        int from = message["from"];
        int8_t payload = message["payload"];
        Serial.println(to);
        Serial.println(from);
        Serial.println(payload);
        // DagrQueues::Instance()->sendQueue.push(message);
        // if (pb_decode_from_bytes((const uint8_t *)current.c_str(), current.length(), ChatMessage_fields, &cm))
        // {
        //     DagrQueues::Instance()->sendQueue.push(cm);
        //     Serial.write("Added to Queue");
        //     pCharacteristic->setValue("");
        // }
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

    // ChatMessage last_recieved = ChatMessage();
    // if (!DagrQueues::Instance()->sendQueue.empty())
    // {
    //     last_recieved = DagrQueues::Instance()->sendQueue.back();
    // }

    // if (!DagrQueues::Instance()->recieveQueue.empty())
    // {
    //     Serial.println("Message Sending to BLE");
    //     size_t numbytes = pb_encode_to_bytes(message_buf, sizeof(message_buf), ChatMessage_fields, &DagrQueues::Instance()->recieveQueue.front());

    //     readCharacteristic->setValue(message_buf, numbytes);
    //     readCharacteristic->indicate();
    //     DagrQueues::Instance()->recieveQueue.pop();
    // }
}

bool BLE::isDeviceConnected()
{
    return _BLEClientConnected;
}

BLE::~BLE()
{
}
