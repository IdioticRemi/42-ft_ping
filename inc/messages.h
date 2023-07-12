#pragma once

#define PING_MSG_HELP "ping: usage error: Destination address required\n"
#define PING_MSG_ERROR "ping: %s: %s\n"
#define PING_MSG_INVALID_IP "ping: %s: Name or service not known\n"

#define PING_MSG_RES_HEADER "PING %s (%s) %d(%d) bytes of data.\n"
#define PING_MSG_RES_BODY "%d bytes from %s (%s): icmp_seq=%d ttl=%d time=%f ms\n"
#define PING_MSG_RES_TAIL_HEADER "--- %s ping statistics ---\n"
#define PING_MSG_RES_TAIL_RESUME "%d packets transmitted, %d received, %f%% packet loss, time %dms"
#define PING_MSG_RES_TAIL_TIME "rtt min/avg/max/mdev = %f/%f/%f/%f ms\n"