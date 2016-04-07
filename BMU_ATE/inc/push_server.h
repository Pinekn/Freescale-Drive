/*
*
*
*
*
*
*/

#ifndef __PUSH__SERVER__H__
#define __PUSH__SERVER__H__

#ifdef __cplusplus
extern "C"{
#endif

extern void close_push_server(int fd);
extern int recv_push_server(int fd, char** buf, int* len);
extern int send_push_server(int fd, const char* buf, int len);
extern int connect_push_server(const char* addr, unsigned short port);

#ifdef __cplusplus
};
#endif

#endif
