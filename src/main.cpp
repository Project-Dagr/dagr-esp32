#include "config.h"
#include "BLE.h"
#include "LORA.h"

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
LORA *lora = new LORA();

void setup()
{
    Serial.begin(115200);
    Serial.write(getDeviceName());

    pinMode(LED_PIN, OUTPUT);
    
    bluetooth->setup();
    lora->setup();
}

void loop()
{
    // while (bluetooth->isDeviceConnected())
    // {
        if(bluetooth->isDeviceConnected()){
            Serial.println("Device Connected");
        }else{
            Serial.println("Device disconnected");

        }
        bluetooth->loop();
        lora->loop();
        // delay(1000);
    // }
}
