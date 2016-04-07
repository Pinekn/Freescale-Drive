
#include <stdio.h> //标准输入输出定义
#include <stdlib.h>
#include <string.h> /* memset */

#include <fcntl.h> //文件控制定义
#include <errno.h> //错误代码定义
#include <unistd.h> //unix标准函数定义
#include <termios.h>//定义串口结构termios，并包含<termbits.h>
#include <string.h> //字符串函数定义
#include <linux/serial.h>
#include <sys/ioctl.h>
#include "../inc/net_log.h"


#define BMS_UART_DEBUG     0

#define BSP_OK     0
#define BSP_ERROR -1

#define SERIAL_PORT1       "/dev/ttyS0" //使用串口1；1，2分别对应/dev/ttyS0和/dev/ttyS1
#define SERIAL_PORT2       "/dev/ttyS2"
#define UART1_BEC_BAUD     B9600 //波特率为9600bps"unsigned char"
#define UART2_BMU_BAUD     250000
#define BAUDRATE           B38400
#define MAX_SIZE           255

static int int_Sci1FD = -1;
static int int_Sci2FD = -1;

int Uart1_Init(void);
int Uart2_Init(void);
int Uart1_Read(char *buf, unsigned int length);
int Uart2_Read(char *buf, unsigned int length);
int Uart1_Write(char *buf, unsigned int length);
int Uart2_Write(char *buf, unsigned int length);
void Uart1_ClearReadBuf(void);
void Uart2_ClearReadBuf(void);

void Uart1_Close(void) ;
void Uart2_Close(void) ;

int Uart1_Init(void)
{
  int int_Flag ;
  struct termios OldTerm,NewTerm;

  speed_t baud ;

  baud = UART1_BEC_BAUD;

  Uart1_Close();

  int_Sci1FD = open(SERIAL_PORT1,O_RDWR|O_NOCTTY|O_NONBLOCK);

  if (int_Sci1FD < 0) //出错显示错误
  {
    LOG_E("<uart> - (BEC)Open serial port %s error.errno=%d.",int_Sci1FD,errno);
      perror("<uart> - Error reason");

    return BSP_ERROR;
  }
  else
  {
    LOG_E("<uart> - (BEC)Open serial port:%d.",int_Sci1FD);
  }

  tcgetattr(int_Sci1FD,&OldTerm); //得到串口的原始属性
  bzero(&NewTerm,sizeof(NewTerm)); //清空新的串口属性结构
  //fcntl(int_Sci1FD,F_SETFL,0); //阻塞read（）操作，读不到数据就阻塞
  NewTerm.c_cflag |= CLOCAL|CREAD; //使能接收，并设置本地状态
  NewTerm.c_cflag &= ~PARENB; //无校验位
  NewTerm.c_cflag &= ~CSTOPB; //一位停止位
  NewTerm.c_cflag &= ~CSIZE; //屏蔽字符大小
  NewTerm.c_cflag |= CS8;  //8位数据
  NewTerm.c_lflag &= ~(ICANON|ECHO|ECHOE); //选择原始输入法
  NewTerm.c_oflag &= ~OPOST; //选择原始数据输出
  NewTerm.c_cc[VMIN] = 8; //指定所要读取字符的最小数量
  NewTerm.c_cc[VTIME] = 1; //指定读取第一个字符的等待时间，时间的单位为n*100ms
  //如果设置VTIME=0，则无字符输入时read（）操作无限期的阻塞
  cfsetispeed(&NewTerm,baud); //设置串口输入端波特率
  cfsetospeed(&NewTerm,baud); //设置串口输出端波特率

  int_Flag = tcsetattr(int_Sci1FD,TCSANOW,&NewTerm); //立即将新属性赋予串口
  if (int_Flag<0)
  {
    LOG_E("<uart> -(BEC)config uart1 for error.errno=%d.",errno);
    return  BSP_ERROR;
  }
  else
  {
    LOG_I("<uart> - uart1 for BEC init sucessed!");
    return BSP_OK;
  }

  return 1 ;
}




int Uart2_Init(void)
{
  int int_Flag ;
  struct termios OldTerm,NewTerm;
  struct serial_struct Serial;
  int baud=UART2_BMU_BAUD;

  int_Sci2FD = open(SERIAL_PORT2,O_RDWR|O_NOCTTY|O_NONBLOCK);

  if (int_Sci2FD < 0)
  {
    LOG_E("<uart> -(BMU)Open serial port %s error.errno=%d.",SERIAL_PORT2,errno);
    perror("<uart> - Error reason");
    return BSP_ERROR;
  }
  else
  {
    LOG_E("<uart> - (BMU)Open serial port:%d.",int_Sci2FD);
  }


  tcgetattr(int_Sci2FD,&OldTerm);
  bzero(&NewTerm,sizeof(NewTerm));
 //fcntl(int_Sci2FD,F_SETFL,0);
  NewTerm.c_cflag |= CLOCAL|CREAD;
  NewTerm.c_cflag &= ~PARENB;
  NewTerm.c_cflag &= ~CSTOPB;
  NewTerm.c_cflag &= ~CSIZE;
  NewTerm.c_cflag |= CS8;
  NewTerm.c_lflag &= ~(ICANON|ECHO|ECHOE);
  NewTerm.c_oflag &= ~OPOST;
  NewTerm.c_cc[VMIN] = 8;
  NewTerm.c_cc[VTIME] = 1;

  cfsetispeed(&NewTerm,BAUDRATE);
  cfsetospeed(&NewTerm,BAUDRATE);

  int_Flag = tcsetattr(int_Sci2FD,TCSANOW,&NewTerm);
  if (int_Flag<0)
  {
    LOG_E("<uart> - config uart2 for BMU  error.errno=%d.",errno);
    return  BSP_ERROR;
  }

  if((ioctl(int_Sci2FD,TIOCGSERIAL,&Serial))<0)
  {
    LOG_E("<uart> - error to get the serial_struct info:%s/n",strerror(errno));
    return BSP_ERROR;
  }

  Serial.flags = ASYNC_SPD_CUST;
  Serial.custom_divisor=Serial.baud_base/baud;

  if((ioctl(int_Sci2FD,TIOCSSERIAL,&Serial))<0)
  {
    LOG_I("<uart> - Fail to set Serial.");
    return BSP_ERROR;
  }

  ioctl(int_Sci2FD,TIOCGSERIAL,&Serial);

  LOG_I("<uart> - Uart2 for BMU init successed.");
  return BSP_OK;
}


