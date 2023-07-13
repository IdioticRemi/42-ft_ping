#include "main.h"

typedef struct addrinfo addrinfo_t;
typedef struct timeval timeval_t;
typedef struct msghdr msghdr_t;
typedef struct iovec iovec_t;
typedef struct icmp icmp_t;
typedef struct ip ip_t;

timeval_t time_start;
timeval_t time_end;

int packet_size = 56;
int packet_count = 0;
int sent_packets = 0;
int recv_packets = 0;

int interval = 1;
int timeout = 1;

int seq = 0;
int ttl = 96;

char	host_src[INET_ADDRSTRLEN] = "0.0.0.0";
char	host_dst[INET_ADDRSTRLEN];
char	host_dstname[512];
char	buffer[20 + 8 + 56]; // 20 -> IP header size, 8 -> ICMP header size, 56 -> default packet size
char	control[1024];
char	*hostname;

msghdr_t	msg;
iovec_t		iov[1];

addrinfo_t	*address_info;
ip_t		*ip;
icmp_t 		*icmp;

double duration_from(timeval_t start, timeval_t end) {
	double duration = (((double) end.tv_sec * 1000000.0 + end.tv_usec) - ((double) start.tv_sec * 1000000.0 + start.tv_usec)) / 1000;

	return duration >= 0.0 ? duration : 0.0;
}

unsigned short	icmp_checksum(ushort *data, int len)
{
	unsigned long checksum = 0;

	while (len > 1)
	{
		checksum = checksum + *data++;
		len -= sizeof(ushort);
	}

	if (len)
		checksum = checksum + ((unsigned char *) data)[0];

	checksum = (checksum >> 16) + (checksum & 0xffff);
	checksum = checksum + (checksum >> 16);

	return (ushort) ~checksum;
}

void wait_interval() {
	timeval_t	now;

	gettimeofday(&now, NULL);

	timeval_t	next = now;

	next.tv_sec += interval;

	while (now.tv_sec < next.tv_sec || now.tv_usec < next.tv_usec)
		gettimeofday(&now, NULL);
}

void init_sendto() {
	memset(buffer, 0, sizeof(buffer));

	ip->ip_v = 4;
	ip->ip_hl = sizeof(*(ip)) >> 2;
	ip->ip_tos = 0;
	ip->ip_len = htons(sizeof(buffer));
	ip->ip_id = 0;
	ip->ip_off |= 0;
	ip->ip_ttl = ttl;
	ip->ip_p = address_info->ai_protocol;
	ip->ip_sum = 0;
	inet_pton(address_info->ai_family, host_src, &(ip->ip_src.s_addr));
	inet_pton(address_info->ai_family, host_dst, &(ip->ip_dst.s_addr));

	icmp->icmp_type = ICMP_ECHO;
	icmp->icmp_code = 0;
	icmp->icmp_hun.ih_idseq.icd_id = getpid();
	icmp->icmp_hun.ih_idseq.icd_seq = seq;
	icmp->icmp_cksum = icmp_checksum((unsigned short*)(icmp), sizeof(icmp));
}

void init_recvfrom() {
	memset(control, 0, sizeof(control));
	memset(buffer, 0, sizeof(buffer));

	iov[0].iov_base = buffer;
	iov[0].iov_len = sizeof(buffer);

	msg.msg_name = address_info->ai_addr;
	msg.msg_namelen = address_info->ai_addrlen;
	msg.msg_iov = iov;
	msg.msg_iovlen = 1;
	msg.msg_flags = 0;
	msg.msg_control = control;
	msg.msg_controllen = sizeof(control);
}

void ensure_arguments(int argc) {
	if (argc != 2) {
		dprintf(STDERR_FILENO, PING_MSG_HELP);
		exit(1);
	}
}

void ensure_root_privileges() {
	if (getuid() != 0) {
		dprintf(STDERR_FILENO, PING_MSG_ERROR, "error", "root privileges are required in order to proceed.");
		exit(1);
	}
}

void addrinfo_to_dns_str(addrinfo_t *addr, char *dst, int size) {
	getnameinfo(addr->ai_addr, addr->ai_addrlen, dst, size, NULL, 0, 0);
}

