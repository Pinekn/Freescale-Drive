/*******************************************************************************
*
*  FILE
*     Ioa.h
*
*  DESCRIPTION
*     The source file is for IO and anlog convert control;
*
*  COPYRIGHT
*     All rights reserved.
*
*  AUTHOR
*     pinekn
*
*  VERSION
*     1.01   2015-06-21    PINKEN    1st Editin ,Transplant to used as 4 In 1
*
*******************************************************************************/

/*******************************************************************************
* include files
*******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h> //文件控制定义
#include <errno.h> //错误代码定义
#include <unistd.h> //unix标准函数定义
#include <sys/ioctl.h>
#include <stdbool.h>
#include  "../inc/Ioa.h"
#include  "../inc/net_log.h"



/*******************************************************************************
* Defines
*******************************************************************************/



/*******************************************************************************
* Macros
*******************************************************************************/
#define  IOA_500W_BEC             0


#ifndef TRUE
  #define TRUE                    1
#endif

#ifndef FALSE
  #define FALSE                   0
#endif


#define GPIO_DEV                  "/dev/gpio"

#define BSP_OK                    0
#define BSP_ERROR                 -1


#define GPIO_CMD_ON               0x21
#define GPIO_CMD_OFF              0x31
#define GPIO_CMD_OUT              0x12
#define GPIO_CMD_IN               0x11
#define GPIO_CMD_STS              0x02





// bmu main relay turn off delay time ;
// to reduce the BEC current carsh when Charge/Discharge stop.
#define IOA_MAIN_RELAY_TURN_OFF_DELAY       10 //1s;
// bmu aux relay turn off delay time
//2015-11-10: 1.5min --> 0.5min
#define IOA_AUX_RELAY_TURN_OFF_DELAY        100 //10s ,short the packet current limit resistance delay time

/*******************************************************************************
* Global Constant definition
*******************************************************************************/

/*******************************************************************************
* Local Constant definition
*******************************************************************************/
struct Color {
    unsigned char r;
    unsigned char g;
    unsigned char b;
};

/*******************************************************************************
* Global Variables definition
*******************************************************************************/

Ioa_FaultTypes ioa_BmuFaultSt[MAX_PACKET_NUM] ;
char ioa_test_state ;

/*******************************************************************************
* Local Variables definition
*******************************************************************************/
Ioa_CtrlTypes ioa_BmuRelay ;

static int ioa_currKeySt ;
static int rgb_fd ;
/*******************************************************************************
* Local Functions prototypes
*******************************************************************************/

static int Ioa_SetGpioHigh(int num);
static int Ioa_SetGpioLow(int num);
static int Ioa_SetGpioDirIn(int num);
static int Ioa_SetGpioDirOut(int num);
static int Ioa_ReadGpioSts(int num);

int Ioa_BmuAuxRelayOn( unsigned int packetNo ) ;
int Ioa_BmuAuxRelayOff( unsigned int packetNo ) ;

int Ioa_BecRelayOn(void) ;
int Ioa_BecRelayOff(void) ;

/*******************************************************************************
* Global Functions prototypes
*******************************************************************************/

int Ioa_Init(void);
void Ioa_Task(void);
int Ioa_BmuPwrOn(void);
int Ioa_BmuPwrOff(void);
unsigned char Ioa_GetBmuRelaySt( unsigned int packetNo );
int Ioa_BmuWakeupHigh(void);
int Ioa_BmuWakeupLow(void);
int Ioa_BmuFaultMonSts(void);
int Ioa_BmuMainRelayOn( unsigned int packetNo ) ;
int Ioa_BmuMainRelayOff( unsigned int packetNo ) ;
int Ioa_BmuOvercurrent( unsigned int packetNo );
int Ioa_BmuRelayStFeedback( unsigned int packetNo );
void Ioa_LedRgb_Red_sharve(void) ;
void Ioa_LedRgb_green_Keep(void) ;
void Ioa_LedRgb_green_sharve(void) ;

#define Ioa_BMU1UartEn()      Ioa_BmuUartChEn(IOA_BMU_CH1)
#define Ioa_BMU2UartEn()      Ioa_BmuUartChEn(IOA_BMU_CH2)

#define Ioa_BMU1PaketOff()    Ioa_SetGpioLow(IOA_BMU1_MAIN_RELAY_PIN)   // power off default state
#define Ioa_BMU2PaketOff()    Ioa_SetGpioLow(IOA_BMU2_MAIN_RELAY_PIN)   // power off default state
#define Ioa_BMU1PaketOn()     Ioa_SetGpioHigh(IOA_BMU1_MAIN_RELAY_PIN)   // power on
#define Ioa_BMU2PaketOn()     Ioa_SetGpioHigh(IOA_BMU2_MAIN_RELAY_PIN)   // power on

