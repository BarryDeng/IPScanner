#include "commons.h"
#include "debug.h"

extern const char* nic_device;
extern uint32_t ip_pool_start;
extern uint32_t ip_pool_end;
extern uint32_t ip_pool_num;
extern int pcap_inited;

static int ctx_inj;
static libnet_t* netctx = NULL;
static uint32_t ip_src;
static uint8_t* mac_src;
uint8_t gateway_mac_src[6];
int gateway_mac_src_set = 0;

static const char* nic_name;
uint32_t tcp_no;
static libnet_ptag_t tcp_tag = 0;
static libnet_ptag_t ip_tag = 0;
static libnet_ptag_t mac_tag = 0;

void init_net_ctx(int inj_type)
{
    char errbuf[LIBNET_ERRBUF_SIZE];
    ctx_inj = inj_type;

    // init net context
    netctx = libnet_init(inj_type, nic_device, errbuf);
    if (!netctx)
    {
        fprintf(stderr, "Device %s error: %s\n", nic_device, errbuf);
        exit(1);
    }

    libnet_seed_prand(netctx);
    /* Get a random number for TCP Seq Num. So that we can use pcap program to get the response packet to the request packet. */
    tcp_no = libnet_get_prand(LIBNET_PRu32);
    Log("TCP NO: %u", tcp_no);

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
        uint32_t mask = 1 << (32 - atoi(pos + 1));
        mask -= 1;

        strncpy(ip_addr, addr, pos - addr);
        ip_addr[pos - addr] = '\0';
        if ((ip_addr_int = libnet_name2addr4(netctx, ip_addr, LIBNET_DONT_RESOLVE)) == (uint32_t)-1)
        {
            fprintf(stderr, "Invalid addr %s: %s\n", ip_addr, libnet_geterror(netctx));
            exit(1);        
        }

        ip_pool_start = (~mask & ntohl(ip_addr_int)) + 1; 
        ip_pool_end = (mask | ntohl(ip_addr_int)) - 1;
        ip_pool_num = mask - 1;
        Log("Start: %s End: %s", libnet_addr2name4(htonl(ip_pool_start), LIBNET_DONT_RESOLVE), libnet_addr2name4(htonl(ip_pool_end), LIBNET_DONT_RESOLVE));

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

void sendSYN(uint32_t dst, uint16_t dp)
{
    // Log("Seed SYN to %s:%u", libnet_addr2name4(dst, LIBNET_DONT_RESOLVE), dp);
    uint32_t src = ip_src;
    uint16_t sp = libnet_get_prand(LIBNET_PRu16);

    tcp_tag = libnet_build_tcp(
            sp, /* source port */
            dp, /* dest port */
            tcp_no, /* TCP seq num */
            0, /* ACK */
            TH_SYN, /* FLAGS */
            1024, /* Window */
            0, /* checksum */
            0, /* URG */
            LIBNET_TCP_H, /* Header length */
            NULL, /* Payload */
            0, /* Payload length*/                
            netctx, /* Context */
            tcp_tag /* Tag */
            );
    if (tcp_tag == -1)
    {
        fprintf(stderr, "Build TCP Header Error: %s\n", libnet_geterror(netctx));
        exit(1);
    }

    ip_tag = libnet_build_ipv4(
            LIBNET_TCP_H + LIBNET_IPV4_H, /* Header length */
            0, /* TOS */
            libnet_get_prand(LIBNET_PRu16), /* IP seq num */
            0,    /* Frag offset */
            127,    /* TTL */
            IPPROTO_TCP,    /* Upper layer protocol */
            0,    /* Checksum */
            src,    /* Src IP */
            dst,    /* Dst IP */
            NULL,    /* Payload */
            0,    /* Payload length */
            netctx,    /* Context */
            ip_tag /* IP tag */
            ); 

    if (ip_tag == -1)
    {
        fprintf(stderr, "Build IP Header Error: %s\n", libnet_geterror(netctx));
        exit(1);
    }

    if (netctx == LIBNET_LINK)
    {
        mac_tag = libnet_build_ethernet(
                    gateway_mac_src,
                    mac_src,
                    ETHERTYPE_IP,
                    NULL,
                    0,
                    netctx,
                    mac_tag
                ); 
        if (mac_tag == -1)
        {
            fprintf(stderr, "Build MAC Header Error: %s\n", libnet_geterror(netctx));
            exit(1);
        }
    }

    if (libnet_write(netctx) == -1)
    {
        Log("Send packet Failed!");
    }

}

void sendUDP(uint32_t dst, uint16_t dp)
{
}

void sendACK(uint32_t dst, uint16_t dp)
{
}
