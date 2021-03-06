#include "commons.h"
#include "debug.h"

extern const char* nic_device;
extern int interval_time;
extern int timeout_time;
extern int host_mode;

void initScanner();
void init_ip_pool(const char* addr);
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
	char *port = NULL, *type = NULL;
    /* Must run as root. */
    if (geteuid())
    {
        fprintf(stderr, "Please run this program as root user. You may use sudo!\n");
        return 1;
    }

    initScanner();
	// opterr = 0;
	while ((ch = getopt(argc, argv, "Hi:s:p:ht:T:")) != -1)	
	{
		switch (ch)
		{
            case 'H':
                host_mode = 1; 
                break;
            // NIC: eth0, wlan0, ...
            case 'i':
                Log("option i: %s\n", optarg);
                nic_device = optarg; 
                break;
            // Scan Type: Connect, SYN, ACK, UDP
			case 's':
				Log("option s: %s\n", optarg);
				type = strdup(optarg);	
				break;
            // Interval time 
            case 't':
                Log("option t: %s\n", optarg);
                interval_time = atoi(optarg);
                break;
            // Port
			case 'p':
				Log("option p: %s\n", optarg);
				port = strdup(optarg);
				break;
            // Timeout time
            case 'T':
                Log("option T: %s\n", optarg);
                timeout_time = atoi(optarg);
                break;
            // Help
			case 'h':
				help();
				break;
		}
	}
	doScan(argv[optind], port, type);
	return 0;
	
}
