#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include "debug.h"
#include "connect.h"
#include "packetSender.h"


void connectScan(uint32_t addr, int* port, int portnum)
{
    int fd;
    struct sockaddr_in sockaddr;
    memset(&sockaddr, 0, sizeof(sockaddr));
    sockaddr.sin_family = AF_INET;
    sockaddr.sin_addr.s_addr = addr;
    Log("Portnum: %d", portnum);
    char ip_addr[20];
    /*inet_pton(AF_INET, addr, &sockaddr.sin_addr);*/
    for (int i = 0; i < portnum; ++i)
    {
        sockaddr.sin_port = htons(port[i]);
        // Log("%d", ntohl(sockaddr.sin_port));
        if ((fd = connect_once(AF_INET, SOCK_STREAM, 0, (struct sockaddr *)&sockaddr, sizeof(sockaddr))) < 0)
        {
            // Log("%u:%d Failed", addr, port[i]);
        }
        else
        {   
            inet_ntop(AF_INET, (void *)&addr, ip_addr, INET_ADDRSTRLEN);
            Log("%s:%u OK", ip_addr, port[i]);
        }
    }

}


