#ifndef __CONNECT_H__
#define __CONNECT_H__

#include "commons.h"

int connect_once(int domain, int type, int protocol, const struct sockaddr * addr, socklen_t alen);
int connect_retry(int domain, int type, int protocol, const struct sockaddr * addr, socklen_t alen);

#endif
