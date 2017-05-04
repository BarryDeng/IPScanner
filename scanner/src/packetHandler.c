#include "packetHandler.h"
#include "debug.h"

extern uint32_t tcp_no;
extern uint8_t gateway_mac_src[6];
extern int gateway_mac_src_set;

// BPF Filter
static char filter[100];
static pcap_t* handler;
static bpf_u_int32 net, mask;
static char errbuf[PCAP_ERRBUF_SIZE];
static struct bpf_program bpf_p;

void init_pcap_ctx(char * interface)
{
    if (!(handler = pcap_open_live(interface, 1500, 0, 2000, errbuf)))
    {
        fprintf(stderr, "Network interface %s open failed: %s\n", interface, errbuf);
        exit(1);
    }

    if (pcap_setnonblock(handler, 1, errbuf) == -1)
    {
        fprintf(stderr, "Network interface set non-blocking failed: %s\n", errbuf);
        exit(1);
    }
    
    if (pcap_lookupnet(interface, &net, &mask, errbuf) == -1)
    {
        fprintf(stderr, "Network interface net discover failed: %s\n", errbuf);
        exit(1);
    }

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
    struct ether_header* eth_h = (struct ether_header*)raw;
    struct ip* ip_h = (struct ip*)(raw + LIBNET_ETH_H);
    struct tcphdr* tcp_h = (struct tcphdr*)((void *)ip_h + LIBNET_TCP_H);

    if (!gateway_mac_src_set)
    {
        memcpy(gateway_mac_src, eth_h->ether_shost, 6);
        gateway_mac_src_set = 1;
    }    

    // Log("%d", tcp_h->th_flags);
    Log("ACK: %u", tcp_h->th_ack);

    if (tcp_h->th_flags == (TH_ACK | TH_RST))
    {
        fprintf(stdout, "%s:%u closed", libnet_addr2name4(ip_h->ip_src.s_addr, LIBNET_DONT_RESOLVE), ntohs(tcp_h->th_sport));
    }
    else if (tcp_h->th_flags == (TH_ACK | TH_SYN))
    {
        fprintf(stdout, "%s:%u opened", libnet_addr2name4(ip_h->ip_src.s_addr, LIBNET_DONT_RESOLVE), ntohs(tcp_h->th_sport));
    }
}

void start_pcap()
{
    Log("Pcap is OK");
    for (;;)
    {
        pcap_dispatch(handler, -1, packet_handler, NULL);
    } 
}


