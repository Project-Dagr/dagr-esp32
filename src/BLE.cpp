#include "config.h"
#include "BLE.h"
#include "DagrQueues.h"
#include <pb_common.h>
#include <pb_decode.h>
#include <pb_encode.h>

int i = 0;
bool _BLEClientConnected = false;

class MyServerCallbacks : public BLEServerCallbacks
{
    void onConnect(BLEServer *pServer)
    {
        _BLEClientConnected = true;
    };

    void onDisconnect(BLEServer *pServer)
    {
        _BLEClientConnected = false;
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

    uint8_t *current = writeCharacteristic->getData();
    pb_istream_t stream = pb_istream_from_buffer(current, sizeof(current));
    ChatMessage current_message;
    pb_decode(&stream, ChatMessage_fields, &current_message);

    if (current_message != last_recieved)
    {

        DagrQueues::Instance()->sendQueue.push(current_message);
        Serial.write("Added to Queue");
        writeCharacteristic->setValue("");
    }
    if (!DagrQueues::Instance()->recieveQueue.empty())
    {

        pb_byte_t output[ChatMessage_size];
        pb_ostream_t ostream = pb_ostream_from_buffer(output, sizeof(output));

        if (!pb_encode(&ostream, ChatMessage_fields, &DagrQueues::Instance()->recieveQueue.front()))
        {
            // NRF_LOG_ERROR("Unable to encode: %s", PB_GET_ERROR(&ostream));
            return;
        }

        readCharacteristic->setValue(output, sizeof(output));
        readCharacteristic->indicate();
        DagrQueues::Instance()->recieveQueue.pop();
    }

    Serial.print("Send Queue: ");
    // DagrQueues::printQueue(DagrQueues::Instance()->sendQueue);
}

bool BLE::isDeviceConnected()
{
    return _BLEClientConnected;
}

BLE::~BLE()
{
}
