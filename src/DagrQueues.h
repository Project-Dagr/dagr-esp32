#include "config.h"
#include <queue>
#include <ArduinoJson.h>

class DagrQueues
{
private:
    DagrQueues() {}
    DagrQueues(DagrQueues const &){};            // copy constructor is private
    DagrQueues &operator=(DagrQueues const &){}; // assignment operator is private

    static DagrQueues *m_pInstance;

public:
    struct uint8_buf
    {
        uint8_t data;
        size_t size;
    };
    
    static DagrQueues *Instance();
    std::queue<std::vector<uint8_t>> sendQueue;
    std::queue<std::vector<uint8_t>> recieveQueue;
    static void printQueue(std::queue<ChatMessage> q);
};
