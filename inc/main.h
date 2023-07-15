#pragma once

// standard shit
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

// signals
#include <signal.h>

// socket/network
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
# include <netinet/in.h>
# include <netinet/ip.h>
# include <netinet/ip_icmp.h>

// gettimeofday
#include <sys/time.h>

// ours
#include "messages.h"

// typedefs
typedef struct addrinfo addrinfo_t;
typedef struct timeval timeval_t;
typedef struct msghdr msghdr_t;
typedef struct iovec iovec_t;
typedef struct icmp icmp_t;
typedef struct ip ip_t;

// globals
timeval_t time_loop_start;
timeval_t time_loop_end;
timeval_t time_total;

int packet_size;
int packet_count;
int sent_packets;
int recv_packets;

int interval;
int timeout;

int seq;
int ttl;

char	host_src[INET_ADDRSTRLEN];
char	host_dst[INET_ADDRSTRLEN];
char	host_dstname[512];
char	buffer[20 + 8 + 56]; // 20 -> IP header size, 8 -> ICMP header size, 56 -> default packet size
char	control[1024];
char	*hostname;

msghdr_t	msg;
iovec_t		iov[1];

addrinfo_t	*address_info;
icmp_t 		*icmp;
ip_t		*ip;

// functions
timeval_t timeval_add(timeval_t v1, timeval_t v2);
timeval_t timeval_sub(timeval_t v1, timeval_t v2);
double timeval_to_ms(timeval_t val);
double timeval_diff(timeval_t start, timeval_t end);
unsigned short	icmp_checksum(ushort *data, int len);
void ft_sleep_interval();

void addrinfo_to_host_str(addrinfo_t *addr, char *dst, int size);
void addrinfo_to_ipv4_str(addrinfo_t *addr, char *dst, int size);
void resolve_dns_addr(const char *host, addrinfo_t **addr);

void init_globals();
void init_sendto();
void init_recvmsg();
int init_socket();