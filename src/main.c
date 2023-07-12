#include "main.h"

typedef struct addrinfo addrinfo_t;
typedef struct msghdr msghdr_t;
typedef struct iovec iovec_t;

void ensure_arguments(int argc) {
	if (argc != 2) {
		dprintf(STDERR_FILENO, PING_MSG_HELP);
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
	hints.ai_protocol = IPPROTO_ICMP;
	hints.ai_socktype = SOCK_RAW;

	int return_code = getaddrinfo(hostname, NULL, &hints, addr);

	if (return_code != 0) {
		dprintf(STDERR_FILENO, PING_MSG_ERROR, hostname, gai_strerror(return_code));
		exit(1);
	}
}

int main(int argc, char **argv) {
    ensure_arguments(argc);

	char	*hostname = argv[argc - 1];

	addrinfo_t	*addr;

	char	ipv4_str[INET_ADDRSTRLEN];
	memset(ipv4_str, 0, INET_ADDRSTRLEN);
	
	char	dns_str[512];
	memset(dns_str, 0, 512);

	int ttl_val = 64;

	resolve_dns_addr(hostname, &addr);

	addrinfo_to_ipv4_str(addr, ipv4_str, INET_ADDRSTRLEN);
	addrinfo_to_dns_str(addr, dns_str, 512);

	dprintf(STDOUT_FILENO, PING_MSG_RES_BODY, PING_PACKET_SIZE, dns_str, ipv4_str, 0, ttl_val, 0);

	int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (sockfd == -1) {
		dprintf(STDERR_FILENO, PING_MSG_ERROR, hostname, strerror(errno));
		exit(1);
	}

	msghdr_t	msg;
	iovec_t		iov[1];
	u_int8_t	*buf = calloc(sizeof(u_int8_t), 4096);

	iov[0].iov_base = buf;
    iov[0].iov_len = 4096 - 1;

	msg.msg_control = NULL;
	msg.msg_controllen = 0;
	msg.msg_flags = 0;
	msg.msg_iov = iov;
    msg.msg_iovlen = 1;
	msg.msg_name = addr->ai_addr;
	msg.msg_namelen = addr->ai_addrlen;

    if (setsockopt(sockfd, SOL_IP, IP_TTL, &ttl_val, sizeof(ttl_val)) != 0)
    {
        printf("Setting socket options to TTL failed!\n");
        return;
    }

	struct timeval tv_out;
    tv_out.tv_sec = PING_TIMEOUT;
    tv_out.tv_usec = 0;

	setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*) &tv_out, sizeof tv_out);

	// WTF JPP ALED PINGPONGTCHINGTCHONG
    ssize_t r = sendto(sockfd, "lala", 4, 0, addr->ai_addr, addr->ai_addrlen);
	dprintf(STDOUT_FILENO, "sent: %u bytes\n", r);

	recvmsg(sockfd, &msg, 0);

	dprintf(STDOUT_FILENO, "content: %s\n", buf);

	freeaddrinfo(addr);

    return 0;
}
