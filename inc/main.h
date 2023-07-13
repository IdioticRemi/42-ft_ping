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