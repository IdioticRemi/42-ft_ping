#include "main.h"

timeval_t timeval_add(timeval_t v1, timeval_t v2) {
	timeval_t result;

	result.tv_sec = v1.tv_sec + v2.tv_sec;
	result.tv_usec = v1.tv_usec + v2.tv_usec;

	return result;
}

timeval_t timeval_sub(timeval_t v1, timeval_t v2) {
	timeval_t result;

	result.tv_sec = v1.tv_sec - v2.tv_sec;
	result.tv_usec = v1.tv_usec - v2.tv_usec;

	return result;
}

double timeval_to_ms(timeval_t val) {
	double duration = (val.tv_sec * 1e6 + val.tv_usec) / 1000.0;

	return duration >= 0.0 ? duration : 0.0;
}

double timeval_diff(timeval_t start, timeval_t end) {
	double duration = timeval_to_ms(timeval_sub(end, start));

	return duration >= 0.0 ? duration : 0.0;
}