#define Ioa_IsMainRelayOffCfm(ch)  (ioa_BmuRelay.MainRelaySt[ch] == IOA_RELAY_TURN_OFF)
#define Ioa_IsAllMainRelayReqOn()     (ioa_BmuRelay.AuxRelayChangeTime != 0)
#define Ioa_IsAllAuxRelayOnCfm()   (ioa_BmuRelay.AuxRelayChangeTime >= 10)

#define Ioa_IsOneMainRelayReqOn()   \
                                  ( ((ioa_BmuRelay.MainRelayCtrl[IOA_BMU_CH1] == IOA_RELAY_TURN_ON) &&       \
                                    (ioa_BmuRelay.MainRelayCtrl[IOA_BMU_CH2] == IOA_RELAY_TURN_OFF) )  ||    \
                                   ((ioa_BmuRelay.MainRelayCtrl[IOA_BMU_CH1] == IOA_RELAY_TURN_OFF) &&       \
                                    (ioa_BmuRelay.MainRelayCtrl[IOA_BMU_CH2] == IOA_RELAY_TURN_ON) ) )


/*******************************************************************************
* NAME:             Ioa_Init
* CALLED BY:        Application
* PRECONDITIONS:
* INPUT PARAMETERS: Void
* RETURN VALUES:    Void
* DESCRIPTION:      Ioa Init.
*******************************************************************************/
int Ioa_Init(void)
{
	int tmp ;

	tmp = BSP_OK ;

	tmp |= Ioa_SetGpioDirOut(IOA_BMU1_AUX_RELAY_PIN) ;
	tmp |= Ioa_SetGpioDirOut(IOA_BMU1_MAIN_RELAY_PIN) ;
	tmp |= Ioa_SetGpioDirIn(IOA_BMU1_OVERCURRENT_PIN) ;

	tmp |= Ioa_SetGpioDirOut(IOA_BMU2_AUX_RELAY_PIN) ;
	tmp |= Ioa_SetGpioDirOut(IOA_BMU2_MAIN_RELAY_PIN) ;
	tmp |= Ioa_SetGpioDirIn(IOA_BMU2_OVERCURRENT_PIN) ;

	tmp |= Ioa_SetGpioDirOut(IOA_BMU12_POWER_PIN) ;
	tmp |= Ioa_SetGpioDirOut(IOA_BMU12_WAKE_PIN) ;
	tmp |= Ioa_SetGpioDirIn(IOA_BMU12_FAULT_PIN) ;

	tmp |= Ioa_SetGpioDirOut(IOA_BMU_UART_SEL_PIN) ;
	tmp |= Ioa_SetGpioDirIn(IOA_AC_OUT_S2_PIN) ;
	tmp |= Ioa_SetGpioDirIn(IOA_AC_OUT_S1_PIN) ;
	tmp |= Ioa_SetGpioDirIn(IOA_BEC_RELAY_PIN) ;

	tmp |= Ioa_SetGpioDirIn(IOA_WPC_EN_PIN) ;
	tmp |= Ioa_SetGpioDirIn(IOA_LED_PWM_PIN) ;

	for ( unsigned char i = 0; i < MAX_PACKET_NUM; i++ )
	{
		ioa_BmuRelay.MainRelayCtrl[i] = IOA_RELAY_TURN_OFF ;
		ioa_BmuRelay.MainRelaySt[i]   = IOA_RELAY_TURN_OFF ;
		ioa_BmuRelay.AuxRelayCtrl[i]  = IOA_RELAY_TURN_OFF ;
		ioa_BmuRelay.AuxRelaySt[i]    = IOA_RELAY_TURN_OFF ;

		ioa_BmuFaultSt[i].BmuChipFaultSt = 0 ;
		ioa_BmuFaultSt[i].HWOverCurrSt   = 0 ;
	}

	ioa_currKeySt = 1 ;
	Ioa_BmuPwrOn();


  //Ioa_BmuAuxRelayOn(IOA_BMU_CH1); // use the limmitted packet current resistance
  //Ioa_BmuAuxRelayOn(IOA_BMU_CH2);

    rgb_fd = open("/dev/led0", O_RDWR);
    if(rgb_fd < 0) 
    {
        LOG_I("\n\n --------------- open device error---------------------\n\n");
        return -1;
    }

    ioa_test_state = 0 ;

	if(tmp == BSP_OK)
	{
		return BSP_OK ;
	}
	else
	{
		return BSP_ERROR ;
	}
}


