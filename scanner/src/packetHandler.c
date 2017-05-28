#include "packetHandler.h"
#include "debug.h"

extern uint32_t tcp_no;
extern uint8_t gateway_mac_src[6];
extern int gateway_mac_src_set;
extern int pcap_inited;

// BPF Filter
static char filter[100];
static pcap_t* handler;
static bpf_u_int32 net, mask;
static char errbuf[PCAP_ERRBUF_SIZE];
static struct bpf_program bpf_p;

void init_pcap_ctx(const char * interface)
{
    if (!(handler = pcap_open_live(interface, 1500, 0, 2000, errbuf)))
    {
        fprintf(stderr, "Network interface %s open failed: %s\n", interface, errbuf);
        exit(1);
    }
    if (pcap_datalink(handler) != DLT_EN10MB)
    {
        fprintf(stderr, "%s is NOT a ethernet device!\n", interface);
        exit(1);
    }

    if (pcap_setnonblock(handler, 0, errbuf) == -1)
    {
        fprintf(stderr, "Network interface set non-blocking failed: %s\n", errbuf);
        exit(1);
    }
    
    if (pcap_lookupnet(interface, &net, &mask, errbuf) == -1)
    {
        fprintf(stderr, "Network interface net discover failed: %s\n", errbuf);
        exit(1);
    }
    Log("Network address: %s", libnet_addr2name4(net, LIBNET_DONT_RESOLVE));

    snprintf(filter, sizeof(filter), "tcp[8:4] = %u", tcp_no + 1);
    Log("BPF: %s", filter);

    if (pcap_compile(handler, &bpf_p, filter, 0, mask) == -1)
    {
      fprintf(stderr, "BPF compiler error: %s\n", pcap_geterr(handler));
        exit(1);
    }

    if (pcap_setfilter(handler, &bpf_p) == -1)
    {
        fprintf(stderr, "BPF set error: %s\n", pcap_geterr(handler));
        exit(1);
    }
    pcap_freecode(&bpf_p);

}

void packet_handler(unsigned char* user, const struct pcap_pkthdr* packet, const unsigned char* raw)
{
    struct libnet_ethernet_hdr* eth_h = (struct libnet_ethernet_hdr*)raw;
    struct libnet_ipv4_hdr* ip_h = (struct libnet_ipv4_hdr*)(raw + LIBNET_ETH_H);
    struct libnet_tcp_hdr* tcp_h = (struct libnet_tcp_hdr*)((void *)ip_h + LIBNET_IPV4_H);

    if (!gateway_mac_src_set)
    {
        memcpy(gateway_mac_src, eth_h->ether_shost, 6);
        gateway_mac_src_set = 1;
    }    

    // Log("%d %d", tcp_h->th_flags, TH_ACK | TH_RST);

    if (tcp_h->th_flags == (TH_ACK | TH_RST))
    {
    //    fprintf(stdout, "%s:%u closed\n", libnet_addr2name4(ip_h->ip_src.s_addr, LIBNET_DONT_RESOLVE), ntohs(tcp_h->th_sport));
    }
    else if (tcp_h->th_flags == (TH_ACK | TH_SYN))
    {
        fprintf(stdout, "%s:%u opened\n", libnet_addr2name4(ip_h->ip_src.s_addr, LIBNET_RESOLVE), ntohs(tcp_h->th_sport));
    }
}

void start_pcap(int num)
{
    Log("Pcap is OK");
    pcap_inited = 1;
    Log("Capture num: %d", num);
    pcap_dispatch(handler, num, packet_handler, NULL);
}


