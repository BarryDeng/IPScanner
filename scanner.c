#include "commons.h"
#include "debug.h"
#include "scans.h"
#include "packetSender.h"

#define MAX_PORT_NUM 20
int ALL_PORTS[65536];

const char* nic_device = NULL;
const int interval_time = 0;
const int timeout_time = 5;

uint32_t ip_pool_start;
uint32_t ip_pool_end;
uint32_t ip_pool_num;

char* scanType[] = {"SYN", "ACK", "Connect", "UDP"};
void (*scanFunc[])(uint32_t, int*, int) = {synScan, ackScan, connectScan, udpScan};
const int SCAN_LEN = sizeof(scanType) / sizeof(scanType[0]);

void initScanner()
{
    for (int i = 1; i <= 65535; ++i)
    {
        ALL_PORTS[i - 1] = i;
    }
}


void (*scanDispatcher(char* type)) (uint32_t, int*, int)
{
	int i = 0;
	for (; i < SCAN_LEN; ++i)
	{
		if (!strcmp(type, scanType[i]))
		{
			return scanFunc[i];
		}	
	} 	
	return NULL;
}


void doScan(char* addr, char *port, char* type)
{
	void (*fun)(uint32_t, int*, int);
    if (!type)
    {
        fprintf(stderr, "You must choose a scan type! e.g. Connect, SYN, ACK, UDP.\n");
        exit(1);
    }


    int* ports, portnum;
    if (!port)
    {
        ports = ALL_PORTS;    
        portnum = 65535;
    }
    else
    {
        // TODO: add more ports support. 
        ports = (int*)malloc(MAX_PORT_NUM * sizeof(int));
		int p = atoi(port);
        *ports = p;
        portnum = 1;
    }

	if (!(fun = scanDispatcher(type)))
	{
		printf("Wrong type!\n");	
	}
	else
	{
        init_ip_pool(addr);
        for (uint32_t i = 0; i < ip_pool_num; ++i)
        {
            Log("NOW %s", libnet_addr2name4(ntohl(ip_pool_start + i), LIBNET_DONT_RESOLVE));
		    fun(get_ip(i), ports, portnum);	
        }
	}
	
}
