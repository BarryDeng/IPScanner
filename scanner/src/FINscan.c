#include "commons.h"
#include "debug.h"
#include "packetSender.h"
#include "packetHandler.h"

struct _addr {
    uint32_t ip;
    int portn;
    int* port;
};

void sendFinHelper(void * args)
{
    struct _addr addr = *(struct _addr *)args;
    sleep(BEGIN_SLEEP_TIME);
    for (int i = 0; i < addr.portn; ++i)
    {
        sendFIN(addr.ip, *(addr.port + i));
        usleep(PACKET_SLEEP_TIME);
    }
}

void finScan(uint32_t addr, int* port, int portnum)
{
    pthread_t tid[portnum / PORTS_PER_THREAD + 1];
    int ret1;
    struct _addr t_addr;
    t_addr.ip = addr;
    void* tret;

    for (int i = 0, j = 0; i < portnum; i += PORTS_PER_THREAD, j++)
    {
        t_addr.portn = i + PORTS_PER_THREAD < portnum ? PORTS_PER_THREAD : portnum - i;
        t_addr.port = port + i; 
        ret1 = pthread_create(tid + j, NULL, (void *)&sendFinHelper, (void *)&t_addr); 
        if (ret1)
        {
            fprintf(stderr, "Thread create error!");
            exit(1);
        }
        break;
    }

    for (int j = 0; j < portnum / PORTS_PER_THREAD + 1; j++)
    {
        ret1 = pthread_join(tid[j], &tret);
        if (ret1)
        {
            fprintf(stderr, "Thread join error");
            exit(1);
        }
    }

}