void Ioa_Task(void)
{
	Ioa_CheckStartkeySt();

	if (ioa_test_state == IOA_TEST_ST_FAILD)
	{
		Ioa_LedRgb_Red_sharve();
	}
	else if(ioa_test_state == IOA_TEST_ST_PASS)
	{
		Ioa_LedRgb_green_Keep();
	}
	else 
	{
		Ioa_LedRgb_green_sharve();
	}
	
}

int Ioa_GetStartkeySt(void)
{
	if(ioa_currKeySt == 0 ) // start
	{
		return IOA_ATE_KEY_START ;
	}
	else // stop 
	{
		return IOA_ATE_KEY_STOP ;
	}
}

int Ioa_CheckStartkeySt(void)
{
	#define IOA_ATE_KEY_CNT 		3
	static int ioa_keyStCnt = 0;

	int st = Ioa_ReadGpioSts(IOA_AC_OUT_S1_PIN);

	if( st !=  ioa_currKeySt )
	{
		if( ioa_keyStCnt < IOA_ATE_KEY_CNT )
		{
			ioa_keyStCnt++ ;
		}
		else
		{
			ioa_currKeySt = st ;
			ioa_keyStCnt = 0 ;
			LOG_I("----------------------ioa_curr_key_st=%d.----------------------------\n", ioa_currKeySt) ;
		}
	}

	return 1 ;
}


void Ioa_CleanHwProtect(void) 
{
	Ioa_SetGpioHigh( IOA_BMU12_POWER_PIN );
}


int Ioa_BmuPwrOn(void)
{
	LOG_E("<IOA> - BMU Pwr On. ");
	return Ioa_SetGpioLow( IOA_BMU12_POWER_PIN );
}

int Ioa_BmuPwrOff(void)
{
	LOG_E("<IOA> - BMU Pwr Off. ");
	return Ioa_SetGpioHigh( IOA_BMU12_POWER_PIN );
}


unsigned char Ioa_GetBmuRelaySt(unsigned int packetNo)
{
   return ioa_BmuRelay.MainRelaySt[packetNo] ;
}

void Ioa_LedRgb_Red_sharve(void) // test finished, faild ;
{
	static int color_red_chg = 0 ;
	struct Color color_red;
	struct Color color_white;

	color_red.r = 254 ;
	color_red.g = 0  ;
	color_red.b = 0  ;

	color_white.r = 0 ;
	color_white.g = 0 ;
	color_white.b = 0 ;

	color_red_chg++ ;

	if (color_red_chg == 1)
	{
		ioctl(rgb_fd, 4, (void *)&color_red);
		//LOG_E("<IOA> - read color. ");
	}
	else if( color_red_chg == 6)
	{
		ioctl(rgb_fd, 4, (void *)&color_white);
		//LOG_E("<IOA> - white color. ");
	}
	else if (color_red_chg >= 10)
	{
		color_red_chg = 0 ;
	}
//LOG_E("<IOA> - color_red_chg=%d. ", color_red_chg);
}


void Ioa_LedRgb_green_Keep(void) // test finished, Ok ; 
{
	struct Color color_grn;

	color_grn.r = 0 ;
	color_grn.g = 250  ;
	color_grn.b = 0  ;

	ioctl(rgb_fd, 4, (void *)&color_grn);
}

void Ioa_LedRgb_green_sharve(void) //running,device in testting 
{
	static int color_grn_chg = 0 ;
	struct Color color_grn;
	struct Color color_white;

	color_grn.r = 0 ;
	color_grn.g = 250  ;
	color_grn.b = 0  ;

	color_white.r = 0 ;
	color_white.g = 0 ;
	color_white.b = 0 ;

	color_grn_chg++ ;

	if (color_grn_chg == 1)
	{
		ioctl(rgb_fd, 4, (void *)&color_grn);
	}
	else if( color_grn_chg == 6)
	{
		ioctl(rgb_fd, 4, (void *)&color_white);
	}
	else if (color_grn_chg >= 10)
	{
		color_grn_chg = 0 ;
	}

}

unsigned char Ioa_IsBmuMainRelayTurnOffCfm( unsigned int packetNo )
{
	if( (Ioa_GetBmuRelaySt(packetNo) == IOA_RELAY_TURN_OFF) && (Ioa_BmuRelayStFeedback(packetNo) == IOA_RELAY_TURN_OFF) )
	{
		return TRUE ;
	}
	else
	{
		return FALSE ;
	}
}

