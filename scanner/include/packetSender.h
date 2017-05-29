#ifndef __PACKET_SENDER_H__
#define __PACKET_SENDER_H__

#define PORTS_PER_THREAD 70000
#define BEGIN_SLEEP_TIME 3
#define PACKET_SLEEP_TIME 2000

void init_net_ctx(int inj_type);
void init_ip_pool(char* addr);
uint32_t get_ip(uint32_t index);

void sendSYN(uint32_t dst, uint16_t dp);
void sendACK(uint32_t dst, uint16_t dp);
void sendUDP(uint32_t dst, uint16_t dp);
void sendFIN(uint32_t dst, uint16_t dp);

#endif
