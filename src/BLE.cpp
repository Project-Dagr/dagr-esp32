#include "config.h"
#include "BLE.h"
#include "DagrQueues.h"



int i = 0;
bool _BLEClientConnected = false;


class MyServerCallbacks : public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      _BLEClientConnected = true;
    };

    void onDisconnect(BLEServer* pServer) {
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
    dagrService->start();                                                             
    dagrAdvertising = pServer->getAdvertising();
    dagrAdvertising->start();                                                         
}
void printQueue(std::queue<std::string> q)
{
	//printing content of queue 
	while (!q.empty()){
		Serial.print(" ");
		Serial.write(q.front().c_str());
		q.pop();
	}
	Serial.write("\n");
}

void BLE::loop()
{
    std::ostringstream testString;
    testString << "Test " << i++;
    readCharacteristic->setValue(testString.str());
    readCharacteristic->indicate();
    Serial.printf(writeCharacteristic->getValue().c_str());
    std::string last_recieved = "";
    if(!DagrQueues::Instance()->sendQueue.empty())
        last_recieved = DagrQueues::Instance()->sendQueue.back();
    if(writeCharacteristic->getValue() != last_recieved){
        DagrQueues::Instance()->sendQueue.push(writeCharacteristic->getValue());
        // writeCharacteristic->setValue(0);
    }
    printQueue(DagrQueues::Instance()->sendQueue);
}

bool BLE::isDeviceConnected(){
    return _BLEClientConnected;
}

BLE::~BLE()
{
}