unsigned char Ioa_IsBmuMainRelayTurnOnCfm( unsigned int packetNo )
{
	if( (Ioa_GetBmuRelaySt(packetNo) == IOA_RELAY_TURN_ON) && (Ioa_BmuRelayStFeedback(packetNo) == IOA_RELAY_TURN_ON) )
	{
		return TRUE ;
	}
	else
	{
		return FALSE ;
	}
}

unsigned char Ioa_IsBmuMainRelayInDelay( unsigned int packetNo )
{
	if( (Ioa_GetBmuRelaySt(packetNo) == IOA_RELAY_TURN_OFF) && (Ioa_BmuRelayStFeedback(packetNo) == IOA_RELAY_TURN_ON) )
	{
		return TRUE ;
	}
	else
	{
		return FALSE ;
	}

}

unsigned char Ioa_IsBmuMainRelayProtected( unsigned int packetNo )
{
	if( (Ioa_GetBmuRelaySt(packetNo) == IOA_RELAY_TURN_ON) && (Ioa_BmuRelayStFeedback(packetNo) == IOA_RELAY_TURN_OFF) )
	{
		return TRUE ;
	}
	else
	{
		return FALSE ;
	}

}


unsigned char Ioa_GetBmuAuxRelaySt(unsigned int packetNo)
{
	if( packetNo < MAX_PACKET_NUM)
	{
		return ioa_BmuRelay.AuxRelaySt[packetNo] ;
	}
	else
	{
		return 0xFF ;
	}
}

unsigned char Ioa_GetBmuAuxRelayAllOnSt(void)
{
    if( (ioa_BmuRelay.AuxRelaySt[IOA_BMU_CH1] == IOA_RELAY_TURN_ON) &&
        (ioa_BmuRelay.AuxRelaySt[IOA_BMU_CH2] == IOA_RELAY_TURN_ON) )
    {
        return 1 ;
    }
    else
    {
        return 0 ;
    }
}

int Ioa_BmuWakeupHigh(void)
{
  //printf("Bmu wakeup pin set to high.\n");
  return Ioa_SetGpioLow(IOA_BMU12_WAKE_PIN);
}

int Ioa_BmuWakeupLow(void)
{
  //printf("Bmu wakeup pin set to Low.\n");
  return Ioa_SetGpioHigh(IOA_BMU12_WAKE_PIN);
}

int Ioa_BmuUartChEn(char ChNo)
{
  if( ChNo == IOA_BMU_CH1 )
  {
    return Ioa_SetGpioLow(IOA_BMU_UART_SEL_PIN);
  }
  else if( ChNo == IOA_BMU_CH2 )
  {
    return Ioa_SetGpioHigh(IOA_BMU_UART_SEL_PIN);
  }
  else
  {
    return BSP_ERROR;
  }

}
/** brief
 * param[in]  pin
 * param[out] none
 * return     success: io level (>=0)
 *            fail: BSP_ERROR
 */
int Ioa_BmuFaultMonSts()
{
  return Ioa_ReadGpioSts(IOA_BMU12_FAULT_PIN);
}

int Ioa_BmuMainRelayOn( unsigned int packetNo )
{

	if( packetNo >= MAX_PACKET_NUM )
	{
		return -2 ;
	}
	else
	{
		int num = 0 ;
		if ( packetNo == IOA_BMU_CH1 )
		{
			num = IOA_BMU1_MAIN_RELAY_PIN ;
		}
		else if ( packetNo == IOA_BMU_CH2 )
		{
			num = IOA_BMU2_MAIN_RELAY_PIN ;
		}
		else
		{
			return 0;
		}
	//	LOG_I("<IOA> - P%d Bmu Relay Turn On.",packetNo);
    	return Ioa_SetGpioHigh(num);
	}

}

int Ioa_BmuMainRelayOff( unsigned int packetNo )
{

	if( packetNo >= MAX_PACKET_NUM )
	{
		return -2 ;
	}
	else
	{
        int num = 0 ;
		if ( packetNo == IOA_BMU_CH1 )
		{
	    	num = IOA_BMU1_MAIN_RELAY_PIN ;
		}
		else if ( packetNo == IOA_BMU_CH2 )
		{
	    	num = IOA_BMU2_MAIN_RELAY_PIN ;
		}

	//	LOG_I("<IOA> - P%d Bmu Relay Turn Off.",packetNo);
		return Ioa_SetGpioLow( num );
    }
}

