#include <string.h>
#include <stdlib.h>
#include <stdio.h>


#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <time.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include "../inc/push_server.h"

//-L/opt/buildroot-gcc463/usr/lib/openssl -lcrypto

static struct sockaddr_in  push_addr = {0, 0};		// 推送服务器信息


void close_push_server(int fd);
int recv_push_server(int fd, char** buf, int* len);
int send_push_server(int fd, const char* buf, int len);
int connect_push_server(const char* addr, unsigned short port);



void close_push_server(int fd)
{
	if (fd != -1)
	{
		close(fd);
	}
}

int recv_push_server(int fd, char** buf, int* len)
{
	char* tem = *buf;
	int recv_len = *len;
	static struct sockaddr_in ad;
	socklen_t l = sizeof(ad);
	int offet = 0;
	int sum_len = 0;
	while(1)
	{
		int ret = recvfrom(fd, tem + offet, recv_len - offet, MSG_DONTWAIT, (struct sockaddr*)&ad, &l);
		if (ret < 0)
		{
			if (EINTR == errno || EAGAIN == errno)
			{
				usleep(0);
				continue;
			}
			offet = -1;
			break;
		}
		offet += ret;

		if (offet > 16)
		{
			sum_len = 16 + htonl(*(int*)(tem + 4)) + htons(*(short*)(tem + 12)) + 32;
			if (sum_len > recv_len)
			{
				offet = -1;
				break;
			}
		}
		else
			break;

		if (offet == sum_len)
		{
			*len = offet;
			break;
		}
		else if (offet > sum_len)
		{
			offet = -1;
			break;
		}
		break;
	}

	return offet;
}

int send_push_server(int fd, const char* buf, int len)
{
	int send_len = 0;
	int offet = 0;
	while (offet < len)
	{
		send_len = sendto(fd, buf + offet, len - offet, MSG_NOSIGNAL | MSG_DONTWAIT, (const struct sockaddr*)&push_addr, sizeof(struct sockaddr_in));
		if (send_len < 0)
		{
			if (EINTR == errno || EAGAIN == errno)
			{
				usleep(0);
				continue;
			}
			offet = -1;
			break;
		}
		offet += send_len;
	}

	return offet;
}


int connect_push_server(const char* addr, unsigned short port)
{
	int fd = -1;
#if 0
	do
	{
		if (!addr || !port)
			break;

		fd = socket(AF_INET, SOCK_DGRAM, 0);
		if (-1 == fd)
		{
			break;
		}

		push_addr.sin_family = AF_INET;
		push_addr.sin_port = htons(port);
		struct hostent* host;
		host = gethostbyname(addr);
		if (!host)
		{
			break;
		}

		push_addr.sin_addr.s_addr = *(unsigned int*)host->h_addr_list[0];

		return fd;
	}while(0);

	close_push_server(fd);
#endif
	return -1;
}

