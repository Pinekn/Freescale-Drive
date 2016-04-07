/*
*
*
*
*
*
*/

#ifndef __CMD_108A__H__
#define __CMD_108A__H__

#ifdef __cplusplus
extern "C"{
#endif

int init_execute_cmd();

int execute_cmd(char cmd, char p1, short addr, int zone, short slot, const void* data, int len, char* buf, int outlen, int mstimeout);

void close_execute_cmd();

#ifdef __cplusplus
};
#endif

#endif
