#include "commons.h"

extern const char* nic_device;
static libnet_t* netctx = NULL;
static uint32_t ip_src;
static uint8_t* mac_src;
static const char* nic_name;

void init_net_ctx()
{
    char errbuf[LIBNET_ERRBUF_SIZE];
    
    // init net context
    netctx = libnet_init(LIBNET_RAW4, nic_device, errbuf);
    if (!netctx)
    {
        fprintf(stderr, "Device %s error: %s\n", nic_device, errbuf);
        exit(1);
    }

    libnet_seed_prand(netctx);
    if ((ip_src = libnet_get_ipaddr4(netctx)) == -1)
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


