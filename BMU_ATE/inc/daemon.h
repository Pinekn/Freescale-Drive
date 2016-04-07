




#ifndef __DAEMON__H__
#define __DAEMON__H__

struct app
{
	int log_level;
	int is_daemon;
	int time_interval;
	int restart_count;
	int (*app_main)();
	int (*app_exit)();
};

int app_daemon(struct app* appc);

#endif
