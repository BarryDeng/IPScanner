#include "commons.h"
#include "scans.h"

const char* nic_device = NULL;
const int interval_time = 0;
const int timeout_time = 5;

char* scanType[] = {"SYN", "ACK", "Connect", "UDP"};
void (*scanFunc[])(char*, int*) = {synScan, ackScan, connectScan, udpScan};
const int SCAN_LEN = sizeof(scanType) / sizeof(scanType[0]);

void (*scanDispatcher(char* type)) (char*, int*)
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
	void (*fun)(char*, int*);
	if (!(fun = scanDispatcher(type)))
	{
		printf("Wrong type!\n");	
	}
	else
	{
		int p = atoi(port);
        int ports[1] = {p};
		fun(addr, ports);	
	}
	
}
