#include "commons.h"
#include "debug.h"
#include "packetSender.h"

void synScan(uint32_t addr, int* port, int portnum)
{
    for (int i = 0; i < portnum; ++i)
    {
        sendSYN(addr, port[i]); 
    }
}
