#include "commons.h"

extern const char* nic_device;
extern uint32_t ip_pool_start;
extern uint32_t ip_pool_end;
extern uint32_t ip_pool_num;


static libnet_t* netctx = NULL;
static uint32_t ip_src;
static uint8_t* mac_src;
static const char* nic_name;

void init_net_ctx(int inj_type)
{
    char errbuf[LIBNET_ERRBUF_SIZE];
    
    // init net context
    netctx = libnet_init(inj_type, nic_device, errbuf);
    if (!netctx)
    {
        fprintf(stderr, "Device %s error: %s\n", nic_device, errbuf);
        exit(1);
    }

    libnet_seed_prand(netctx);
    if ((ip_src = libnet_get_ipaddr4(netctx)) == (uint32_t)-1)
    {
        fprintf(stderr, "Cannot get IP addr: %s\n", libnet_geterror(netctx));
        exit(1);
    }

    if ((nic_name = libnet_getdevice(netctx)) == NULL)
    {
        fprintf(stderr, "Cannot get NIC name: %s\n", libnet_geterror(netctx));
        exit(1);
    }

    if ((mac_src = (uint8_t *)libnet_get_hwaddr(netctx)) == NULL)
    {
        fprintf(stderr, "Cannot get MAC addr: %s\n", libnet_geterror(netctx));
        exit(1);

    }
}

void init_ip_pool(const char* addr)
{
    char* pos = 0;

    char ip_addr[16];
    uint32_t ip_addr_int;

    if ((pos = strchr(addr, '/')))
    {
        /*Has a mask. */ 
        uint32_t mask = 1 << atoi(pos + 1);
        mask -= 1;

        strncpy(ip_addr, addr, pos - addr);
        if ((ip_addr_int = libnet_name2addr4(netctx, ip_addr, LIBNET_DONT_RESOLVE)) == (uint32_t)-1)
        {
            fprintf(stderr, "Invalid addr %s: %s\n", ip_addr, libnet_geterror(netctx));
            exit(1);        
        }
        
        ip_pool_start = ~mask & ntohl(ip_addr_int); 
        ip_pool_end = mask | ntohl(ip_addr_int);
        ip_pool_num = mask - 1;
         
    }
    else
    {
        /*Only has a IP. */
        strncpy(ip_addr, addr, 16);
        if ((ip_addr_int = libnet_name2addr4(netctx, ip_addr, LIBNET_DONT_RESOLVE)) == (uint32_t)-1)
        {
            fprintf(stderr, "Invalid addr %s: %s\n", ip_addr, libnet_geterror(netctx));
            exit(1);
        }        

        ip_pool_start = ntohl(ip_addr_int);
        ip_pool_end = ntohl(ip_addr_int);
        ip_pool_num = 1;
    }
}

uint32_t get_ip(uint32_t index)
{
    return htonl(ip_pool_start + index);
}
