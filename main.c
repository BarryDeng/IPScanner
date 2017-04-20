#include "commons.h"
#include "debug.h"

void doScan(char*, char*, char*);
void help()
{
	puts("Usage: PROG_NAME [-p port] [-t type] addr");
	puts("port: port number in 1-65535");
	puts("type: SYN, ACK, Connect, UDP, ICMP");
}


int main(int argc, char* argv[])
{
	int ch;
	char *port, *type;
	// opterr = 0;
	while ((ch = getopt(argc, argv, "t:p:h")) != -1)	
	{
		switch (ch)
		{
			case 't':
				Log("option a: %s\n", optarg);
				type = strdup(optarg);	
				break;
			case 'p':
				Log("option b: %s\n", optarg);
				port = strdup(optarg);
				break;
			case 'h':
				help();
				break;
		}
	}
	doScan(argv[optind], port, type);
	return 0;
	
}
