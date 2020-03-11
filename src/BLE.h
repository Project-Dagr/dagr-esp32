#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>
#include <string>
#include <sstream>

class BLE
{
private:
    /* data */
    std::string name;
    BLEServer *pServer;
    BLEService *dagrService;
    BLECharacteristic *readCharacteristic;
    BLECharacteristic *writeCharacteristic;
    BLEAdvertising *dagrAdvertising;

public:
    BLE(const char *deviceName);
    void setup();
    void loop();
    bool isDeviceConnected();
    ~BLE();
};


