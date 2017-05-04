#include "commons.h"
#include "debug.h"
#include "packetSender.h"
#include "packetHandler.h"

struct _addr {
    uint32_t ip;
    int port;
};

void sendHelper(void * args)
{
    struct _addr addr = *(struct _addr *)args;
    sendSYN(addr.ip, addr.port);
}

void synScan(uint32_t addr, int* port, int portnum)
{
    pthread_t p1;
    int ret1;
    struct _addr t_addr;
    t_addr.ip = addr;
    t_addr.port = port[0];

    for (int i = 0; i < portnum; ++i)
    {
        ret1 = pthread_create(&p1, NULL, (void *)&sendHelper, (void *)&t_addr); 
    }
    start_pcap();

}