int Ioa_BmuAuxRelayOn( unsigned int packetNo )
{
  // short the limmitted packet current resistance
	static int relay_On_debonce[MAX_PACKET_NUM] = {0} ;

	if( packetNo >= MAX_PACKET_NUM )
	{
		return -2 ;
	}

	if ( ioa_BmuRelay.AuxRelaySt[packetNo] == IOA_RELAY_TURN_OFF )
	{
		if( relay_On_debonce[packetNo] >= 4 )
		{
			relay_On_debonce[packetNo] = 0 ;
			ioa_BmuRelay.AuxRelaySt[packetNo] = IOA_RELAY_TURN_ON ;
			LOG_I("<IOA> - P%d Aux Bmu Relay Turn On.",packetNo);
		}
		else
		{
			relay_On_debonce[packetNo]++;
		}

		int num = 0 ;

		if ( packetNo == IOA_BMU_CH1 )
		{
			num = IOA_BMU1_AUX_RELAY_PIN ;
		}
		else if ( packetNo == IOA_BMU_CH2 )
		{
			num = IOA_BMU2_AUX_RELAY_PIN ;
		}
		return Ioa_SetGpioHigh( num );
	}
	else
	{
		return 0 ;
	}
}

int Ioa_BmuAuxRelayOff( unsigned int packetNo )
{
  // use the resistance to limmitted packet crash current
	if( packetNo >= MAX_PACKET_NUM )
	{
		return -2 ;
	}

	if ( ioa_BmuRelay.AuxRelaySt[packetNo] == IOA_RELAY_TURN_ON )
	{
		int num = 0 ;
		ioa_BmuRelay.AuxRelaySt[packetNo] = IOA_RELAY_TURN_OFF ;

		if ( packetNo == IOA_BMU_CH1 )
		{
			num = IOA_BMU1_AUX_RELAY_PIN ;
		}
		else if ( packetNo == IOA_BMU_CH2 )
		{
			num = IOA_BMU2_AUX_RELAY_PIN ;
		}

		LOG_I("<IOA> - P%d Aux Bmu Relay Turn Off.",packetNo);
		return Ioa_SetGpioLow(num);
	}
	else
	{
		return -1 ;
	}
}


int Ioa_BmuRelayStFeedback( unsigned int packetNo )
{
    int tmp,ch ;

    if ( packetNo == IOA_BMU_CH1 )
	{
		ch = IOA_BMU1_OVERCURRENT_PIN ;
    }
    else if ( packetNo == IOA_BMU_CH2 )
	{
		ch = IOA_BMU2_OVERCURRENT_PIN ;
	}
	else
	{
		return -1 ;
	}

    tmp = Ioa_ReadGpioSts(ch)  ;

    if( tmp == 1)
    {
      return IOA_RELAY_TURN_ON ;
    }
    else
    {
      return IOA_RELAY_TURN_OFF ;
    }
}



static int Ioa_SetGpioHigh(int num)
{
    int fd;

    fd = open(GPIO_DEV, O_RDWR);
    if(fd < 0)
    {
        return BSP_ERROR;
    }

    if(ioctl(fd, GPIO_CMD_ON, &num)<0)
    {
        return BSP_ERROR;
    }
    close(fd);

    return BSP_OK;
}

static int Ioa_SetGpioLow(int num)
{
    int fd;

    fd = open(GPIO_DEV, O_RDWR);
    if(fd < 0) 
    {
        return BSP_ERROR;
    }

    if(ioctl(fd, GPIO_CMD_OFF, &num)<0)
    {
        return BSP_ERROR;
    }
    close(fd);

    return BSP_OK;
}

static int Ioa_SetGpioDirIn(int num)
{
    int fd;
    fd = open(GPIO_DEV, O_RDWR);

    if(fd < 0)
    {
        return BSP_ERROR;
    }

    if(ioctl(fd, GPIO_CMD_IN, &num)<0) 
    {
        return BSP_ERROR;
    }
    close(fd);

    return BSP_OK;
}

static int Ioa_SetGpioDirOut(int num)
{
    int fd;

    fd = open(GPIO_DEV, O_RDWR);
    if(fd < 0) 
    {
        return BSP_ERROR;
    }

    if(ioctl(fd, GPIO_CMD_OUT, &num)<0) 
    {
        return BSP_ERROR;
    }
    close(fd);

    return BSP_OK;
}


static int Ioa_ReadGpioSts(int num)
{
    int fd;

    fd = open(GPIO_DEV, O_RDWR);
    if(fd < 0)
    {
        return BSP_ERROR;
    }

    if(ioctl(fd, GPIO_CMD_STS, &num)<0) 
    {
        return BSP_ERROR;
    }
    close(fd);

    return num;
}
