#include "commons.h"
#include "debug.h"
#include "scans.h"
#include "packetSender.h"
#include "packetHandler.h"

#define MAX_PORT_NUM 65536
int ports[MAX_PORT_NUM];

const char* nic_device = NULL;
int interval_time = 0;
int timeout_time = 5;
int host_mode = 0;

int pcap_inited = 0;

uint32_t ip_pool_start;
uint32_t ip_pool_end;
uint32_t ip_pool_num;

char* scanType[] = {"SYN", "ACK", "Connect", "UDP", "FIN"};
void (*scanFunc[])(uint32_t, int*, int) = {synScan, ackScan, connectScan, udpScan, finScan};
char* gScanType;
const int SCAN_LEN = sizeof(scanType) / sizeof(scanType[0]);

void initScanner()
{
}


void (*scanDispatcher(char* type)) (uint32_t, int*, int)
{
    int i = 0;
    for (; i < SCAN_LEN; ++i)
    {
        if (!strcmp(type, scanType[i]))
        {
            gScanType = scanType[i];
            return scanFunc[i];
        }	
    } 	
    return NULL;
}

void* start_pcap_helper(void* arg)
{
    start_pcap(*(int *)arg);
}

void doScan(char* addr, char *port, char* type)
{
    void (*fun)(uint32_t, int*, int);
    if (!type)
    {
        fprintf(stderr, "You must choose a scan type! e.g. Connect, SYN, ACK, UDP.\n");
        exit(1);
    }


    int portnum = 0;
    if (!port)
    {
        for (int i = 1; i <= 65535; ++i)
        {
            ports[portnum++] = i;
        }    
    }
    else
    {
        // DONE: add more ports support. 
        int last_port = 0, now_port = 0;
        char * p = port;
        for (; *p != '\0'; ++p)
        {
            if (isdigit(*p))
            {
                now_port = 10 * now_port + *p - '0';
            }
            else if (*p == ',')
            {
                if (!last_port) last_port = now_port;
                for (int i = last_port; i <= now_port; ++i)
                {
                    ports[portnum++] = i;
                }
                last_port = 0;
                now_port = 0;
            }
            else if (*p == '-')
            {
                if (last_port)
                {
                    fprintf(stderr, "Port invalid");
                    exit(1);
                }
                last_port = now_port;
                now_port = 0;
            }
        }
        if (now_port)
        {
            if (!last_port) last_port = now_port;    
            for (int i = last_port; i <= now_port; ++i)
            {
                ports[portnum++] = i;
            }
        }
    }

    pthread_t tid;
    if (!(fun = scanDispatcher(type)))
    {
        fprintf(stderr, "Wrong type!\n");	
        exit(1);
    }
    else
    {
        init_ip_pool(addr);
        int capnum = ip_pool_num * portnum;
        void* status;

        if (fun != connectScan)
        {
            init_net_ctx(LIBNET_RAW4);
            init_pcap_ctx(nic_device);

            pthread_create(&tid, NULL, start_pcap_helper, (void *)&capnum);

            while (!pcap_inited);
        }
        for (uint32_t i = 0; i < ip_pool_num; ++i)
        {
            // Log("NOW %s", libnet_addr2name4(htonl(ip_pool_start + i), LIBNET_DONT_RESOLVE));
            fun(get_ip(i), ports, portnum);	
        }
        alarm(15);
        if (fun != connectScan) 
            pthread_join(tid, status);
    }

}
