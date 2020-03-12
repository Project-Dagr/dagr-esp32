#include "config.h"
#include "BLE.h"

const char *getDeviceName()
{
    uint64_t chipID = ESP.getEfuseMac();
    // String deviceName = "Dagr-";
    // deviceName += (uint16_t)(chipID>>32);

    static char name[20];
    sprintf(name, "Dagr_%d", (uint16_t)(chipID >> 32));
    return name;
}

BLE *bluetooth = new BLE(getDeviceName());

void setup()
{
    Serial.begin(115200);
    Serial.write(getDeviceName());

    pinMode(LED_PIN, OUTPUT);
    
    bluetooth->setup();
}

void loop()
{
    while (bluetooth->isDeviceConnected())
    {
        bluetooth->loop();
        delay(1000);
    }
}
