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
    std::queue<std::string> sendQueue;
    std::queue<std::string> recieveQueue;
    static void printQueue(std::queue<std::string> q);
};
