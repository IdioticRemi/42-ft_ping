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

// gettimeofday
#include <sys/time.h>

// ours
#include "messages.h"

#define PING_PACKET_SIZE 64
#define PING_PORT_NO 0
#define PING_TIMEOUT 1000