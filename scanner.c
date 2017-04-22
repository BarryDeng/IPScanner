#include "commons.h"
#include "scans.h"

char* scanType[] = {"SYN", "ACK", "Connect", "UDP"};
void (*scanFunc[])(char*, int*) = {synScan, ackScan, connectScan, udpScan};

void (*scanDispatcher(char* type)) (char*, int*)
{
	int i = 0;
	for (; i < sizeof(scanType) / sizeof(scanType[0]); ++i)
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
