#pragma once

#define PING_MSG_HELP "ping: usage error: Destination address required\n"
#define PING_MSG_ERROR "ping: %s: %s\n"

#define PING_MSG_RES_HEADER "PING %s (%s) %d(%ld) bytes of data.\n"
#define PING_MSG_RES_BODY "%li bytes from %s (%s): icmp_seq=%d ttl=%d time=%.2f ms\n"
#define PING_MSG_RES_TAIL_HEADER "\n--- %s ping statistics ---\n"
#define PING_MSG_RES_TAIL_RESUME "%d packets transmitted, %d received, %.0f%% packet loss, time %.0fms\n"
#define PING_MSG_RES_TAIL_TIME "\nrtt min/avg/max/mdev = %f/%f/%f/%f ms\n"