void addrinfo_to_ipv4_str(addrinfo_t *addr, char *dst, int size) {
	getnameinfo(addr->ai_addr, addr->ai_addrlen, dst, size, NULL, 0, NI_NUMERICHOST | NI_NUMERICSERV);
}

void resolve_dns_addr(const char *hostname, addrinfo_t **addr) {
	addrinfo_t	hints;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_RAW;
	hints.ai_protocol = IPPROTO_ICMP;

	int return_code = getaddrinfo(hostname, NULL, &hints, addr);

	if (return_code != 0) {
		dprintf(STDERR_FILENO, PING_MSG_ERROR, hostname, gai_strerror(return_code));
		exit(1);
	}
}

void sigalrm_handler() {
	dprintf(STDOUT_FILENO, "timeout...\n");
}

void sigint_handler() {
	dprintf(STDOUT_FILENO, PING_MSG_RES_TAIL_HEADER, hostname);
	dprintf(STDOUT_FILENO, PING_MSG_RES_TAIL_RESUME, sent_packets, recv_packets, (1.0 - ((double)sent_packets / (double)recv_packets)) * 100);
	exit(0);
}

int main(int argc, char **argv) {
	ensure_arguments(argc);
	ensure_root_privileges();

	signal(SIGINT, sigint_handler);
	signal(SIGALRM, sigalrm_handler);

	hostname = argv[argc - 1];

	memset(host_dst, 0, INET_ADDRSTRLEN);
	memset(host_dstname, 0, 512);

	resolve_dns_addr(hostname, &address_info);

	addrinfo_to_ipv4_str(address_info, host_dst, INET_ADDRSTRLEN);
	addrinfo_to_dns_str(address_info, host_dstname, 512);

	int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (sockfd == -1) {
		dprintf(STDERR_FILENO, PING_MSG_ERROR, hostname, strerror(errno));
		exit(1);
	}

	int hincl = 1;
	if (setsockopt(sockfd, IPPROTO_IP, IP_HDRINCL, &hincl, sizeof(hincl)) != 0)
	{
		dprintf(STDERR_FILENO, PING_MSG_ERROR, hostname, "failed to configure socket");
		exit(1);
	}

	ip = (ip_t *) buffer;
	icmp = (icmp_t *) (ip + 1);

	dprintf(STDOUT_FILENO, PING_MSG_RES_HEADER, hostname, host_dst, packet_size, sizeof(buffer));

	while (!(packet_count && sent_packets == packet_count)) {
		ssize_t bytes_recv;
		ssize_t bytes_sent;

		// SEND
		alarm(timeout);
		init_sendto();

		gettimeofday(&time_start, NULL);

		bytes_sent = sendto(sockfd, buffer, sizeof(buffer), 0, address_info->ai_addr, address_info->ai_addrlen);
		if (bytes_sent == -1) {
			dprintf(STDOUT_FILENO, "sendto: %s\n", strerror(errno));
			exit(1);
		} else {
			sent_packets += 1;
//			dprintf(STDOUT_FILENO, "sendto: %lu bytes\n", bytes_sent);
		}


		// RECEIVE
		double duration = 0;

		init_recvfrom();

		for (;;) {
			bytes_recv = recvmsg(sockfd, &msg, MSG_DONTWAIT);

			if (bytes_recv == -1 && errno != EAGAIN && errno != EWOULDBLOCK) {
				dprintf(STDOUT_FILENO, "recvmsg: %s\n", strerror(errno));
				exit(1);
			}

//			dprintf(STDOUT_FILENO, "recv: %li\n", bytes_recv);

			if (icmp->icmp_hun.ih_idseq.icd_id == getpid())
				break;
		}

		gettimeofday(&time_end, NULL);
		duration = duration_from(time_start, time_end);
		recv_packets += 1;
		seq += 1;

		dprintf(STDOUT_FILENO, PING_MSG_RES_BODY, bytes_recv - 20, host_dstname, host_dst, seq, ttl, duration);
		alarm(0);
		wait_interval();
	}

	freeaddrinfo(address_info);

	return 0;
}
