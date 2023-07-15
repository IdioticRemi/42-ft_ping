#include "main.h"

void ft_sleep_interval() {
	timeval_t	now;

	gettimeofday(&now, NULL);

	timeval_t	next = now;

	next.tv_sec += interval;

	while (now.tv_sec < next.tv_sec || now.tv_usec < next.tv_usec)
		gettimeofday(&now, NULL);

	time_total.tv_sec += interval;
}