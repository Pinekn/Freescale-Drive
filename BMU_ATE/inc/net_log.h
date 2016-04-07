
#ifndef __NET_LOG__H__
#define __NET_LOG__H__


#ifdef __cplusplus
extern "C" {
#endif

#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>



int net_log_init();
int write_net_log(const char* fromat, ...);
void net_log_close();

#define DEVICE_ID_SIZE				20

extern char device_code[DEVICE_ID_SIZE];

#define NET_FILEINFO(s, x)			s"[pro=bmu_500,ver=1.0.0:fi=%s:fun=%s:li=%d:pid=%d:tid=%d] [%s] "x, __FILE__, __FUNCTION__, __LINE__, getpid(), pthread_self(), device_code

#define NET_LOG_E(fromat, ...)		write_net_log(NET_FILEINFO("[error] ", fromat), ##__VA_ARGS__)
#define NET_LOG_W(fromat, ...)		write_net_log(NET_FILEINFO("[warn] ", fromat), ##__VA_ARGS__)
#define NET_LOG_I(fromat, ...)		write_net_log(NET_FILEINFO("[info] ", fromat), ##__VA_ARGS__)

#define PRT_LOG_E(fromat, ...)		printf(fromat"\n", ##__VA_ARGS__)
#define PRT_LOG_W(fromat, ...)		printf(fromat"\n", ##__VA_ARGS__)
#define PRT_LOG_I(fromat, ...)		printf(fromat"\n", ##__VA_ARGS__)



#define LOG_E 						PRT_LOG_E
#define LOG_W 						PRT_LOG_W
#define LOG_I 						PRT_LOG_I


#ifdef __cplusplus
};
#endif

#endif
