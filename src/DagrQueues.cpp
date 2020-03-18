#include "DagrQueues.h"

DagrQueues* DagrQueues::m_pInstance = NULL;

DagrQueues* DagrQueues::Instance(){
    if(!m_pInstance){
        m_pInstance = new DagrQueues;

    }
    return m_pInstance;
}

// void DagrQueues::printQueue(std::queue<ChatMessage> q)
// {
//     //printing content of queue
//     while (!q.empty())
//     {
//         // Serial.print(" ");
//         Serial.write(q.front().message);
//         q.pop();
//     }
//     Serial.println("");
// }