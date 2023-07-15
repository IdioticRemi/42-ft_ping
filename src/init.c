#include "main.h"

void init_globals() {
	address_info = NULL;

	packet_size = 56;
	packet_count = 4;
	sent_packets = 0;
	recv_packets = 0;

	interval = 1;
	timeout = 1;

	seq = 0;
	ttl = 118;

	ip = (ip_t *) buffer;
	icmp = (icmp_t *) (ip + 1);

	time_total.tv_sec = 0;
	time_total.tv_usec = 0;

	memcpy(host_src, "0.0.0.0", INET_ADDRSTRLEN);
	memset(host_dst, 0, INET_ADDRSTRLEN);
	memset(host_dstname, 0, 512);
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

void init_recvmsg() {
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

int init_socket() {
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

	return sockfd;
}