# Basic Scanner
## Info
A simple IP scanner, which is the program assignment of the Internet Security in NUAA, 2017. It can do such kinds of IP scanning:
- Connect
- SYN
- FIN
- ACK(TODO)
- UDP(TODO)
With `libnet` and `libpcap`, building packets and handling packets is quiet easy. Also, the use of multi-thread makes it faster.

## Usage
sudo ./ipscan -s [Connect|SYN|FIN|ACK|UDP] -i [interface\_name] -p [ports] -t [interval\_time] -T [timeout\_time] ip\_address

Note:
1. `ports` can be `22,80,125`, `1-100`, or `1-100,135`, but spaces cannot be existed.
2. `ip_address` can be the simple one `192.168.1.1`, or a CIDR address `192.168.1.0/24`

## TODO
- Finish other kinds of scanning.
