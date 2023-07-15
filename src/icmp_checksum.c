#include "main.h"

unsigned short	icmp_checksum(ushort *data, int len)
{
	unsigned long checksum = 0;

	while (len > 1)
	{
		checksum = checksum + *data++;
		len -= sizeof(ushort);
	}

	if (len)
		checksum = checksum + ((unsigned char *) data)[0];

	checksum = (checksum >> 16) + (checksum & 0xffff);
	checksum = checksum + (checksum >> 16);

	return (ushort) ~checksum;
}