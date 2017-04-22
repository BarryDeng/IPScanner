#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include "debug.h"

#include <errno.h>

int connect_retry(int domain, int type, int protocol, const struct sockaddr * addr, socklen_t alen);

void connectScan(char* addr, int* port)
{
    int fd;
    struct sockaddr_in sockaddr;
    memset(&sockaddr, 0, sizeof(sockaddr));
    sockaddr.sin_family = AF_INET;
    sockaddr.sin_port = htons(port[0]);
    inet_pton(AF_INET, addr, &sockaddr.sin_addr);
    if ((fd = connect_retry(AF_INET, SOCK_STREAM, 0, (struct sockaddr *)&sockaddr, sizeof(sockaddr))) < 0)
    {
        Log("%s:%d Failed", addr, port[0]);
    }
    else
    {
        Log("%s:%d OK", addr, port[0]);
    }
}

