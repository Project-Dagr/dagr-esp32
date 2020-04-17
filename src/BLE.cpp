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
        std::vector<uint8_t> v(current.begin(), current.end());
        DynamicJsonDocument message(256);

        DeserializationError error = deserializeMsgPack(message, v);
        if (error)
        {
            Serial.print("Deserialization failed with error: ");
            Serial.println(error.c_str());
            return;
        }
        JsonObject test = message.to<JsonObject>();

        std::string to = message["dest"];
        const char *from = test["source"];
        std::string payload = message["payload"];
        Serial.println((const char)message["dest"]);
        Serial.println(from);
        Serial.println(payload.c_str());
        std::string testing;
        serializeJsonPretty(message, testing);
        Serial.println(testing.c_str());
        // struct DagrQueues::uint8_buf data = {(uint8_t) *current.c_str(), current.length()};
        DagrQueues::Instance()->sendQueue.push(v);
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

    if (!DagrQueues::Instance()->recieveQueue.empty())
    {
        Serial.println("Message Sending to BLE");
        DynamicJsonDocument message(256);
        std::vector<uint8_t> current = DagrQueues::Instance()->recieveQueue.front();

        DeserializationError error = deserializeMsgPack(message, current);
        if (error)
        {
            Serial.print("Deserialization failed with error: ");
            Serial.println(error.c_str());
            return;
        }

        int to = message["to"];
        int from = message["from"];
        std::string payload = message["payload"];
        // Serial.println(message.data());
        Serial.println(to);
        Serial.println(from);
        Serial.println(payload.c_str());
        for (int i = 0; i < current.size(); i++)
            Serial.printf("%d, ", current.at(i));
        Serial.println();
        Serial.println(current.size());

        readCharacteristic->setValue(current.data(), current.size());
        // delay(500);
        readCharacteristic->indicate();
        DagrQueues::Instance()->recieveQueue.pop();
    }
}

bool BLE::isDeviceConnected()
{
    return _BLEClientConnected;
}

BLE::~BLE()
{
}
