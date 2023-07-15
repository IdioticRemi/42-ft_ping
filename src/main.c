#include "main.h"

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

void sigalrm_handler() {
	dprintf(STDOUT_FILENO, "timeout...\n");
}

void sigint_handler() {
	dprintf(STDOUT_FILENO, PING_MSG_RES_TAIL_HEADER, hostname);
	dprintf(STDOUT_FILENO, PING_MSG_RES_TAIL_RESUME, sent_packets, recv_packets, (1.0 - ((double)sent_packets / (double)recv_packets)) * 100, timeval_to_ms(time_total));
	if (address_info) freeaddrinfo(address_info);
	exit(0);
}

int main(int argc, char **argv) {
	ensure_arguments(argc);
	ensure_root_privileges();

	init_globals();

	hostname = argv[argc - 1];

	signal(SIGINT, sigint_handler);
	signal(SIGALRM, sigalrm_handler);

	resolve_dns_addr(hostname, &address_info);

	addrinfo_to_ipv4_str(address_info, host_dst, INET_ADDRSTRLEN);
	addrinfo_to_host_str(address_info, host_dstname, 512);

	int sockfd = init_socket();

	dprintf(STDOUT_FILENO, PING_MSG_RES_HEADER, hostname, host_dst, packet_size, sizeof(buffer));

	while(!(packet_count != 0 && sent_packets == packet_count)) {
		ssize_t bytes_sent;
		ssize_t bytes_recv;

		alarm(timeout);

		// SEND
		init_sendto();

		gettimeofday(&time_loop_start, NULL);

		bytes_sent = sendto(sockfd, buffer, sizeof(buffer), 0, address_info->ai_addr, address_info->ai_addrlen);
		if (bytes_sent == -1) {
			dprintf(STDOUT_FILENO, "sendto: %s\n", strerror(errno));
			exit(1);
		} else
			sent_packets += 1;

		// RECEIVE

		init_recvmsg();

		for (;;) {
			bytes_recv = recvmsg(sockfd, &msg, MSG_DONTWAIT);

			if (bytes_recv == -1 && errno != EAGAIN && errno != EWOULDBLOCK) {
				dprintf(STDOUT_FILENO, "recvmsg: %s\n", strerror(errno));
				exit(1);
			}

			if (icmp->icmp_hun.ih_idseq.icd_id == getpid())
				break;
		}

		gettimeofday(&time_loop_end, NULL);
		time_total = timeval_add(time_total, timeval_sub(time_loop_end, time_loop_start));

		recv_packets += 1;
		seq += 1;

		dprintf(STDOUT_FILENO, PING_MSG_RES_BODY, bytes_recv - 20, host_dstname, host_dst, seq, ttl, timeval_diff(time_loop_start, time_loop_end));
		alarm(0);

		// Only wait if there is more to send
		if (packet_count == 0 || sent_packets != packet_count)
			ft_sleep_interval();
	}

	sigint_handler();

	return 0;
}
