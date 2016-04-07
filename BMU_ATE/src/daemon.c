#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/un.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/time.h>

#include <unistd.h>


#include "../inc/daemon.h"
#include "../inc/Task.h"


static struct app app_d;


static int daemon_init(void);
static void killfile(pid_t p, pid_t c) ;
static int app_main(void) ;
static int map_print(void) ;
static void app_exit(int sig) ;
static int sock;
static int listen_fd ;

int app_client_fd;


static int app_CreateServer(const char* addr_name);
static int App_Read(int fd, char* pszBuff, int nLen);

static int app_MsgAccept(void);
static void app_CloseSvr(int fd);
static int get_module_file_name(char* pszName, int nLen);
static char* get_module_file_path_name(char* pszName, int nLen);


int App_MainFunc(void);
int App_SvrExit(void);

int App_Write(int fd, const char* lpszBuff, int nLen);



int main(int argc, const char *argv[])
{
  struct app p ;

	p.log_level    		= 5;// 0:close log file
	p.is_daemon 		= 1 ;  // 0:foreground;
	p.time_interval   	= 2 ;
	p.restart_count   	= 10;
	p.app_main	 	= App_MainFunc ;
	p.app_exit 		= App_SvrExit ;

	//if (argc != 1)
	  p.is_daemon = 0;

	app_daemon(&p);
	return 0;
}

int app_daemon(struct app* appc)
{
	app_d = *appc;

	//net_log_init();

	if (!appc->is_daemon) // foreground
	{
		signal(SIGTERM, app_exit);
		if (app_main() == -1)
		{
			app_exit(-1);
		}
	}
	else// back ground
	{
		if (daemon_init() == -1)
			return -1;

		signal(SIGTERM, app_exit);

		pid_t pid_p = getpid();

		while (1)
		{
			pid_t pid = fork();

			if (pid < 0)
				break;

			if (pid > 0)
			{
				usleep(0);
				killfile(pid_p, pid);

				while(1)
				{
					int nStatu;
					int nR = wait(&nStatu);
					if (-1 == nR || errno == EINTR)
					{
						continue;
					}
					break;
				}

				-- app_d.restart_count;
				if (0 == app_d.restart_count)
				{
					app_exit(-1);
					break;
				}
				else
				{
					app_exit(app_d.restart_count);
				}
				sleep(app_d.time_interval);
			}
			else
			{
				if (app_main() == -1)
				{
					app_exit(0);
					break;
				}
			}
		}
	}
	return 0;
}


static int daemon_init(void)
{
	pid_t pid;

	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	signal(SIGPIPE, SIG_IGN);
	signal(SIGIO, SIG_IGN);

	do
	{
		pid = fork();
		if (pid < 0)
			break;

		if (pid > 0)
			exit(0);

		if (setsid() == -1)
		{
			printf("setsid() failed !\n");
			break;
		}
		umask(0);

		if (map_print() == -1)
			break;

		return 0;
	}while(0);
	return -1;
}




static void killfile(pid_t p, pid_t c)
{
	char exe_name[64];
	if (get_module_file_name(exe_name, 64) == -1)
		return;


	char kill_name[128];
	snprintf(kill_name, 128, "kill%s", exe_name);

	FILE* pf = fopen(kill_name, "wb");
	if (!pf)
		return;

	fprintf(pf, "%d\n%d", p, c);

	fclose(pf);
	chmod(kill_name, S_IRUSR | S_IWUSR | S_IXUSR | S_IXGRP | S_IWGRP | S_IRGRP | S_IROTH | S_IWGRP);
}




static int app_main(void)
{
	if (app_d.app_main && app_d.app_main() != 0)
		return -1;

	while(1)
		sleep(-1);
	return 0;
}


