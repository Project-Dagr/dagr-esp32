#include "DagrQueues.h"

DagrQueues* DagrQueues::m_pInstance = NULL;

DagrQueues* DagrQueues::Instance(){
    if(!m_pInstance){
        m_pInstance = new DagrQueues;

    }
    return m_pInstance;
}