#include "commons.h"
#include "debug.h"
#include "connect.h"
#include "packetSender.h"

char* hostlookup(struct in_addr in)
{
    static char blah[1024];
    struct hostent *he;
    he = gethostbyaddr((char *)&in, sizeof(struct in_addr), AF_INET);
    if (!he)
    {
        strcpy(blah, inet_ntoa(in));
    }
    else
    {
        strcpy(blah, he->h_name);
    }
    return blah;
}

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
        // Log("%d", ntohs(sockaddr.sin_port));
        if ((fd = connect_once(AF_INET, SOCK_STREAM, 0, (struct sockaddr *)&sockaddr, sizeof(sockaddr))) < 0)
        {
            // Log("%u:%d Failed", addr, port[i]);
        }
        else
        {   
            printf("%s:%u opened\n", hostlookup(sockaddr.sin_addr), port[i]);
        }
    }

}