static int map_print(void)
{
	int fd;
	do
	{
		fd = open("/dev/null", O_RDWR);
		if (fd == -1)
		{
			printf("open /dev/null failed !");
			break;
		}

		if (dup2(fd, STDIN_FILENO) == -1)
		{
			printf("dup2 STDIN_FILENO failed !");
			break;
		}

		if (dup2(fd, STDOUT_FILENO) == -1)
		{
			printf("dup2 STDOUT_FILENO failed !");
			break;
		}

		if (dup2(fd, STDERR_FILENO) == -1)
		{
			printf("dup2 STDERR_FILENO failed !");
			break;
		}
		if (close(fd) == -1)
		{
			printf("close /dev/null failed !");
			fd = -1;
			break;
		}
		return 0;
	}while(0);


	if (fd != -1)
	{
		close(fd);
	}
	return -1;
}


static void app_exit(int sig)
{
	if (sig != -1)
	{
		//net_log_close();
		if (app_d.app_exit && app_d.app_exit());
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//Aplication function
////////////////////////////////////////////////////////////////////////////////////////////////////

int App_MainFunc(void)
{
  int se_fd;
  fd_set  readSet ;

  app_client_fd = -1 ;
  FD_ZERO(&readSet);  // Clean the file descriptor,1 bit per possible descriptor


  listen_fd = app_CreateServer("/dev/bms");
  if (listen_fd < 0)
   {
     return -1;
     printf("Do create socket failed.\n");
   }
  se_fd = listen_fd + 1;

  Task_Init();

  while(1)
  {
      int fdReadyNum;
      struct timeval timeout ;

      timeout.tv_sec = 0;
      timeout.tv_usec = 5000;

    //  printf("listen_fd = %d  \n",listen_fd );

      FD_SET(listen_fd, &readSet);

      if (-1 != app_client_fd)
       {
         FD_SET(app_client_fd, &readSet);
       }

       fdReadyNum = select(se_fd, &readSet, NULL, NULL, &timeout);

	    if (fdReadyNum > 0) /* Select Func get a ready file descriptor */
	    {
	        if (FD_ISSET(listen_fd, &readSet))
	        {
             app_client_fd = app_MsgAccept();

	           FD_SET(app_client_fd, &readSet);

	           if (app_client_fd > listen_fd)
	           {
	            se_fd = app_client_fd + 1;
	           }
               else
	           {
	            se_fd = listen_fd + 1;
	           }
	        }

          if ( (-1 != app_client_fd) && FD_ISSET(app_client_fd, &readSet))
	        {
	          char buf[1024];
	          int l = App_Read(app_client_fd, buf, 1024);

		         if (l == -1)// connect broken ;
		          {
                app_CloseSvr(app_client_fd);
                FD_CLR(app_client_fd, &readSet);
                app_client_fd = -1;
		          }
		         else
		          {
      			    printf("ComIf:The rx data length = %d. \n", l );
      			    //ComIf_RxMsgDispose(buf);
	         	  }
	        }
	    }
		else if (0 == fdReadyNum) //timeout,No ready file descriptor get
	    {
		    if (-1 != app_client_fd)
        	{
		    }
	    }
	    else
	    {
	     // Error : -1
	    }


	    Task_Schedule(); // main task
	    //return 1;

  } //end of while
}


int App_SvrExit(void)
{
  app_CloseSvr(-1);
  return 1;
}





static int app_CreateServer(const char* addr_name)
{
  int nReuse = 1, fd;
  struct sockaddr_un  un;
  socklen_t   len;

  do
  {
	/* Create Socket */
    sock = socket(AF_UNIX, SOCK_STREAM, 0); // family:UNIX 文件系统; type: SOCK_STREAM
    if (sock == -1)
    {
      printf("create socket failed . \"%s\"", strerror(errno));
      break;
    }

    fd = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &nReuse, sizeof(nReuse)) ;
    if( fd < 0)
    {
      printf("set addr failed! . \"%s\"", strerror(errno));
      break;
    }

    unlink(addr_name);               	/* in case it already exists */
    memset(&un, 0, sizeof(un));
    un.sun_family = AF_UNIX;
    strcpy(un.sun_path, addr_name);
    len = offsetof(struct sockaddr_un, sun_path) + strlen(addr_name);

	/* bind the name to the descriptor */
	  fd = bind(sock, (struct sockaddr *)&un, len) ;
    if( fd == -1)
    {
      printf("Failed to bind :\" %s \".(\" %s \")", addr_name, strerror(errno));
      break;
    }

    fd = listen(sock, 3) ;
    if (fd == -1)
    {
      printf("listen addr = %s (%s)", addr_name, strerror(errno));
      break;
    }

    return sock;

  }while (0);

    if (sock != -1)
     close(sock);

    return -1;
}





int App_Write(int fd, const char* lpszBuff, int nLen)
{
	int nResult = nLen;
	int nSize;
	while (nLen > 0)
	{
		nSize = send(fd, lpszBuff, nLen, MSG_NOSIGNAL | MSG_DONTWAIT);
		if (0 == nSize)
		{
		  break;
		}

    if (-1 == nSize)
		{
			if (errno == EAGAIN || errno == EINTR)
			{
			  // Buffer overflow
			  usleep(0);
			  continue;
			}
			nResult = -1;
			break;
		}

		if (nSize <= nLen)
		{
		 nLen -= nSize;
		 lpszBuff += nSize;
		}
		else
		{
			nResult = -1;
			break;
		}
	}

	return nResult;
}




int App_Read(int fd, char* pszBuff, int nLen)
{
	int ret;
	int offset = 0;
	int data_len = 0;

	while (offset < nLen)
	{
		int nResult ;

		nResult = recv(fd, (pszBuff+offset), (nLen - offset),MSG_DONTWAIT);
    if ( -1 == nResult )
	  {
  	  if (errno == EAGAIN || errno == EINTR)
	     {
	       usleep(0);
         ret = 0;
	     }
	     else
	     {
	       ret = -1;
	     }
	  }
	  else if (0 == nResult)
	  {
	    ret = -1 ;
  	}
  	else
  	{
  	  ret = nResult ;
  	}

		if (ret == -1)
		{
		  data_len = -1;
		  break;
		}
		offset += ret;

		if (offset > 2 && data_len == 0)
		{
			data_len = *(short*)(pszBuff + 2);
			if (data_len > nLen)
			{
			  data_len = -1;
			  break;
			}
		}

		if (offset >= data_len)
		break;
	}

	return data_len;
}



int app_MsgAccept(void)
{
  struct sockaddr_un addr = {0};

	socklen_t len = sizeof(addr);

	return accept(sock, (struct sockaddr*)&addr, &len);
}

void app_CloseSvr(int fd)
{
	if (fd != -1)
		close (fd);
	else
	{
		close(sock);
		sock = -1;
	}
}


int get_module_file_name(char* pszName, int nLen)
{
	if (get_module_file_path_name(pszName, nLen))
	{
		int l = strlen(pszName);
		const char* p = pszName + l;
		while(*p != '/')
			-- p;
		++ p;
		nLen = 0;
		while(*p != 0)
		{
			*pszName = *p;
			++ p;
			++ nLen;
			++ pszName;
		}
		*pszName = 0;
		return nLen;
	}
	return -1;
}

char* get_module_file_path_name(char* pszName, int nLen)
{
	const char* lpszLink = "/proc/self/exe";
	memset(pszName, 0, nLen);
	if (readlink(lpszLink, pszName, (size_t)nLen) != -1)
	{
		return pszName;
	}
	return NULL;
}

int create_dir(const char* pszPathName, int nMode)
{
	struct stat csFileInfo;
	int nRet = stat(pszPathName, &csFileInfo);
	if (nRet == -1)
	{
		return mkdir(pszPathName, nMode);
	}
	return 0;
}

int is_dir(const char* pszPathName)
{
	struct stat csFileInfo;
	if (stat(pszPathName, &csFileInfo) == -1)
	{
		return -1;
	}
	return 0;
}

int is_file(const char* pszPathName)
{
	if (access(pszPathName, F_OK) < 0)
		return -1;
	return 0;
}
