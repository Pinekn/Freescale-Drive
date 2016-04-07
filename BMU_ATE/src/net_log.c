
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <stdarg.h>
#include <string.h>
#include <pthread.h>
#include "../inc/aes.h"
#include "../inc/net_log.h"
#include "../inc/push_server.h"
#include "../inc/Cmd108a.h"



typedef struct log_header
{
	int len;
	short src_len;
	short des_len;
	char body[0];
}LOG_HEADER, *LPLOG_HEADER;

#define AES_BLOCK_SIZE		16
#define AES_KEY_SIZE		32
#define AES_IV_SIZE			16

#define AES_KEY_NAME		"@#$<^&!amilodon=~!`;:|.,?+123456"
#define AES_IV_NAME			"amilodon_@#$>^&!"

//#define DOMAIN_NANME        "smilodon.pomcube.net"
#define DOMAIN_NANME        "192.168.0.15"
#define DEVICE_CODE_FILE    "/etc/id"

#define GET_DEVICE_ID_FORM_FILE			1
#define GET_DEVICE_ID_FORM_CHIP			2
#define GET_DEVICE_ID_FORM_DEBUG		3
#define GET_DEVICE_ID_WAY				GET_DEVICE_ID_FORM_DEBUG

static pthread_mutex_t	log_net_lock;
char device_code[DEVICE_ID_SIZE] = {107, 107, 107, 106, 106, 106, 106, 106, 106, 0};

int g_fd = -1;


inline char getChar(char a)
{
	if (a < 10)
		return '0' + a;
	else
		return 'A' + a - 10;
	return 0;
}

int DecToHex(char* hex, const char* dec, int len)
{
	int i = 0;
	for (i = 0; i < len; ++ i, ++hex)
	{
		char a = (dec[i] >> 4) & 0x0F;
		*hex = getChar(a);
		++ hex;
		a = dec[i] & 0x0F;
		*hex = getChar(a);
	}
	*hex = 0;
	return i;
}

// read the device ID
static int get_device_id()
{
	char id[10];

#if( GET_DEVICE_ID_WAY == GET_DEVICE_ID_FORM_CHIP )
{
	static char get108ACnt ;

	get108ACnt = 0 ;
	do
	{
		int init_108Ast  = init_execute_cmd() ;

		if (init_108Ast < 0)
		{
			get108ACnt++ ;
			usleep(50000) ;
		}
		else
		{
			break; //init successed.
		}
	}while( get108ACnt < 10 );

	if( get108ACnt >= 10)
	{
		Display_log_E("<NET LOG>init_execute_cmd error!");
		return -1;
	}

	get108ACnt = 0 ;
	char buf[48];
	do
	{
		int ret = execute_cmd(0x02, 0x80, 0, 2, 0, 0, 0, buf, 48, 50 * 1000);
		if (ret < 2)
		{
			get108ACnt++ ;
			usleep(50000) ;
		}
		else
		{
			break;//Get device code successed.
		}
	}while( get108ACnt < 10 );

	if( get108ACnt >= 10)
	{
		Display_log_E("<NET LOG> - execute_cmd errorerror!");
		return -1;
	}

	memcpy(id, buf, 4);
	memcpy(&id[4], &buf[8], 5);
	DecToHex(device_code, id, 9);
}
#elif( GET_DEVICE_ID_WAY == GET_DEVICE_ID_FORM_CHIP )
{
    int Dev_fd,Dev_size ;
    char buf[48];

    Dev_size = 18 ;

    Dev_fd = open(DEVICE_CODE_FILE,O_RDONLY );

    if(Dev_fd < 0)
    {
        Display_log_E("<Net log> - Get device code from chip error");
    }
    else
    {
    	int static read_cnt = 0 ;
    	int bytes_read = 0 ;

    	do
		{
    		char cnt = 0 ;
    		bytes_read = read(Dev_fd, device_code , Dev_size) ;
    		if( bytes_read >= 1 )
    		{
    			for(unsigned char i =0 ;i<Dev_size; i++ )
    			{
    				if( device_code[i] == 0xFF )
    				{
    					cnt++ ;
    				}
    			}

    			if(cnt < Dev_size)
    			{
    				break;
    			}
    		}

    		read_cnt++;
    		usleep(1000);
    	}while( read_cnt < 10 );

    	if( read_cnt >= 10 )
    	{
    		Display_log_E("<Net log> - Get device code from file error");
    	}
    }

    close(Dev_fd);
}
#else
{
	memcpy(id, device_code, 9);
	id[9] = 0;
	DecToHex(device_code, id, 9);
}
#endif

return 0;
}

static int get_net_log_time(char* pszTime)
{
	time_t t = time(NULL);

	struct tm* l = localtime(&t);

	int ret = sprintf(pszTime, "%04d-%02d-%02d %02d:%02d:%02d ", l->tm_year + 1900, l->tm_mon + 1, l->tm_mday, l->tm_hour, l->tm_min, l->tm_sec);
	return ret;
}


static int aes_decrypt(const char* src, int src_len, char** des)
{
	char* tmp = NULL;
	unsigned char key[AES_KEY_SIZE];
	unsigned char iv[AES_IV_SIZE];
	memcpy(iv, AES_IV_NAME, AES_IV_SIZE);
	memcpy(key, AES_KEY_NAME, AES_KEY_SIZE);
	do
	{
		tmp = (char*)malloc(src_len + AES_BLOCK_SIZE + sizeof(LOG_HEADER));
		if (!tmp)
			break;

		LPLOG_HEADER header = (LPLOG_HEADER)tmp;

		header->src_len = src_len;
		header->des_len = ((src_len / AES_BLOCK_SIZE) == 0) ? src_len : src_len + (AES_BLOCK_SIZE - (src_len % AES_BLOCK_SIZE));
		header->len = header->des_len + sizeof(LOG_HEADER);

		AES_KEY aes;
		if( AES_set_encrypt_key((unsigned char*)key, AES_KEY_SIZE * 8, &aes) < 0 )
			break;
		*des = tmp;

		AES_cbc_encrypt((const unsigned char*)src, (unsigned char*)(header->body), src_len, &aes, iv, AES_ENCRYPT);
		return header->len;
	}while(0);
	if (tmp)
		free(tmp);
	return -1;
}

int net_log_init()
{
	get_device_id();

	close_execute_cmd();

	if ((g_fd = connect_push_server(DOMAIN_NANME, 8080)) < 0)
	{
		return -1;
	}

	if (pthread_mutex_init(&log_net_lock, 0) != 0)
	{
		return -1;
	}
	return 0;
}

int write_net_log(const char* fromat, ...)
{
	char buf[2024];

	int len = get_net_log_time(buf);

	va_list arg;
	va_start(arg, fromat);
	len = vsnprintf(buf + len, 2024 - len, fromat, arg) + len;
	va_end(arg);

	char *sen;
	len = aes_decrypt(buf, len, &sen);

	if (len < 0)
		return -1;

	pthread_mutex_lock(&log_net_lock);
	send_push_server(g_fd, sen, len);
	pthread_mutex_unlock(&log_net_lock);

	free(sen);
	return len;
}

void net_log_close()
{
	if (-1 != g_fd)
		close_push_server(g_fd);
	pthread_mutex_destroy(&log_net_lock);
}