int Read_UartItech(unsigned char *buf, int BLKSZ)
{
    int cnt=0, ret;
    int index=0;
    struct timeval timeout;

    if(NULL == buf)
        return -1;

    fd_set inputs;
    FD_ZERO(&inputs);//用select函数之前先把集合清零

    int len = 0;
    while(1) 
    {
        timeout.tv_sec = 0;
        timeout.tv_usec = 15000;
        FD_SET(int_Sci1FD, &inputs);
        ret = select(int_Sci1FD+1, &inputs, (fd_set *)0, (fd_set *)0, &timeout);
        if (0 == ret || ret < 0)
        {
        	cnt = -1;
        	break;
        }

		if(FD_ISSET(int_Sci1FD, &inputs)) 
		{
		    index = read(int_Sci1FD, buf+cnt, BLKSZ - cnt );
		    if (index < 0)
		    {
		    	return -1;
		    }

		    if (len == 0 && index > 0)
		    {
		    	len = BLKSZ ;
		    }
		    cnt += index;
		    if (cnt >= len)
		    	break;
		}
    }
    return cnt;
}

// uart 1 for ITECH e-power/e-load 
int Uart1_Read(char *buf, unsigned int length)
{
#if 0
	int int_Flag;

	int_Flag = read(int_Sci1FD,buf,length); //返回实际读取的字符数目

	if(int_Flag == 0 )
	{
		return 0;
	}
	else if(int_Flag < 0)
	{
		return int_Flag;
	}
	else
	{
		return int_Flag;
	}
	
	return 1 ;
#endif
	return(Read_UartItech((unsigned char*)buf, length)) ;
}


int Uart2_Read(char *buf, unsigned int length)
{
  int int_Flag;

  int_Flag = read(int_Sci2FD, buf, length); //返回实际读取的字符数目
  if( int_Flag == 0 )
  {
    //LOG_I("<uart> - (BMU)uart2 get data nothing");
    return 0;
  }
  else if(int_Flag < 0)
  {
   // LOG_E("<uart> -- (BMU)uart2 read error:%d.", errno);
    //perror("<uart> - (BMU)uart2 read") ;
    return int_Flag;
  }
  else
  {
    //LOG_E("<uart> - (BMU)uart2 read normal");
    return int_Flag;
  }
  return 1 ;
}


int Uart1_Write(char *buf, unsigned int length)
{
  int int_Flag;

  int_Flag = write(int_Sci1FD,buf,length);
  if (int_Flag<0)
  {
    //#ifdef BMS_UART_DEBUG
      LOG_E("<uart> - (BEC)uart1 send data error:%d.",errno);
      perror("<uart> - (BEC)uart1") ;
    //#endif
    return BSP_ERROR;
  }

  return BSP_OK;
}


int Uart2_Write(char *buf, unsigned int length)
{
  int int_Flag = write(int_Sci2FD,buf,length);
  if (int_Flag<0)
  {
    LOG_E("<uart> - (BMU)uart2 send data error:%d.",errno);
    perror("<uart> - (BMU)uart2") ;
    return BSP_ERROR;
  }
  return BSP_OK;
}

void Uart1_ClearReadBuf(void)
{
  //LOG_E("\n<uart> - (BEC)uart1 -- clear buff////////");
//  char bBuf[512];
//  Uart1_Read(bBuf, sizeof(bBuf) ) ;
  //  LOG_E("<uart> - (BEC)uart1 -- clear buff////////\n");
}

void Uart2_ClearReadBuf(void)
{
  char bBuf[512];
 // LOG_E("\n<uart> - (BMU)uart2 -- clear buff////////");
 // Uart2_Read(bBuf, sizeof(bBuf)) ;
//  LOG_E("<uart> - (BMU)uart2 -- clear buff////////\n");
}

void Uart1_Close(void)
{
	if (-1 != int_Sci1FD)
		close(int_Sci1FD);
	int_Sci1FD = -1;
}


void Uart2_Close(void)
{
	if (-1 != int_Sci2FD)
		close(int_Sci2FD);
	int_Sci2FD = -1;
}



