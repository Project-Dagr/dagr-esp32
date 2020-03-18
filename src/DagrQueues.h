#include "config.h"
#include <queue>

class DagrQueues
{
private:
    DagrQueues() {}
    DagrQueues(DagrQueues const &){};            // copy constructor is private
    DagrQueues &operator=(DagrQueues const &){}; // assignment operator is private

    static DagrQueues *m_pInstance;

public:
    static DagrQueues *Instance();
    std::queue<ChatMessage> sendQueue;
    std::queue<ChatMessage> recieveQueue;
    static void printQueue(std::queue<ChatMessage> q);
};
