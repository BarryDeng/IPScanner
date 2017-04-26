#ifndef __PACKET_SENDER_H__
#define __PACKET_SENDER_H__

void init_net_ctx(int inj_type);
void init_ip_pool(char* addr);
uint32_t get_ip(uint32_t index);


#endif
