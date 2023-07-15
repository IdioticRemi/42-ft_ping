#include "main.h"

void addrinfo_to_host_str(addrinfo_t *addr, char *dst, int size) {
	getnameinfo(addr->ai_addr, addr->ai_addrlen, dst, size, NULL, 0, 0);
}

void addrinfo_to_ipv4_str(addrinfo_t *addr, char *dst, int size) {
	getnameinfo(addr->ai_addr, addr->ai_addrlen, dst, size, NULL, 0, NI_NUMERICHOST | NI_NUMERICSERV);
}

void resolve_dns_addr(const char *host, addrinfo_t **addr) {
	addrinfo_t	hints;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_RAW;
	hints.ai_protocol = IPPROTO_ICMP;

	int return_code = getaddrinfo(host, NULL, &hints, addr);

	if (return_code != 0) {
		dprintf(STDERR_FILENO, PING_MSG_ERROR, host, gai_strerror(return_code));
		exit(1);
	}
}