/*******************************************************************************
* include files
*******************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h> /* memset */

#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/ioctl.h>

#include "../inc/net_log.h"
#include "../inc/Ioa.h"
#include "../inc/uart.h"
#include "../inc/CRC.h"
#include "../inc/PL455A.h"
/*******************************************************************************
* Defines
*******************************************************************************/
#define APOLLO_PL455_REINIT_DEBUG         	0
#define APOLLO_PRIMEVAL_DEBUG           	0
#define PL455_MSG_FRAME_DEBUG           	1
#define PL455_INIT_INFO_DEDUG             	1
//#define PL455_TX_BUFF_DEBUG
//#define PL455_RX_BUFF_DEBUG
#define PL455_CORRECT_PAR_DEBUG				    1
#define PL455_MAIN_TASK_DEBUG				      0



#define TRUE                            1
#define FALSE                           0
//#define PL455_BAUDRATE_REG            (PL455_BAUDRATE_CFG<<4)
#define PL455_BAUDRATE_REG              0x10

/*******************************************************************************
* Macros
*******************************************************************************/
#define CELL_VOLT_SAMPLE_IDX         1
#define PACKET_VOLT_SAMPLE_IDX       1+MAX_CELL_NUM_PER_MOUDLE*2
#define PACKET_CURR_SAMPLE_IDX       1+MAX_CELL_NUM_PER_MOUDLE*2
/*******************************************************************************
* Global Constant definition
*******************************************************************************/

/*******************************************************************************
* Local Constant definition
*******************************************************************************/
static const int PL455_Temp_table[146] =
{
   331498, 309738, 289712, 271242, 254175, 238375, 223722, 210113,  /* -40C -- -33C */
   197455, 185664, 174669, 164404, 154810, 145835, 137432, 129558,  /* -32C -- -25C */
   122175, 115248, 108744, 102636,  96895,  91498,  86422,  81646,  /* -24C -- -17C */
    77152,  72922,  68939,  65188,  61656,  58328,  55192,  52238,  /* -16C --  -9C */
    49454,  46830,  44356,  42025,  39827,  37755,  35802,  33960,  /*  -8C --  -1C */
    32223,  30585,  29041,  27584,  26211,  24915,  23692,  22539,  /*   0C --   7C */
    21451,  20425,  19456,  18542,  17679,  16865,  16097,  15371,  /*   8C --  15C */
    14686,  14039,  13428,  12851,  12306,  11791,  11305,  10845,  /*  16C --  23C */
    10411,  10000,   9565,   9152,   8760,   8387,   8033,   7696,  /*  24C --  31C */
     7376,   7072,   6781,   6505,   6241,   5990,   5751,   5522,  /*  32C --  39C */
     5304,   5096,   4897,   4706,   4525,   4351,   4185,   4026,  /*  40C --  47C */
     3873,   3728,   3588,   3455,   3327,   3204,   3086,   2974,  /*  48C --  55C */
     2866,   2762,   2663,   2567,   2476,   2388,   2303,   2222,  /*  56C --  63C */
     2145,   2070,   1998,   1929,   1863,   1799,   1737,   1678,  /*  64C --  71C */
     1622,   1567,   1515,   1464,   1416,   1369,   1324,   1280,  /*  72C --  79C */
     1239,   1199,   1160,   1123,   1087,   1052,   1019,    986,  /*  80C --  87C */
      955,    926,    897,    869,    842,    816,    791,    767,  /*  88C --  95C */
      744,    722,    700,    679,    659,    640,    621,    603,  /*  96C --  103C */
      585,    568,                                                  /* 104C --  105C */
 };


/*******************************************************************************
* Global Variables definition
*******************************************************************************/
unsigned short PL455_CellVolt[MAX_PACKET_NUM][MAX_MOUDLE_NUM][MAX_CELL_NUM_PER_MOUDLE];
		 short PL455_CellTemp[MAX_PACKET_NUM][MAX_MOUDLE_NUM][MAX_CELL_NUM_PER_MOUDLE];
 		 int   PL455_DigDieTemp[MAX_PACKET_NUM][MAX_MOUDLE_NUM] ;
 		 int   PL455_AnalogDieTemp[MAX_PACKET_NUM][MAX_MOUDLE_NUM] ;
		 int   PL455_VoltMoudle[MAX_PACKET_NUM][MAX_MOUDLE_NUM] ;
		 short PL455_PacketCurr[MAX_PACKET_NUM] ;
 unsigned short PL455_PacketVolt[MAX_PACKET_NUM] ;
 unsigned char PL455_CommErrSt[MAX_PACKET_NUM];
 unsigned char PL455_InitSt[MAX_PACKET_NUM];
 PL455_FaultSummaryTypes PL455_HWFaultSum[MAX_PACKET_NUM] ;
 unsigned char PL455_SampleEn[MAX_PACKET_NUM] ;
 PL455_FaultSenseType Pl455_Fault[MAX_PACKET_NUM][MAX_MOUDLE_NUM];
/*******************************************************************************
* Local Variables declare
*******************************************************************************/
static unsigned char PL455_ReInitEn[MAX_PACKET_NUM];
static unsigned char PL455_CellVoltNormalCnt[MAX_PACKET_NUM][MAX_MOUDLE_NUM][MAX_CELL_NUM_PER_MOUDLE];
static unsigned char PL455_CellVoltErrCnt[MAX_PACKET_NUM][MAX_MOUDLE_NUM][MAX_CELL_NUM_PER_MOUDLE];
static unsigned char PL455_IsCellVoltNormal[MAX_PACKET_NUM][MAX_MOUDLE_NUM][MAX_CELL_NUM_PER_MOUDLE];

static unsigned char PL455_PacketVoltNormalCnt[MAX_PACKET_NUM] ;
static unsigned char PL455_PacketVoltErrCnt[MAX_PACKET_NUM] ;
static unsigned char PL455_IsPacketVoltNormal[MAX_PACKET_NUM];

static unsigned char PL455_PacketCurrNormalCnt[MAX_PACKET_NUM] ;
static unsigned char PL455_PacketCurrErrCnt[MAX_PACKET_NUM] ;
static unsigned char PL455_IsPacketCurrNormal[MAX_PACKET_NUM];

static unsigned char PL455_CellTempNormalCnt[MAX_PACKET_NUM][MAX_MOUDLE_NUM][MAX_TEMP_NUM_PER_MOUDLE];
static unsigned char PL455_CellTempErrCnt[MAX_PACKET_NUM][MAX_MOUDLE_NUM][MAX_TEMP_NUM_PER_MOUDLE];
static unsigned char PL455_IsCellTempNormal[MAX_PACKET_NUM][MAX_MOUDLE_NUM][MAX_TEMP_NUM_PER_MOUDLE];

static unsigned char PL455_CommNormalCnt[MAX_PACKET_NUM];
static unsigned char PL455_CommErrCnt[MAX_PACKET_NUM];

static PL455_StatusTypes PL455_DeviceStatus[MAX_PACKET_NUM] ;
static PL455_AdcCorrectionType PL455_Cali[MAX_PACKET_NUM] ;

static unsigned short PL455_TestBoardRef1 ;
static unsigned short PL455_TestBoardRef2 ;


#if(APOLLO_PRIMEVAL_DEBUG == 1)
void PL455_PrimevalValDebug(unsigned char *pBuf , unsigned int packetNo);
#endif
/*******************************************************************************
* Global Functions prototypes
*******************************************************************************/
void PL455_Init(unsigned int packetNo);
int PL455_Task(unsigned int packetNo);
void PL455_Sleep(unsigned int packetNo);

/*******************************************************************************
* Local Functions prototypes
*******************************************************************************/
#define PL455_Delay1ms()            for(int k=0;k<0xE000;k++) ; // delay 1ms
#define PL455_Delay5ms()            for(int k=0;k<0x40040;k++) ;
#define PL455_Delay20ms()			      for(int m=0;m<4;m++) PL455_Delay5ms() ;
#define PL455_Delay30ms()			      for(int m=0;m<6;m++) PL455_Delay5ms() ;

static void PL455_ReInit(unsigned int packetNo);
void PL455_WakeUp(unsigned int packetNo);
static int PL455_SetAddress(void);
static int PL455_ComPortInit(unsigned int packetNo);
static int PL455_AdcConfigure(void);
static int PL455_FaultSensorInit(void);
static int PL455_WriteReg(unsigned char deviceID, unsigned short regAddr,
                          unsigned char *bData, unsigned char bLen, unsigned char ReqType);
static int PL455_WriteFrame(unsigned char deviceID, unsigned short regAddr, unsigned char * pData,
                            unsigned char bLen, unsigned char ReqType);
static int PL455_WaitRespFrame(unsigned char *pFrame, unsigned char bLen, unsigned int dwTimeOut);
static int PL455_BinSearch(int *array, int n, int target);

static void PL455_CheckCellVolt(unsigned char *pBuf, unsigned char nDev_ID, unsigned int packetNo );
static void PL455_CheckCellTemp(unsigned char *pBuf, unsigned char nDev_ID, unsigned int packetNo );
static void PL455_CheckDieTemp(unsigned char *pBuf, unsigned char nDev_ID, unsigned int packetNo);
static void PL455_CheckMoudleVolt(unsigned char *pBuf, unsigned char nDev_ID, unsigned int packetNo);
static void PL455_CheckPacketVolt(unsigned char *pBuf, unsigned int packetNo);
static void PL455_CheckPacketCurr(unsigned char *pBuf, unsigned int packetNo);
static void PL455_CheckComm(unsigned char ErrSt, unsigned int packetNo);
static int PL455_CheckFaultSensor(unsigned int packetNo) ;

static void PL455_GetBmuConfig(unsigned int packetNo) ;
static void PL455_CTO(void);
static void PL455_CheckRefVolt(unsigned char *pBuf, unsigned char nDev_ID, unsigned int packetNo) ;


void PL455_ExceptionHand(unsigned int packetNo); //debug
int PL455_TestTask(unsigned int packetNo );
int PL455_GetCorrectPar(unsigned int packetNo);
int PL455_ResetCorrectPar(unsigned int packetNo ) ;
int PL455_GetPacketVlot(unsigned int packetNo ) ;
int PL455_GetPacketCurrent(unsigned int packetNo ) ;

void PL455_SaveReg_TB(unsigned int packetNo, char *buff);

#define APOLLO_PL455_DEBUG_FUNC  1

#if( APOLLO_PL455_DEBUG_FUNC == 1)
int PL455_DebugFunc(unsigned int packetNo) ;
#endif

void PL455_Set_HWVersion(unsigned int packetNo) ;


/*******************************************************************************
*  Global Functions Body
*******************************************************************************/

/*******************************************************************************
* NAME:             PL455_Init
* CALLED BY:        Alm_Init
* PRECONDITIONS:
* INPUT PARAMETERS: Void
* RETURN VALUES:    Void
* DESCRIPTION:        .
*******************************************************************************/
void PL455_Init(unsigned int packetNo)
{

	Ioa_BmuUartChEn(packetNo) ;
	usleep(100);

  	PL455_WakeUp(packetNo);
	PL455_SetAddress();

	PL455_ComPortInit( packetNo);

	PL455_AdcConfigure();

	PL455_GetCorrectPar(packetNo);
	//PL455_GetBmuConfig(packetNo);

  	PL455_CTO();
	for(unsigned char nDev_ID=0;nDev_ID < MAX_MOUDLE_NUM;nDev_ID++)
	{
	  for(unsigned char i=0;i<MAX_CELL_NUM_PER_MOUDLE;i++)
		{
			PL455_CellVolt[packetNo][nDev_ID][i] = 0xFFFF ;
			PL455_IsCellVoltNormal[packetNo][nDev_ID][i] = 1 ;
			PL455_CellVoltNormalCnt[packetNo][nDev_ID][i] = 0 ;
			PL455_CellVoltErrCnt[packetNo][nDev_ID][i] = 0 ;
		}
	}

	for(unsigned char nDev_ID=0;nDev_ID < MAX_MOUDLE_NUM;nDev_ID++)
	{
	  for(unsigned char i=0;i<MAX_TEMP_NUM_PER_MOUDLE;i++)
		{
			PL455_CellTemp[packetNo][nDev_ID][i] = 0x7FFF ;
			PL455_IsCellTempNormal[packetNo][nDev_ID][i] = 1 ;
			PL455_CellTempNormalCnt[packetNo][nDev_ID][i] = 0 ;
			PL455_CellTempErrCnt[packetNo][nDev_ID][i] = 0 ;
		}
	}

	PL455_PacketVolt[packetNo]          = 0xFFFF ;
	PL455_IsPacketVoltNormal[packetNo]  = 1 ;
	PL455_PacketVoltNormalCnt[packetNo] = 0 ;
	PL455_PacketVoltErrCnt[packetNo]    = 0 ;

	PL455_PacketCurr[packetNo]          = 0x7FFF ;
	PL455_IsPacketCurrNormal[packetNo]  = 1 ;
	PL455_PacketCurrNormalCnt[packetNo] = 0 ;
	PL455_PacketCurrErrCnt[packetNo]    = 0 ;

	PL455_CommNormalCnt[packetNo] = 0 ;
	PL455_CommErrCnt[packetNo]    = 0 ;
	PL455_CommErrSt[packetNo]     = PL455_COMM_ST_NORMAL ;

	PL455_InitSt[packetNo]   = PL455_INIT_ST_NA ; //NA
	PL455_ReInitEn[packetNo] = FALSE ;

}


/*******************************************************************************
* NAME:             PL455_Task
* CALLED BY:        Alm_Task
* PRECONDITIONS:
* INPUT PARAMETERS:
* RETURN VALUES:
* DESCRIPTION:        .
*******************************************************************************/
int PL455_Task(unsigned int packetNo )
{
    static int mChoose[2]   = {0} ;
    static int mNomalCnt[2] = {0} ;
    int IsDataNormal = 0 ;
    int nSent ;
    unsigned char bReadBuf[512], bFrame[8], nDev_ID ;

    Ioa_BmuUartChEn(packetNo);
    usleep(500);

    if( PL455_ReInitEn[packetNo] ==  TRUE ) // error
    {
      PL455_ExceptionHand(packetNo);
      mChoose[packetNo] = 0 ;
      mNomalCnt[packetNo] = 0 ;
      return -1 ;
    }

    if( mNomalCnt[packetNo] == 0 )
    {
		// Send broadcast request to all boards to sample and store results (section 4.2.1)
		bFrame[0] = 0x00 ;
		nSent = PL455_WriteReg(0, 2, bFrame, 1, FRMWRT_ALL_NR); // send sync sample command
		mNomalCnt[packetNo] =  1 ;
		return 0 ;
	}
	else
	{
		mNomalCnt[packetNo] = 0 ;
	}

    memset(bReadBuf, 0xFF, sizeof(bReadBuf));

	for( nDev_ID=0; nDev_ID <2; nDev_ID++ )
	{
		IsDataNormal = 0 ; // clear the flag
		bFrame[0] = 0x20 ;
		int nSent = PL455_WriteReg(nDev_ID, 2, bFrame, 1, FRMWRT_SGL_R); // send read sampled values command

		usleep(8000); //5.5ms --> 8ms; wait for data transmit
		nSent = PL455_WaitRespFrame(bReadBuf, 120, 0); // 28 bytes data + packet header + CRC, 0ms timeout

		if( nSent <= -2 )
		{
			LOG_E("<PL455> - P%d M%d :Error! CRC error.",packetNo, nDev_ID );
		}
		else if( nSent == -1 )
		{
			LOG_E("<PL455> - P%d M%d :Error!Read Uart Error.",packetNo, nDev_ID );
		}
		else if( nSent == 0 )
		{
			LOG_E("<PL455> - P%d M%d : Error!Rx data is empty.",packetNo,nDev_ID);
		}
		else if( nSent != 48 )
		{
			LOG_E("<PL455> - P%d M%d :Error!Rx data length expect as 48,actual is %d.",packetNo,nDev_ID, nSent );
		}
		else
		{
			//normal. data is vaild.
			mNomalCnt[packetNo]++ ; //Communication normal count
			IsDataNormal = 1 ;
		}

		if( IsDataNormal == 0 ) //Communication error
		{
			#if 0
			bFrame[0] = 0xFF ;
			bFrame[1] = 0xE9 ;
			PL455_WriteReg(nDev_ID, 94, bFrame, 2, FRMWRT_SGL_NR); // reset fault ;
			usleep(500);
			#endif
			 /* Clear all faults */
			bFrame[0] = 0xFF ;
			bFrame[1] = 0xC0 ;
			nSent = PL455_WriteReg(nDev_ID, 82, bFrame, 2, FRMWRT_ALL_NR); // clear all fault summary flags

			bFrame[0] = 0x38 ;
			nSent = PL455_WriteReg(nDev_ID, 81, bFrame, 1, FRMWRT_ALL_NR); // clear fault flags in the system status register
		}
		else
		{
			/* Check cell voltage/cell temp/ packet voltage/packet current and debonce */
			PL455_CheckCellVolt(&bReadBuf[1], nDev_ID,packetNo) ;
			if( nDev_ID == 0 )
			{
				PL455_CheckPacketVolt( &bReadBuf[1+MAX_CELL_NUM_PER_MOUDLE*2],packetNo ) ;
				PL455_CheckPacketCurr( &bReadBuf[1+(1+MAX_CELL_NUM_PER_MOUDLE)*2],packetNo) ;
			}
			else
			{
				
			}

			PL455_CheckRefVolt( &bReadBuf[1+(2+MAX_CELL_NUM_PER_MOUDLE)*2], nDev_ID, packetNo ) ;
			PL455_CheckCellTemp(&bReadBuf[1+(2+MAX_CELL_NUM_PER_MOUDLE)*2], nDev_ID, packetNo) ;
			PL455_CheckDieTemp(&bReadBuf[1+(2+MAX_CELL_NUM_PER_MOUDLE+MAX_TEMP_NUM_PER_MOUDLE)*2], nDev_ID,packetNo) ;
			PL455_CheckMoudleVolt(&bReadBuf[1+(3+MAX_CELL_NUM_PER_MOUDLE+MAX_TEMP_NUM_PER_MOUDLE)*2], nDev_ID,packetNo);


			#if( APOLLO_PRIMEVAL_DEBUG  == 1)
			LOG_I("Packet No:%d",packetNo);
			PL455_PrimevalValDebug( bReadBuf, packetNo );
			#endif

		}
	}
	/* check the PL455 communication status */
	if( IsDataNormal == 1 )
	{
		if( mNomalCnt[packetNo] >= 2)
		{
			PL455_CheckComm( PL455_COMM_ST_NORMAL, packetNo);
		}
		else
		{
			PL455_CheckComm( PL455_COMM_ST_ERROR, packetNo);
		}
		mNomalCnt[packetNo] = 0 ; // clear normal counter

		int ret = PL455_DebugFunc(packetNo);

		if( ret == 1)
		{
			return 1 ;
		}
		else
		{
			return -2 ;
		}
	}
	else
	{
		return -3 ; // read data error.
	}

//PL455_CheckFaultSensor(packetNo);
//PL455_TestTask(packetNo);

	return nSent ;
}



static void PL455_ReadAllReg(unsigned int packetNo)
{
	unsigned char buff[128],j, nDev_ID ;
	unsigned char Frame  ;
	int nSent = 0 ;
	printf("Read register V:P%x.\n",packetNo);
	memset(buff, 0x00, sizeof(buff));
	nDev_ID = packetNo ;

	buff[0] = 0x00 ;
	nSent = PL455_WriteReg(nDev_ID, 0, buff, 1, FRMWRT_SGL_R); 
	nSent = PL455_WaitRespFrame(buff, 120, 0);
	if( nSent == 1 )
	{
		printf("Device Version 0x00 (0) Digital die version number):%x.\n",buff[1]);
	}	
	else
	{
		printf("error. 0.\n") ;
	} 
 

	buff[0] = 0x00 ;
	nSent = PL455_WriteReg(nDev_ID, 1, buff, 1, FRMWRT_SGL_R); 
	nSent = PL455_WaitRespFrame(buff, 120, 0);
	if( nSent == 1 )
	{
		printf("Device Version 0x01 (1) Analog die version number):%x.\n",buff[1]);
	}
	else
	{
		printf("error. 1.\n") ;
	} 
 

	//  CMD 0x02 (2) Command -- can not read

	buff[0] = 0x03 ;
	nSent = PL455_WriteReg(nDev_ID, 3, buff, 1, FRMWRT_SGL_R); 
	nSent = PL455_WaitRespFrame(buff, 120, 0);
	if( nSent == 4 )
	{
		printf("CHANNELS 0x03-06 (3-6) Channel Select:%x.%x.%x.%x\n",buff[1], buff[2], buff[3], buff[4]);
	}
	else
	{
		printf("error. 3-6.\n") ;
	} 
 

	buff[0] = 0x00 ;
	nSent = PL455_WriteReg(nDev_ID, 7, buff, 1, FRMWRT_SGL_R); 
	nSent = PL455_WaitRespFrame(buff, 120, 0);
	if( nSent == 1 )
	{
		printf("OVERSMPL 0x07 (7) Command Oversampling:%x.\n",buff[1] );
	}
	else
	{
		printf("error. 7.\n") ;
	} 
 

	buff[0] = 0x00 ;
	nSent = PL455_WriteReg(nDev_ID, 10, buff, 1, FRMWRT_SGL_R); 
	nSent = PL455_WaitRespFrame(buff, 120, 0);
	if( nSent == 1 )
	{
		printf("ADDR 0x0A (10) Device Address:%x.\n",buff[1] );
	}
	else
	{
		printf("error. 10.\n") ;
	} 
 

	buff[0] = 0x00 ;
	nSent = PL455_WriteReg(nDev_ID, 11, buff, 1, FRMWRT_SGL_R); 
	nSent = PL455_WaitRespFrame(buff, 120, 0);
	if( nSent == 1 )
	{
		printf("GROUP_ID 0x0B (11) Group ID:%x.\n",buff[1] );
	}
	else
	{
		printf("error. 11.\n") ;
	} 
 

	buff[0] = 0x00 ;
	nSent = PL455_WriteReg(nDev_ID, 12, buff, 1, FRMWRT_SGL_R); 
	nSent = PL455_WaitRespFrame(buff, 120, 0);
	if( nSent == 1 )
	{
		printf("DEV_CTRL 0x0C (12) Device Control:%x.\n",buff[1] );
	}
	else
	{
		printf("error. 12.\n") ;
	} 
 

	buff[0] = 0x00 ;
	nSent = PL455_WriteReg(nDev_ID, 13, buff, 1, FRMWRT_SGL_R); 
	nSent = PL455_WaitRespFrame(buff, 120, 0);
	if( nSent == 1 )
	{
		printf("NCHAN 0x0D (13) Number of Channels:%x.\n",buff[1] );
	}
	else
	{
		printf("error. 13.\n") ;
	} 
 

	buff[0] = 0x00 ;
	nSent = PL455_WriteReg(nDev_ID, 14, buff, 1, FRMWRT_SGL_R); 
	nSent = PL455_WaitRespFrame(buff, 120, 0);
	if( nSent == 1 )
	{
		printf("DEVCONFIG 0x0E (14) Device Configuration:%x.\n",buff[1] );
	}
	else
	{
		printf("error. 14.\n") ;
	} 
 

	buff[0] = 0x00 ;
	nSent = PL455_WriteReg(nDev_ID, 15, buff, 1, FRMWRT_SGL_R); 
	nSent = PL455_WaitRespFrame(buff, 120, 0);
	if( nSent == 1 )
	{
		printf("PWRCONFIG (0x0F) (15) Power Configuration:%x.\n",buff[1] );
	}
	else
	{
		printf("error. 15.\n") ;
	} 
 

	buff[0] = 0x01 ;
	nSent = PL455_WriteReg(nDev_ID, 16, buff, 1, FRMWRT_SGL_R); 
	nSent = PL455_WaitRespFrame(buff, 120, 0);
	if( nSent == 2 )
	{
		printf("COMCONFIG 0x10-11 (16-17) Communications Configuration:%x-%x.\n",buff[1], buff[2] );
	}
	else
	{
		printf("error. 16-17.\n") ;
	} 
 

	buff[0] = 0x00 ;
	nSent = PL455_WriteReg(nDev_ID, 18, buff, 1, FRMWRT_SGL_R); 
	nSent = PL455_WaitRespFrame(buff, 120, 0);
	if( nSent == 1 )
	{
		printf("TXHOLDOFF 0x12 (18) UART Transmitter Holdoff:%x.\n",buff[1] );
	} 
	else
	{
		printf("error. 18.\n") ;
	} 


	buff[0] = 0x00 ;
	nSent = PL455_WriteReg(nDev_ID, 19, buff, 1, FRMWRT_SGL_R); 
	nSent = PL455_WaitRespFrame(buff, 120, 0);
	if( nSent == 1 )
	{
		printf("CBCONFIG 0x13 (19) Balance Configuration:%x.\n",buff[1] );
	} 
	else
	{
		printf("error. 19.\n") ;
	} 


	buff[0] = 0x01 ;
	nSent = PL455_WriteReg(nDev_ID, 20, buff, 1, FRMWRT_SGL_R); 
	nSent = PL455_WaitRespFrame(buff, 120, 0);
	if( nSent == 2 )
	{
		printf("CBENBL 0x14–15 (20-21) Balancing Enable:%x-%x.\n",buff[1], buff[2] );
	} 
	else
	{
		printf("error. 20-21.\n") ;
	} 


	buff[0] = 0x01 ;
	nSent = PL455_WriteReg(nDev_ID, 30, buff, 1, FRMWRT_SGL_R); 
	nSent = PL455_WaitRespFrame(buff, 120, 0);
	if( nSent == 2)
	{
		printf("TTSTCONFIG 0x1E-1F (30-31) Test Configuration:%x-%x.\n",buff[1], buff[2] );
	}
	else
	{
		printf("error. 30-31.\n") ;
	} 
 

	buff[0] = 0x01 ;
	nSent = PL455_WriteReg(nDev_ID, 32, buff, 1, FRMWRT_SGL_R); 
	nSent = PL455_WaitRespFrame(buff, 120, 0);
	if( nSent == 2 )
	{
		printf("TESTCTRL 0x20-21 (32-33) Test Control:%x-%x.\n",buff[1], buff[2] );
	} 
	else
	{
		printf("error. 32-33.\n") ;
	} 


	buff[0] = 0x02 ;
	nSent = PL455_WriteReg(nDev_ID, 34, buff, 1, FRMWRT_SGL_R); 
	nSent = PL455_WaitRespFrame(buff, 120, 0);
	if( nSent == 3 )
	{
		printf("TEST_ADC 0x22-24 (34-36) ADC Output Test:%x-%x-%x-%x.\n",buff[1], buff[2], buff[3] );
	} 
	else
	{
		printf("error. 34-36.\n") ;
	} 

	buff[0] = 0x00 ;
	nSent = PL455_WriteReg(nDev_ID, 40, buff, 1, FRMWRT_SGL_R); 
	nSent = PL455_WaitRespFrame(buff, 120, 0);
	if( nSent == 1 )
	{
		printf("CTO 0x28 (40) Communication Timeout:%x.\n",buff[1] );
	} 
	else
	{
		printf("error. 40.\n") ;
	} 

	buff[0] = 0x02 ;
	nSent = PL455_WriteReg(nDev_ID, 41, buff, 1, FRMWRT_SGL_R); 
	nSent = PL455_WaitRespFrame(buff, 120, 0);
	if( nSent == 3 )
	{
		printf("CTO_CNT 0x29-2B (41-43) Communication Timeout Counter::%x-%x-%x-%x.\n",buff[1], buff[2], buff[3] );
	} 
	else
	{
		printf("error. 41-43.\n") ;
	} 

	buff[0] = 0x00 ;
	nSent = PL455_WriteReg(nDev_ID, 50, buff, 1, FRMWRT_SGL_R); 
	nSent = PL455_WaitRespFrame(buff, 120, 0);
	if( nSent == 1 )
	{
		printf("AM_PER 0x32 (50) Auto-Monitor Period:%x.\n",buff[1] );
	} 
	else
	{
		printf("error. 50.\n") ;
	} 


	buff[0] = 0x03 ;
	nSent = PL455_WriteReg(nDev_ID, 51, buff, 1, FRMWRT_SGL_R); 
	nSent = PL455_WaitRespFrame(buff, 120, 0);
	if( nSent == 4 )
	{
		printf("AM_CHAN 0x33-36 (51-54) Auto-Monitor Channel Select:%x-%x-%x-%x\n",buff[1], buff[2], buff[3], buff[4]);
	} 
	else
	{
		printf("error. 51-54.\n") ;
	} 

	buff[0] = 0x00 ;
	nSent = PL455_WriteReg(nDev_ID, 55, buff, 1, FRMWRT_SGL_R); 
	nSent = PL455_WaitRespFrame(buff, 120, 0);
	if( nSent == 1 )
	{
		printf("AM_OSMPL 0x37 (55) Auto-Monitor Oversampling:%x.\n",buff[1] );
	} 
	else
	{
		printf("error. 55.\n") ;
	} 


	buff[0] = 0x00 ;
	nSent = PL455_WriteReg(nDev_ID, 61, buff, 1, FRMWRT_SGL_R); 
	nSent = PL455_WaitRespFrame(buff, 120, 0);
	if( nSent == 1 )
	{
		printf("SMPL_SLY1 0x3D (61) Initial Sampling Delay:%x.\n",buff[1] );
	} 
	else
	{
		printf("error. 61.\n") ;
	} 

	buff[0] = 0x00 ;
	nSent = PL455_WriteReg(nDev_ID, 62, buff, 1, FRMWRT_SGL_R); 
	nSent = PL455_WaitRespFrame(buff, 120, 0);
	if( nSent == 1 )
	{
		printf("Cell_CSPER 0x3E (62) Cell Voltage and Internal Temperature Sampling Interval:%x.\n",buff[1] );
	} 
	else
	{
		printf("error. 62.\n") ;
	} 


	buff[0] = 0x03 ;
	nSent = PL455_WriteReg(nDev_ID, 63, buff, 1, FRMWRT_SGL_R); 
	nSent = PL455_WaitRespFrame(buff, 120, 0);
	if( nSent == 4 )
	{
		printf("AUX_SPER 0x3F-42 (63-66) AUX Sampling Period:%x.%x.%x.%x\n",buff[1], buff[2], buff[3], buff[4]);
	} 
	else
	{
		printf("error. 63-66.\n") ;
	} 


	buff[0] = 0x01 ;
	nSent = PL455_WriteReg(nDev_ID, 67, buff, 1, FRMWRT_SGL_R); 
	nSent = PL455_WaitRespFrame(buff, 120, 0);
	if( nSent == 2 )
	{
		printf("TEST_SPER 0x43-44 (67-68) Test Sampling Periods:%x.%x\n",buff[1], buff[2]);
	} 
	else
	{
		printf("error. 67-68.\n") ;
	} 

	buff[0] = 0x00 ;
	nSent = PL455_WriteReg(nDev_ID, 80, buff, 1, FRMWRT_SGL_R); 
	nSent = PL455_WaitRespFrame(buff, 120, 0);
	if( nSent == 1 )
	{
		printf("SHDN_STS 0x50 (80) Shutdown Recovery Status:%x.\n",buff[1] );
	} 
	else
	{
		printf("error. 80.\n") ;
	} 

	buff[0] = 0x00 ;
	nSent = PL455_WriteReg(nDev_ID, 81, buff, 1, FRMWRT_SGL_R); 
	nSent = PL455_WaitRespFrame(buff, 120, 0);
	if( nSent == 1 )
	{
		printf("STATUS 0x51 (81) Device Status:%x.\n",buff[1] );
	} 
	else
	{
		printf("error. 81.\n") ;
	} 

	buff[0] = 0x01 ;
	nSent = PL455_WriteReg(nDev_ID, 82, buff, 1, FRMWRT_SGL_R); 
	nSent = PL455_WaitRespFrame(buff, 120, 0);
	if( nSent == 2 )
	{
		printf("FAULT_SUM 0x52-53 (82-83) Fault Summary:%x.%x\n",buff[1],buff[2] );
	} 
	else
	{
		printf("error. 82-83.\n") ;
	} 

	buff[0] = 0x01 ;
	nSent = PL455_WriteReg(nDev_ID, 94, buff, 1, FRMWRT_SGL_R); 
	nSent = PL455_WaitRespFrame(buff, 120, 0);
	if( nSent == 2 )
	{
		printf("FAULT_COM 0x5E-5F (94-95) Communications Fault:%x.%x\n",buff[1],buff[2] );
	} 
	else
	{
		printf("error. 94-95.\n") ;
	} 

	buff[0] = 0x00 ;
	nSent = PL455_WriteReg(nDev_ID, 96, buff, 1, FRMWRT_SGL_R); 
	nSent = PL455_WaitRespFrame(buff, 120, 0);
	if( nSent == 1 )
	{
		printf("FAULT_SYS 0x60 (96) System Fault:%x.\n",buff[1] );
	} 
	else
	{
		printf("error. 96.\n") ;
	} 

	buff[0] = 0x01 ;
	nSent = PL455_WriteReg(nDev_ID, 97, buff, 1, FRMWRT_SGL_R); 
	nSent = PL455_WaitRespFrame(buff, 120, 0);
	if( nSent == 2 )
	{
		printf("FAULT_DEV 0x61-62 (97-98) Chip Fault:%x.%x\n",buff[1],buff[2] );
	}
	else
	{
		printf("error. 97-98.\n") ;
	} 
 

	buff[0] = 0x00 ;
	nSent = PL455_WriteReg(nDev_ID, 99, buff, 1, FRMWRT_SGL_R); 
	nSent = PL455_WaitRespFrame(buff, 120, 0);
	if( nSent == 1 )
	{
		printf("FAULT_GPI 0x63 (99) GPI Fault:%x.\n",buff[1] );
	}
	else
	{
		printf("error. 99.\n") ;
	} 
 

	buff[0] = 0x01 ;
	nSent = PL455_WriteReg(nDev_ID, 110, buff, 1, FRMWRT_SGL_R); 
	nSent = PL455_WaitRespFrame(buff, 120, 0);
	if( nSent == 2 )
	{
		printf("FO_CTRL 0x6E-6F (110-111) Fault Output Control:%x.%x\n",buff[1],buff[2] );
	} 
	else
	{
		printf("error. 110-111.\n") ;
	} 

	buff[0] = 0x07 ;
	nSent = PL455_WriteReg(nDev_ID, 240, buff, 1, FRMWRT_SGL_R); 
	nSent = PL455_WaitRespFrame(buff, 120, 0);
	if( nSent == 8 )
	{
		printf("LOT_NUM 0xBE-C5 (190-197) Device Lot Number: %x.%x.%x.%x.%x.%x.%x.%x\n",buff[1], buff[2], buff[3], buff[4],
			buff[5], buff[6], buff[7], buff[8] );
	} 
	else
	{
		printf("error. 190-197.\n") ;
	} 

	buff[0] = 0x01 ;
	nSent = PL455_WriteReg(nDev_ID, 198, buff, 1, FRMWRT_SGL_R); 
	nSent = PL455_WaitRespFrame(buff, 120, 0);
	if( nSent == 2 )
	{
		printf("SER_NUM 0xC6-C7 (198-199) Device Serial Number:%x.%x\n",buff[1],buff[2] );
	} 
	else
	{
		printf("error. 198-199.\n") ;
	} 

	buff[0] = 0x07 ;
	nSent = PL455_WriteReg(nDev_ID, 230, buff, 1, FRMWRT_SGL_R); 
	nSent = PL455_WaitRespFrame(buff, 120, 0);
	if( nSent == 8 )
	{
		printf("TSTR_ECC 0xE6-ED (230-237) ECC Test Result[1:0]: %x.%x.%x.%x.%x.%x.%x.%x\n",buff[1], buff[2], buff[3], buff[4],
			buff[5], buff[6], buff[7], buff[8] );
	} 
	else
	{
		printf("error. 230 - 237.\n") ;
	} 


	buff[0] = 0x03 ;
	nSent = PL455_WriteReg(nDev_ID, 240, buff, 1, FRMWRT_SGL_R); 
	nSent = PL455_WaitRespFrame(buff, 120, 0);
	if( nSent == 4 )
	{
		printf("CSUM 0xF0-F3 (240-243) Checksum: %x.%x.%x.%x\n",buff[1], buff[2], buff[3], buff[4]);
	} 
	else
	{
		printf("error. 240-243.\n") ;
	} 

	buff[0] = 0x03 ;
	nSent = PL455_WriteReg(nDev_ID, 244, buff, 1, FRMWRT_SGL_R); 
	nSent = PL455_WaitRespFrame(buff, 120, 0);
	if( nSent == 4 )
	{
		printf("CSUM_RSLT 0xF4-F7(244-247) Checksum Readout : %x.%x.%x.%x\n",buff[1], buff[2], buff[3], buff[4]);
	} 
	else
	{
		printf("error. 244-247.\n") ;
	} 


	buff[0] = 0x03 ;
	nSent = PL455_WriteReg(nDev_ID, 248, buff, 1, FRMWRT_SGL_R); 
	nSent = PL455_WaitRespFrame(buff, 120, 0);
	if( nSent == 4 )
	{
		printf("TEST_CSUM 0xF8-F9 (248-249) Checksum Test Result: %x.%x.%x.%x\n",buff[1], buff[2], buff[3], buff[4]);
	}
	else
	{
		printf("error. 248-249.\n") ;
	} 

	buff[0] = 0x00 ;
	nSent = PL455_WriteReg(nDev_ID, 250, buff, 1, FRMWRT_SGL_R); 
	nSent = PL455_WaitRespFrame(buff, 120, 0);
	if( nSent == 1 )
	{
		printf("EE_BURN 0xFA (250) EEPROM Burn Count:%x.\n",buff[1] );
	}
	else
	{
		printf("error. 250.\n") ;
	} 

}


void PL455_Set_HWVersion(unsigned int packetNo)
{
	#define GET_HW_VER_TRT_TIMES   10

	unsigned char buff[128], bFrame[8], j, nDev_ID ;
	int nSent ;

	nDev_ID = 1 ;

	for( j=0; j<GET_HW_VER_TRT_TIMES; j++ )
	{
		bFrame[0] = BMU_HW_VERSION ;
		nSent = PL455_WriteReg(nDev_ID, PL455_REG_SCRATCH, bFrame, 1, FRMWRT_SGL_NR); 

		memset(buff, 0x00, sizeof(buff));
		nSent = PL455_WriteReg(nDev_ID, PL455_REG_SCRATCH, buff, 1, FRMWRT_SGL_R); //
		usleep(5000);
		nSent = PL455_WaitRespFrame(buff, 120, 0); // 28 bytes data + packet header + CRC, 0ms timeout

		if ( (nSent == 1 ) && (buff[1] == BMU_HW_VERSION) )
		{
			break;
		}
		else
		{
			//printf(" i error.\n");
		}
	}

	nDev_ID = 1 ;
	bFrame[0] = 0x66 ; //time out shoud be longer than 1s ;
	PL455_WriteReg(nDev_ID, PL455_REG_CTO, bFrame, 1, FRMWRT_SGL_NR);
	usleep(1000);

	bFrame[0] = 0x8C ; //constant value
	bFrame[1] = 0x2D ;
	bFrame[2] = 0xB1 ;
	bFrame[3] = 0x94 ;
	PL455_WriteReg(nDev_ID, PL455_REG_MAGIC1, bFrame, 4, FRMWRT_SGL_NR);
	usleep(10000);

	bFrame[0] = 0xA3 ; //constant value
	bFrame[1] = 0x75 ;
	bFrame[2] = 0xE6 ;
	bFrame[3] = 0x0F ;
	PL455_WriteReg(nDev_ID, PL455_REG_MAGIC2, bFrame, 4, FRMWRT_SGL_NR);
	usleep(10000);

	bFrame[0] = 0x10 ;
	PL455_WriteReg(nDev_ID, PL455_REG_DEV_CTRL, bFrame, 1, FRMWRT_SGL_NR); // becareful
	usleep(1000);

  
	if( j >= GET_HW_VER_TRT_TIMES )
	{
		//PL455_Bmu_HW_type[packetNo] = 0xFFFF ;
	}
	else
	{
		//PL455_Bmu_HW_type[packetNo] = bReadBuf[0] ;
	}

}

static void PL455_CheckSum(void)
{
	unsigned char checksum[6] ={0xFE} ;
	unsigned char buff[128], nDev_ID ;
	int nSent  ;

	nDev_ID = 0 ;
	for( nDev_ID = 0 ; nDev_ID<2; nDev_ID++ )
	{
		buff[0] = 0x03 ;
		nSent = PL455_WriteReg(nDev_ID, 244, buff, 1, FRMWRT_SGL_R); //Customer checksum readout
		nSent = PL455_WaitRespFrame(buff, 100, 0);
		if( nSent == 4 )
		{
			checksum[0] = buff[1] ;
			checksum[1] = buff[2] ;
			checksum[2] = buff[3] ;
			checksum[3] = buff[4] ;

			nSent = PL455_WriteReg(nDev_ID, 240, checksum, 4, FRMWRT_SGL_NR); 
		} 
			  
	}

	/* Clear all faults */
	buff[0] = 0xFF ;
	buff[1] = 0xC0 ;
	nSent = PL455_WriteReg(nDev_ID, 82, buff, 2, FRMWRT_ALL_NR); // clear all fault summary flags
	buff[0] = 0x38 ;
	nSent = PL455_WriteReg(nDev_ID, 81, buff, 1, FRMWRT_ALL_NR); // clear fault flags in the system status register
}

int PL455_IsPowerIdle(int target_board )
{
  //check VCC to confirm the power wether is down;
  //read VCC/Vmoudle ;
	#define IDLE_ST_VOLT_TH     2000 // 1.0V
	#define VP_IDLE_VOLT_TH     4000 // 1.0V
	#define VP2_IDLE_VOLT_TH    4000 // 1.0V
	#define V10_IDLE_VOLT_TH    9500 // 9.5V
	int idle_volt[3], idle_volt_th[3] ;

	idle_volt_th[0] = VP_IDLE_VOLT_TH ;
	idle_volt_th[1] = VP2_IDLE_VOLT_TH ;
	//idle_volt_th[2] = V10_IDLE_VOLT_TH ;

	idle_volt[0] = PL455_TestBoardRef1 ;
	idle_volt[1] = PL455_TestBoardRef2 ;
	//idle_volt[2] =
	//LOG_I("PL455_TestBoardRef1=%d.PL455_TestBoardRef2=%d.",PL455_TestBoardRef1, PL455_TestBoardRef2) ;
	for( unsigned char i=0; i<2; i++ )
	{
		if( idle_volt[i] > IDLE_ST_VOLT_TH )
		{
			return -1 ;
		}
	}

	return 1 ;
}

int PL455_IsPowerNormal(int target_board )
{
  //check VCC to confirm the power wether is down;
  //read VCC/Vmoudle ;
	#define VP_WORK_VOLT_TH     5000 // 1.0V
	#define VP2_WORK_VOLT_TH    5000 // 1.0V
	#define V10_WORK_VOLT_TH    9500 // 9.5V
	int work_volt[3], work_volt_th[3] ;

	work_volt_th[0] = VP_WORK_VOLT_TH ;
	work_volt_th[1] = VP2_WORK_VOLT_TH ;
	work_volt_th[2] = V10_WORK_VOLT_TH ;

	work_volt[0] = PL455_TestBoardRef1 ;
	work_volt[1] = PL455_TestBoardRef2 ;
	//work_volt[2] =
	//LOG_I("PL455_TestBoardRef1=%d.PL455_TestBoardRef2=%d.",PL455_TestBoardRef1, PL455_TestBoardRef2) ;

	for( unsigned char i=0; i<2; i++ )
	{
		if( work_volt[i] < work_volt_th[i] )
		{
			return -1 ;
		}
	}

	return 1 ;
}

int PL455_IsOuvNormal(int target_board )
{
  return 1 ;
}

int PL455_GetInitSt(unsigned int packetNo)
{
  return PL455_InitSt[packetNo] ;
}


int PL455_GetPacketVlot(unsigned int packetNo )
{
	return  PL455_PacketVolt[packetNo] ;
}

int PL455_GetPacketCurrent(unsigned int packetNo )
{
	return  PL455_PacketCurr[packetNo] ;
}


int PL455_CheckBal(unsigned int packetNo, int type )  
{
	//cell volt is in 3200 -4150 ;

	#define CELL_VOLT_TH_UP 		4150 
	#define CELL_VOLT_TH_LOW 		3200 

	static unsigned short cell_volt_idle[MAX_PACKET_NUM][MAX_MOUDLE_NUM][MAX_CELL_NUM_PER_MOUDLE] ;
	static unsigned short cell_volt_bal[MAX_PACKET_NUM][MAX_MOUDLE_NUM][MAX_CELL_NUM_PER_MOUDLE] ;

	if (packetNo >= 2)
	{
		return -2 ;
	}

	for(unsigned char j=0; j<MAX_MOUDLE_NUM; j++)
	{
		for(unsigned char i=0; i<MAX_CELL_NUM_PER_MOUDLE; i++)
		{
			if( (PL455_CellVolt[packetNo][j][i] >= CELL_VOLT_TH_LOW) && (PL455_CellVolt[packetNo][j][i] <= CELL_VOLT_TH_UP) )
			{
				if (type == BAL_IDLE)
				{
					cell_volt_idle[packetNo][j][i] = PL455_CellVolt[packetNo][j][i] ;
				}
				else
				{
					cell_volt_bal[packetNo][j][i] = PL455_CellVolt[packetNo][j][i] ;
				}
				
			}
			else
			{
				printf("CV not in rang:P%d.M%d.C%d.V%d.\n",packetNo,j,i,PL455_CellVolt[packetNo][j][i] ) ;
				return -1 ;
			}
		}
	}

	if (type == BAL_IDLE)
	{
		return 1 ;
	}
	else if (type == BAL_ODD)
	{
		for(unsigned char j=0; j<MAX_MOUDLE_NUM; j++)
		{
			for(unsigned char i=0; i<MAX_CELL_NUM_PER_MOUDLE; i++)
			{
				#define CELL_VOLT_BAL_NORMAL_TH 	20
				int diff =  cell_volt_idle[packetNo][j][i] - cell_volt_bal[packetNo][j][i] ;

				if ( i%2 == 0) // odd channel
				{
					if (diff < CELL_VOLT_BAL_NORMAL_TH)
					{
						return -1 ;
					}
				}
			}
		}

		return 1 ;

	}
	else if (type == BAL_EVEN)
	{
		for(unsigned char j=0; j<MAX_MOUDLE_NUM; j++)
		{
			for(unsigned char i=0; i<MAX_CELL_NUM_PER_MOUDLE; i++)
			{
				#define CELL_VOLT_BAL_NORMAL_TH 	20
				int diff =  cell_volt_idle[packetNo][j][i] - cell_volt_bal[packetNo][j][i] ;

				if ( (i > 0) && ((i-1)%2 == 0) ) // even channel
				{
					if (diff < CELL_VOLT_BAL_NORMAL_TH)
					{
						return -1 ;
					}
				}
			}
		}

		return 1 ;
	}
	else
	{
		return -2 ;
	}
}
/*******************************************************************************
* NAME:             PL455_BalanceSet
* CALLED BY:        Pbc_Task
* PRECONDITIONS:
* INPUT PARAMETERS: Void
* RETURN VALUES:    Void
* DESCRIPTION:        .
*******************************************************************************/
int PL455_BalanceSet(int chs, int dura, int mNo)
{
  unsigned char bFrame[8],*pBuf ;

  pBuf = bFrame ;

  if(chs > 0xFFFF)
   {
     return -1 ; //error
   }
  bFrame[0] = (unsigned char)(chs >> 8) ;
  bFrame[1] = (unsigned char)chs ;
  PL455_WriteReg(mNo, PL455_REG_CBENBL, pBuf, 2, FRMWRT_SGL_NR); // Balance Enable

  if(dura > 5) // Balance time extend to 10 minutes
   {
     return -1 ; //error
   }
  bFrame[0] = (dura<<4) | 0xF0 ;
  PL455_WriteReg(mNo, PL455_REG_CBCONFIG, pBuf, 1, FRMWRT_SGL_NR); // Balance Time

  //printf("Pl455：Do balance.ch=%x.duration=%d.\n\n\n\n\n\n",chs,dura);

  if(  (PL455_ReInitEn[0] ==  TRUE ) || (PL455_ReInitEn[1] ==  TRUE ) )
  {
    return 0 ;
  }
  else
  {
    return 1 ;
  }
}

/*******************************************************************************
* NAME:             PL455_Sleep
* CALLED BY:
* PRECONDITIONS:
* INPUT PARAMETERS: Void
* RETURN VALUES:    Void
* DESCRIPTION:        .
*******************************************************************************/
void PL455_Sleep(unsigned int packetNo)
{
   unsigned char bFrame[8] ;

   Ioa_BmuUartChEn(packetNo) ;
   usleep(500);

   Ioa_BmuWakeupLow();
   PL455_Delay5ms();

   /* Boadcast shut down command to PL455 stack */

    bFrame[0] = 0x40 ;
    PL455_WriteReg(0, PL455_REG_DEV_CTRL, bFrame, 1, FRMWRT_ALL_NR);
	usleep(5000);

	Ioa_BmuPwrOff() ;
}

/*******************************************************************************
* NAME:             PL455_ReInit
* CALLED BY:        PL455_Init
* PRECONDITIONS:
* INPUT PARAMETERS: Void
* RETURN VALUES:    Void
* DESCRIPTION:        .
*******************************************************************************/
static void PL455_ReInit(unsigned int packetNo )
{
}


void PL455_TestBoardInit( unsigned int packetNo)
{
	Ioa_BmuUartChEn(packetNo) ;
	usleep(100);


	PL455_WakeUp(packetNo);
	PL455_SetAddress();

	PL455_ComPortInit( packetNo);

	PL455_AdcConfigure();
	
	char bCal[8]={0} ;
	PL455_Set_HWVersion(packetNo) ;

	PL455_SaveReg_TB(packetNo, bCal);
}

/*******************************************************************************
* NAME:             PL455_WakeUp
* CALLED BY:        PL455_Init

* PRECONDITIONS:
* INPUT PARAMETERS: Void
* RETURN VALUES:    Void
* DESCRIPTION:        .
*******************************************************************************/
void PL455_WakeUp(unsigned int packetNo)
{
	unsigned char bFrame[8]  ;

	Ioa_BmuWakeupLow();     // assert wake ;
	usleep(1000) ;

	/* send out broadcast pwrdown command */
	bFrame[0] = 0x40 ;
	PL455_WriteReg(0, PL455_REG_DEV_CTRL, bFrame, 1, FRMWRT_ALL_NR);

	/* WAKEUP hold time (high-pulse width) :At least 3.7ms */
	Ioa_BmuWakeupHigh();
	usleep(2000) ;

	/* send out wakeup tone through broadcast */
	bFrame[0] = 0xC0 ; // soft reset / STACK WAKE
	PL455_WriteReg(0, PL455_REG_DEV_CTRL, bFrame, 1, FRMWRT_ALL_NR);

	/* The WAKEUP pin is usually kept in the de-asserted (low) state. */
	Ioa_BmuWakeupLow();
	//usleep(5000) ;

}

/*******************************************************************************
* NAME:             PL455_CTO
* CALLED BY:        PL455_Init

* PRECONDITIONS:
* INPUT PARAMETERS: Void
* RETURN VALUES:    Void
* DESCRIPTION:        .
*******************************************************************************/
static void PL455_CTO(void)
{
  unsigned char Frame  ;

    //Frame = 0x76 ; // power down is 30s；set fault is 10s
  	Frame = 0x76 ;
    PL455_WriteReg(0, PL455_REG_CTO, &Frame, 1, FRMWRT_ALL_NR);
    
    // Send broadcast request to all boards to sample and store results (section 4.2.1)
    Frame = 0x00 ;
    PL455_WriteReg(0, 2, &Frame, 1, FRMWRT_ALL_NR); // send sync sample command
}


/*******************************************************************************
* NAME:             PL455_GetCorrectPar
* CALLED BY:        PL455_Init

* PRECONDITIONS:
* INPUT PARAMETERS: Void
* RETURN VALUES:    Void
* DESCRIPTION:        .
*******************************************************************************/
int PL455_GetCorrectPar(unsigned int packetNo )
{
  unsigned char bReadBuf[32],j, nDev_ID ;
  unsigned char buff[128] ;
  int nSent ;

	char name[128] ;
  	if( packetNo == ATE_STD_BOARD_IDX)
	{
		strcpy( name,"Stand BMU Board") ;
	}
	else if( packetNo == ATE_TEST_BOARD_IDX)
	{
		strcpy( name,"Test BMU Board") ;
	}
	else
	{
		strcpy( name,"BMU Board No errror") ;
	}


	for( j=0; j<4; j++ )
	{
		unsigned char i ;

    //for(	nDev_ID = 0 ; nDev_ID <2 ;nDev_ID++ )
    	nDev_ID = 0 ;
    	{
    		for( i=0;i<8;i++)
	  		{
				memset(buff, 0x00, sizeof(buff));
				//nSent = PL455_WriteReg(nDev_ID, PL455_REG_SCRATCH+i, buff, 1, FRMWRT_SGL_R); //

				nSent = PL455_WriteReg(nDev_ID, PL455_REG_SCRATCH+i, buff, 1, FRMWRT_SGL_R); //
		        nSent = PL455_WaitRespFrame(buff, 120, 0); // 28 bytes data + packet header + CRC, 0ms timeout

		        if( nSent == 1 )
		        {
		        	bReadBuf[i] = buff[1] ;
		        	//printf(" %x ",buff[1]);
		        }
		        else
		        {
		        	//printf(" i error.\n");
		        	break;
		        }
			}

   			//printf(" \n");
  		}

		if( (i == 8) ) // get correnct parameter success!
		{
			PL455_Cali[packetNo].PCGainOfChg    = ((int)bReadBuf[0] <<8) | bReadBuf[1] ;
			PL455_Cali[packetNo].PCGainOfDischg = ((int)bReadBuf[2] <<8) | bReadBuf[3] ;
			PL455_Cali[packetNo].PCOffsetOfChg  = bReadBuf[4] ;
			PL455_Cali[packetNo].PCOffsetOfDischg  = bReadBuf[5] ;

			//PL455_Cali[packetNo].PVGain   = ((int)bReadBuf[8] <<8) | bReadBuf[9] ;
			PL455_Cali[packetNo].PVGain = 900+bReadBuf[6] ;
			PL455_Cali[packetNo].PVOffset = bReadBuf[7] ;

			if( ( (PL455_Cali[packetNo].PCGainOfChg < 1200) && ( PL455_Cali[packetNo].PCGainOfChg > 800) ) &&
				( (PL455_Cali[packetNo].PCGainOfDischg < 1200) && ( PL455_Cali[packetNo].PCGainOfDischg > 800) ) &&
				( (PL455_Cali[packetNo].PCOffsetOfChg < 120) && ( PL455_Cali[packetNo].PCOffsetOfChg > -120) ) &&
				( (PL455_Cali[packetNo].PCOffsetOfDischg < 120) && ( PL455_Cali[packetNo].PCOffsetOfDischg > -120) ) &&
				( (PL455_Cali[packetNo].PVGain < 1200) && ( PL455_Cali[packetNo].PVGain > 800) ) &&
				( (PL455_Cali[packetNo].PVOffset < 120) && ( PL455_Cali[packetNo].PVOffset > -120) )   )
			{
				#if( LOG_DEBUG_ENABLE == 1 )
				{
                	//LOG_E("<PL455> - Get P%d Correct parameter - Normal idx =%d.",packetNo, i);
				}
				#endif

				break;
			}
            else
            {
                #if( LOG_DEBUG_ENABLE == 1 )
                {
                	//LOG_E("<PL455> - Get P%d Correct parameter - Normal idx =%d. data error!",packetNo, i);
                }
                #endif

            }
		}
        else
        {
            #if( LOG_DEBUG_ENABLE == 1 )
            {
            	//LOG_E("<PL455> - Get P%d Correct parameter - idx error =%d.!",packetNo, i);
            }
            #endif
        }
	}


	if( j >= 4 )
	{
		LOG_E("<PL455> -[%s] Correct parameter faild!",name);

		PL455_Cali[packetNo].PCGainOfChg		= 1000 ;
		PL455_Cali[packetNo].PCGainOfDischg		= 1000 ;
		PL455_Cali[packetNo].PCOffsetOfChg		= 0 ;
		PL455_Cali[packetNo].PCOffsetOfDischg	= 0 ;
		PL455_Cali[packetNo].PVGain				= 1000 ;
		PL455_Cali[packetNo].PVOffset			= 0 ;

		return -1 ;
	}
    else
    {
        #if( LOG_DEBUG_ENABLE == 1 )
        {
			LOG_E("<PL455> -[%s]Correct parameter:PC(Chg/Dischg) %d,%d./%d,%d. ",
                    name, PL455_Cali[packetNo].PCGainOfChg, PL455_Cali[packetNo].PCOffsetOfChg,
                    PL455_Cali[packetNo].PCGainOfDischg, PL455_Cali[packetNo].PCOffsetOfDischg );

			LOG_E("<PL455> -[%s] Correct parameter:PV %d,%d.",
                    name, PL455_Cali[packetNo].PVGain, PL455_Cali[packetNo].PVOffset);


			LOG_I("<PL455> -[%s] Correct parameter success!",name);
		}
		#endif

		return 1 ;
    }
#if 0
    	PL455_Cali[packetNo].PCGainOfChg		= 1000 ;
		PL455_Cali[packetNo].PCGainOfDischg		= 1000 ;
		PL455_Cali[packetNo].PCOffsetOfChg		= 0 ;
		PL455_Cali[packetNo].PCOffsetOfDischg	= 0 ;
		PL455_Cali[packetNo].PVGain				= 1000 ;
		PL455_Cali[packetNo].PVOffset			= 0 ;
#endif
return 1 ;

}


int PL455_ResetCorrectPar(unsigned int packetNo )
{
	PL455_Cali[packetNo].PCGainOfChg		= 1000 ;
	PL455_Cali[packetNo].PCGainOfDischg		= 1000 ;
	PL455_Cali[packetNo].PCOffsetOfChg		= 0 ;
	PL455_Cali[packetNo].PCOffsetOfDischg	= 0 ;
	PL455_Cali[packetNo].PVGain				= 1000 ;
	PL455_Cali[packetNo].PVOffset			= 0 ;

	return 0x7F00 ;
}

int PL455_LoadCorrectPar(unsigned int packetNo, char *buff )
{
	*buff 	  = (char)(PL455_Cali[packetNo].PCGainOfChg >> 8) ;
	*(buff+1) = (char)(PL455_Cali[packetNo].PCGainOfChg) ;

	*(buff+2) = (char)(PL455_Cali[packetNo].PCGainOfDischg >> 8) ;
	*(buff+3) = (char)(PL455_Cali[packetNo].PCGainOfDischg) ;

	*(buff+4) = PL455_Cali[packetNo].PCOffsetOfChg ;
	*(buff+5) = PL455_Cali[packetNo].PCOffsetOfDischg ;

	*(buff+6) = PL455_Cali[packetNo].PVGain - 900 ;
	*(buff+7) = PL455_Cali[packetNo].PVOffset ;

	return 1 ;
}


void PL455_SaveReg_TB(unsigned int packetNo, char *buff)
{
	unsigned char bFrame[10] ;
	int nDev_ID ;

	for(unsigned char i=0; i<8; i++ )
	{
		bFrame[i] = buff[i] ;
	}

	nDev_ID = 0 ;
	PL455_WriteReg(nDev_ID, PL455_REG_SCRATCH, bFrame, 4, FRMWRT_SGL_NR);
	usleep(10000);

	PL455_WriteReg(nDev_ID, PL455_REG_SCRATCH+4, bFrame+4, 4, FRMWRT_SGL_NR);
	usleep(10000);

	bFrame[0] = 0 ;
	bFrame[1] = 0 ;
	bFrame[2] = 0 ;
	bFrame[3] = 0 ;
	bFrame[4] = 0 ;
	bFrame[5] = 0 ;
	bFrame[6] = 0 ;
	bFrame[7] = 0 ;

	nDev_ID = 1 ;
	PL455_WriteReg(nDev_ID, PL455_REG_SCRATCH, bFrame, 4, FRMWRT_SGL_NR);
	usleep(10000);

	PL455_WriteReg(nDev_ID, PL455_REG_SCRATCH+4, bFrame+4, 4, FRMWRT_SGL_NR);
	usleep(10000);

	//bFrame[0] = 0x02 ;
	//bFrame[1] = 0x59 ;
	//PL455_WriteReg(nDev_ID, PL455_REG_AX0OFFSET,  bFrame, 4, FRMWRT_SGL_NR);  //PC offset

	nDev_ID = 0 ;
	bFrame[0] = 0x66 ; //time out shoud be longer than 1s ;
	PL455_WriteReg(nDev_ID, PL455_REG_CTO, bFrame, 1, FRMWRT_ALL_NR);
	usleep(1000);

	bFrame[0] = 0x8C ; //constant value
	bFrame[1] = 0x2D ;
	bFrame[2] = 0xB1 ;
	bFrame[3] = 0x94 ;
	PL455_WriteReg(nDev_ID, PL455_REG_MAGIC1, bFrame, 4, FRMWRT_ALL_NR);
	usleep(10000);

	bFrame[0] = 0xA3 ; //constant value
	bFrame[1] = 0x75 ;
	bFrame[2] = 0xE6 ;
	bFrame[3] = 0x0F ;
	PL455_WriteReg(nDev_ID, PL455_REG_MAGIC2, bFrame, 4, FRMWRT_ALL_NR);
	usleep(10000);

	bFrame[0] = 0x10 ;
	PL455_WriteReg(nDev_ID, PL455_REG_DEV_CTRL, bFrame, 1, FRMWRT_ALL_NR); // becareful
	usleep(1000);
}
/*******************************************************************************
* NAME:             PL455_SaveReg
* CALLED BY:        PL455_Init

* PRECONDITIONS:
* INPUT PARAMETERS: Void
* RETURN VALUES:    Void
* DESCRIPTION:        .
*******************************************************************************/
void PL455_SaveReg(unsigned int packetNo, char *buff)
{
	unsigned char bFrame[10] ;
	int nDev_ID ;

	for(unsigned char i=0; i<8; i++ )
	{
		bFrame[i] = buff[i] ;
	}

	nDev_ID = 0 ;
	PL455_WriteReg(nDev_ID, PL455_REG_SCRATCH, bFrame, 4, FRMWRT_SGL_NR);
	usleep(10000);

	PL455_WriteReg(nDev_ID, PL455_REG_SCRATCH+4, bFrame+4, 4, FRMWRT_SGL_NR);
	usleep(10000);

	bFrame[0] = 0 ;
	bFrame[1] = 0 ;
	bFrame[2] = 0 ;
	bFrame[3] = 0 ;
	bFrame[4] = 0 ;
	bFrame[5] = 0 ;
	bFrame[6] = 0 ;
	bFrame[7] = 0 ;

	nDev_ID = 1 ;
	PL455_WriteReg(nDev_ID, PL455_REG_SCRATCH, bFrame, 4, FRMWRT_SGL_NR);
	usleep(10000);

	PL455_WriteReg(nDev_ID, PL455_REG_SCRATCH+4, bFrame+4, 4, FRMWRT_SGL_NR);
	usleep(10000);

	//bFrame[0] = 0x02 ;
	//bFrame[1] = 0x59 ;
	//PL455_WriteReg(nDev_ID, PL455_REG_AX0OFFSET,  bFrame, 4, FRMWRT_SGL_NR);  //PC offset

	nDev_ID = 0 ;
	bFrame[0] = 0x66 ; //time out shoud be longer than 1s ;
	PL455_WriteReg(nDev_ID, PL455_REG_CTO, bFrame, 1, FRMWRT_ALL_NR);
	usleep(1000);

	bFrame[0] = 0x8C ; //constant value
	bFrame[1] = 0x2D ;
	bFrame[2] = 0xB1 ;
	bFrame[3] = 0x94 ;
	PL455_WriteReg(nDev_ID, PL455_REG_MAGIC1, bFrame, 4, FRMWRT_ALL_NR);
	usleep(10000);

	bFrame[0] = 0xA3 ; //constant value
	bFrame[1] = 0x75 ;
	bFrame[2] = 0xE6 ;
	bFrame[3] = 0x0F ;
	PL455_WriteReg(nDev_ID, PL455_REG_MAGIC2, bFrame, 4, FRMWRT_ALL_NR);
	usleep(10000);

	bFrame[0] = 0x10 ;
	PL455_WriteReg(nDev_ID, PL455_REG_DEV_CTRL, bFrame, 1, FRMWRT_SGL_NR); // becareful
	usleep(1000);
}
/*******************************************************************************
* NAME:             PL455_GetCorrectPar
* CALLED BY:        PL455_Init

* PRECONDITIONS:
* INPUT PARAMETERS: Void
* RETURN VALUES:    Void
* DESCRIPTION:        .
*******************************************************************************/
static void PL455_GetBmuConfig(unsigned int packetNo)
{
  unsigned char j,checkErrSt ;
  unsigned char bReadBuf[32], buff[128] ;
  int nSent ;

	checkErrSt = 0 ;
    for( j=0; j<10; j++ )
	{
		for( unsigned char nDev_ID=0; nDev_ID<2; nDev_ID++ )
	  	{
	    	unsigned char i ;
	    	for( i=0;i<2;i++)
	    	{
				memset(buff, 0x00, sizeof(buff));
				nSent = PL455_WriteReg( nDev_ID, PL455_REG_SN+i, buff, 1, FRMWRT_SGL_R ); // send read sampled values command
				usleep(10000);
				nSent = PL455_WaitRespFrame(buff, 120, 0); // 28 bytes data + packet header + CRC, 0ms timeout

		        if( nSent == 1 )
		        {
		        	bReadBuf[i] = buff[1] ;
		        }
		        else
		        {
		        	//error
		        	checkErrSt = 1 ;
		        	break;
		        }
	        }

			if( i == 2 ) // get correnct parameter success!
			{
				Bms_ConfigPar.BmuDeviceSN[packetNo][nDev_ID] = ((int)bReadBuf[0] <<8) | bReadBuf[1] ;
			}

			for( i=0; i<8; i++ )
			{
				memset(buff, 0x00, sizeof(buff));
				nSent = PL455_WriteReg( nDev_ID, PL455_REG_LOT+i, buff, 1, FRMWRT_SGL_R ); // send read sampled values command
				usleep(10000);
				nSent = PL455_WaitRespFrame(buff, 120, 0); // 28 bytes data + packet header + CRC, 0ms timeout

		        if( nSent == 1 )
		        {
		        	Bms_ConfigPar.BmuChipLot[packetNo][nDev_ID][i] = buff[1] ;
		        }
		        else
		        {
		        	//error
		        	checkErrSt = 1 ;
		        	break;
		        }
			}

			if(checkErrSt == 1 )
			{
				break ;
			}
		}

		if(checkErrSt == 0 )
		{
			//ok
			break ;
		}

	}

	if( j >= 10 )
	{
		LOG_E("<PL455> - Get BMU  parameter faild!");
	}

}



/*******************************************************************************
* NAME:             PL455_SetAddress
* CALLED BY:        PL455_Init
* PRECONDITIONS:
* INPUT PARAMETERS: Void
* RETURN VALUES:    Void
* DESCRIPTION:        .
*******************************************************************************/
static int PL455_SetAddress(void)
{
    int nSent ,nDev_ID ;
    unsigned char bFrame[8],*pBuf ;

    pBuf = bFrame ;

 /* a wakeup tone to be sent from the high-side communication interface to the next device up the stack. */
    bFrame[0] = 0x20 ;
    nSent = PL455_WriteReg(0, PL455_REG_DEV_CTRL, pBuf, 1, FRMWRT_ALL_NR);

 /* Clear all faults */
    bFrame[0] = 0xFF ;
    bFrame[1] = 0xC0 ;
    nSent = PL455_WriteReg(0, 82, pBuf, 2, FRMWRT_ALL_NR); // clear all fault summary flags
    bFrame[0] = 0x38 ;
    nSent = PL455_WriteReg(0, 81, pBuf, 1, FRMWRT_ALL_NR); // clear fault flags in the system status register

  /* Fully Enable Differential Interfaces and Select Auto-Addressing Mode */
    bFrame[0] = PL455_BAUDRATE_REG ; // 250K Baudrate,Enable the high-side differential receiver
    bFrame[1] = 0xE0 ; // Enable the low-side differential transmitter
    nSent = PL455_WriteReg(0, 16, pBuf, 2, FRMWRT_ALL_NR);

  /* Put Devices into Auto-Address Learning Mode*/
    bFrame[0] = 0x19 ; //Configure the bq76PL455 device to use auto-addressing to select address
    nSent = PL455_WriteReg(0, 14, pBuf, 1, FRMWRT_ALL_NR);

    bFrame[0] = 0x08 ; //Configure the bq76PL455 device to enter auto-address mode
    nSent = PL455_WriteReg(0, 12, pBuf, 1, FRMWRT_ALL_NR);

 /* The next write to this ID will be its address */
 /* Set New Addresses for Daisy-Chained Devices*/
    for (nDev_ID = 0; nDev_ID < 2; nDev_ID++)
    {
      bFrame[0] = nDev_ID ;
      nSent = PL455_WriteReg(nDev_ID, PL455_REG_ADDR, pBuf, 1, FRMWRT_ALL_NR); // send address to each board
    }

   return nSent ;
}

/*******************************************************************************
* NAME:             PL455_ComPortInit
* CALLED BY:        PL455_Init
* PRECONDITIONS:
* INPUT PARAMETERS: Void
* RETURN VALUES:    Void
* DESCRIPTION:        .
*******************************************************************************/
static int PL455_ComPortInit(unsigned int packetNo)
{
	int nSent,nRead,ErrSt ;
	char name[128] ;
	unsigned char bFrame[8],bReadBuf[128], *pBuf,nDev_ID,nTopFound;

	if( packetNo == ATE_STD_BOARD_IDX)
	{
		strcpy( name,"Stand BMU Board") ;
	}
	else if( packetNo == ATE_TEST_BOARD_IDX)
	{
		strcpy( name,"Test BMU Board") ;
	}
	else
	{
		strcpy( name,"BMU Board No errror") ;
	}

	PL455_WaitRespFrame(bReadBuf, 32, 0); // clear the buff
	usleep(100);

	pBuf = bFrame ;
	memset(bReadBuf, 0xFF, sizeof(bReadBuf));

	bFrame[0] = 0 ;
	nSent = PL455_WriteReg(0, 10, pBuf, 1, FRMWRT_SGL_R);
	nRead = PL455_WaitRespFrame(bReadBuf, 32, 0); // 4 bytes data

	ErrSt = 0 ;

	if( (nRead != 1) || (bReadBuf[1] != 0) || (bReadBuf[0] == 0xFF) )
	{
		LOG_E("<PL455> - [%s] Chip No0 is not exist.",name);
		PL455_InitSt[packetNo]   = PL455_INIT_ST_ERR ; //Error
		PL455_ReInitEn[packetNo] = TRUE ;
		ErrSt = 1 ;
	}
	#if( PL455_INIT_INFO_DEDUG == 1 )
	else
	{
		LOG_I("<PL455> - [%s] Chip No0 = %d.",name, bReadBuf[1]+1);
	}
	#endif

      //  LOG_I("<PL455> - com port init -nRead %d,bReadBuf %d, %d.",nRead, bReadBuf[0],bReadBuf[1]) ;


	Uart2_ClearReadBuf();
	nSent = PL455_WriteReg(1, 10, pBuf, 1, FRMWRT_SGL_R);

	nRead = PL455_WaitRespFrame(bReadBuf, 32, 0);

  if( (nRead != 1) || (bReadBuf[1] != 1) )
   {
		LOG_E("<PL455> - [%s] Chip No1 is not exist.",name);
		PL455_InitSt[packetNo]   = PL455_INIT_ST_ERR ; // Erorr
		PL455_ReInitEn[packetNo] =  TRUE ;
		ErrSt = 1 ;
   }
#if( PL455_INIT_INFO_DEDUG == 1 )
  else
   {
     LOG_I("<PL455> - [%s] Chip No1 = %d .",name,   bReadBuf[1]+1);
   }
#endif

    //LOG_I("<PL455> - com port init -nRead %d,bReadBuf %d, %d.",nRead, bReadBuf[0],bReadBuf[1]) ;

	if( ErrSt == 0 )
	{
		PL455_InitSt[packetNo] = PL455_INIT_ST_OK ; // OK
	}

	nTopFound = 0 ;
	/* Initialize comm port settings for all found boards */
	bFrame[0] = PL455_BAUDRATE_REG ;
	bFrame[1] = 0x20 ; // 0x28 -> 0x20
	nSent = PL455_WriteReg(1, 16, pBuf, 2, FRMWRT_SGL_NR);

	bFrame[0] = PL455_BAUDRATE_REG ;
	bFrame[1] = 0xC0 ; // 0xDO -> 0xC0
	nSent = PL455_WriteReg(0, 16, pBuf, 2, FRMWRT_SGL_NR);


// 2016-4-2
 /* Clear all faults */
	// Starting at the top of the stack of devices, clear all existing faults. 
	// It is important to start clearing faults from the top of the stack to prevent faults from "higher"
	// units from re-enabling faults as they propagate down the stack
	bFrame[0] = 0xFF ;
	bFrame[1] = 0xC0 ;
	nSent = PL455_WriteReg(0, 82, pBuf, 2, FRMWRT_ALL_NR); // clear all fault summary flags

	bFrame[0] = 0x38 ;
	nSent = PL455_WriteReg(0, 81, pBuf, 1, FRMWRT_ALL_NR); // clear fault flags in the system status register

	return nSent ;
}

/*******************************************************************************
* NAME:             PL455_AdcConfigure
* CALLED BY:        PL455_Init
* PRECONDITIONS:
* INPUT PARAMETERS: Void
* RETURN VALUES:    Void
* DESCRIPTION:        .
*******************************************************************************/
static int PL455_AdcConfigure(void)
{
    int nSent ;
    unsigned char nDev_ID,bFrame[8], *pBuf ;

    pBuf = bFrame ;
    nDev_ID = 0 ;
  /* initial delay  */
    bFrame[0] = 0x00 ;
    nSent = PL455_WriteReg(nDev_ID, 61, pBuf, 1, FRMWRT_ALL_NR); // set 0 initial delay

  /* Configure celll voltage and internal temp sample period */
    bFrame[0] = 0xBC;
    nSent = PL455_WriteReg(nDev_ID, 62, pBuf, 1, FRMWRT_ALL_NR); // set 99.92us ADC sampling period

  /* Configure AUX sample period */
    bFrame[0] = 0x44;  // 0x44 --> 0x88 --> 0x00
    bFrame[1] = 0x44;
    bFrame[2] = 0x44;
    bFrame[3] = 0x44;
    nSent = PL455_WriteReg(nDev_ID, 63, pBuf, 4, FRMWRT_ALL_NR); // set 99.92us ADC sampling period

  /* Configure the oversampling rate */
    //bFrame[0] = 0x00 ;
    bFrame[0] = 0x7A ; // 0x7A
    nSent = PL455_WriteReg(nDev_ID, 7, pBuf, 1, FRMWRT_ALL_NR); // set no oversampling period

  /* Clear all faults */
    bFrame[0] = 0xFF ;
    bFrame[1] = 0xC0 ;
    nSent = PL455_WriteReg(nDev_ID, 82, pBuf, 2, FRMWRT_ALL_NR); // clear all fault summary flags

    bFrame[0] = 0x38 ;
    nSent = PL455_WriteReg(nDev_ID, 81, pBuf, 1, FRMWRT_ALL_NR); // clear fault flags in the system status register

  /* Select number of cells and channels to sample */
    bFrame[0] =  MAX_CELL_NUM_PER_MOUDLE ; // number of cells :16
    nSent = PL455_WriteReg(nDev_ID, 13, pBuf, 1, FRMWRT_ALL_NR);

  /* Assigned the cell No and AUX No be used */
    bFrame[0] = 0xFF ; //cells
    bFrame[1] = 0xFF ;
    bFrame[2] = 0x3F ; //AUX4 is for current measurement,AUX30-0 is for temperature ;
    bFrame[3] = 0x00 ;
    nSent = PL455_WriteReg(nDev_ID, 3, pBuf, 4, FRMWRT_ALL_NR); //0xFFFF03C0

    bFrame[0] = 0x00 ; //page 76
    bFrame[1] = 0x00   ;
    nSent = PL455_WriteReg(nDev_ID, PL455_REG_TSTCONFIG, pBuf, 2, FRMWRT_ALL_NR);
	
	return nSent;
}


/*******************************************************************************
* NAME:             PL455_WriteReg
* CALLED BY:
* PRECONDITIONS:
* INPUT PARAMETERS: Void
* RETURN VALUES:    Void
* DESCRIPTION:        .
*******************************************************************************/
static int PL455_WriteReg(unsigned char deviceID, unsigned short regAddr, unsigned char* bData, unsigned char bLen, unsigned char ReqType)
{
	int bRes = 0;
	unsigned char bBuf[32],i;

	if( (bLen == 0) || (bLen > 8) )
	{
		return -1 ;
	}

	for(i=0;i<bLen;i++)
	{
		bBuf[i] = *(bData++) ;
	}

  bRes = PL455_WriteFrame(deviceID, regAddr, bBuf, bLen, ReqType);
  //PL455_Delay20ms();

  return bRes;
}


/*******************************************************************************
* NAME:             PL455_WriteFrame
* CALLED BY:
* PRECONDITIONS:
* INPUT PARAMETERS: Void
* RETURN VALUES:    Void
* DESCRIPTION:        .
*******************************************************************************/
static int PL455_WriteFrame(unsigned char deviceID, unsigned short regAddr, unsigned char * pData, unsigned char bLen, unsigned char ReqType)
{
  int  bPktLen = 0;
  unsigned char pFrame[128];
  unsigned char *pBuf = pFrame;
  unsigned short wCRC;

	if (bLen == 7 || bLen > 8) // Data size of 7 bytes is not supported.
   	{
		#if( PL455_MSG_FRAME_DEBUG ==  1)
			LOG_E("WriteFrame Error:bLen rang errror. ");
		#endif
     return -1 ;
   }

   memset(pFrame, 0x7F, sizeof(pFrame));

   if (regAddr < 255)
   {
      /* Frame Initialization */
      *pBuf++ = 0x80 | ReqType | bLen;	// use 8-bit address

      if ( (ReqType == FRMWRT_SGL_R) || (ReqType == FRMWRT_SGL_NR) || \
           (ReqType == FRMWRT_GRP_R) || (ReqType == FRMWRT_GRP_NR) )
    	{
    	 *pBuf++ = (deviceID & 0x00FF);
    	}
      /* Register Address 8(s) */
    	 *pBuf++ = regAddr & 0x00FF;
   }
   else // use 16-bit address
   {
       /* Frame Initialization */
    	 *pBuf++ = 0x88 | ReqType | bLen;	// Command Frame,16-bit Register Address,Data Size

       /* Device Address or Group ID */
      if ( (ReqType == FRMWRT_SGL_R) || (ReqType == FRMWRT_SGL_NR) || \
           (ReqType == FRMWRT_GRP_R) || (ReqType == FRMWRT_GRP_NR) )
    	 {
    	   *pBuf++ = (deviceID & 0x00FF);
    	 }

        // Register Address unsigned char(s)
    	 *pBuf++ = (regAddr & 0xFF00) >> 8;
    	 *pBuf++ =  regAddr & 0x00FF;
   }

  	while(bLen--)
  	{
  	  *pBuf++ = *pData++;
  	}

  	bPktLen = pBuf - pFrame;
  	wCRC = CRC16((unsigned char*)pFrame, bPktLen);

  	*pBuf++ = wCRC & 0x00FF;
  	*pBuf++ = (wCRC & 0xFF00) >> 8;

  	bPktLen += 2;

#ifdef PL455_TX_BUFF_DEBUG
    printf("uart Tx data is \n");
    for(char i=0;i<bPktLen;i++)
    {
      LOG_I(" %2x ",pFrame[i]);
    }
    LOG_I("\n");
#endif
    bPktLen = Uart2_Write((char *)pFrame, (unsigned int)bPktLen);
    usleep(1000);

  	return bPktLen;
}


/*******************************************************************************
* NAME:             PL455_WaitRespFrame
* CALLED BY:
* PRECONDITIONS:
* INPUT PARAMETERS: Void
* RETURN VALUES:    Void
* DESCRIPTION:        .
*******************************************************************************/
static int PL455_WaitRespFrame(unsigned char *pFrame, unsigned char bLen, unsigned int dwTimeOut)
{
  unsigned short wCRC = 0, wCRC16;
  unsigned char bBuf[512];
  char bRxDataLen;
  int RxFlg ;

  bRxDataLen = 0 ;
  usleep(100);
  memset(bBuf, 0xFF, sizeof(bBuf));

  RxFlg = Uart2_Read((char*)bBuf, bLen) ;//debug

  if( (RxFlg < 0) || (bBuf[0] == 0xFF) )
   {

#if( PL455_MSG_FRAME_DEBUG ==  1)
     LOG_E("ERROR! PL455 No Responce! RxFlg =%d,bBuf[0] =%d.",RxFlg,bBuf[0]);
#endif
     return -1 ; // error.no responce .
   }
  bRxDataLen = bBuf[0];

  wCRC = bBuf[bRxDataLen+2];
  wCRC |= ((unsigned short)bBuf[bRxDataLen+3] << 8);
  wCRC16 = CRC16((unsigned char*)bBuf, bRxDataLen+2);

#ifdef  PL455_RX_BUFF_DEBUG
    LOG_I("WaitRespFrame: Rx Data length  =%d; ", bRxDataLen );
    LOG_I("uart Rx data is ");
      for(char i=0;i < (bBuf[0]+4);i++)
      {
         LOG_I("%x  ",(unsigned char)bBuf[i]);
      }
      LOG_I("\n");
#endif

  if (wCRC != wCRC16)
   {
#ifdef APOLLO_PL455_DEBUG
     LOG_E("WaitRespFrame: uart Rx data CRC error!");
     LOG_E("WaitRespFrame: The Rx CRC is:%x .",wCRC);
     LOG_E("WaitRespFrame: The expect CRC is:%x .",wCRC16);
     memcpy(pFrame, bBuf, bRxDataLen + 4);
     return -1;
#endif
   }

  memcpy(pFrame, bBuf, bRxDataLen + 4);
  return (bRxDataLen + 1);
}


/*******************************************************************************
* NAME:             PL455_BinSearch
* CALLED BY:        PL455_CheckCellTemp
* PRECONDITIONS:
* INPUT PARAMETERS: Void
* RETURN VALUES:    Void
* DESCRIPTION:        .
*******************************************************************************/
static int PL455_BinSearch(int *array, int n, int target)
{
  int low ,high ,mid;

  low  = 0;
  high = n-1 ;

  while(low <= high)
  {
    if(array[low]  <= target )
      return low;
    if(array[high] >= target )
      return high;

    mid = low+((high-low)/2);

    if(array[mid] == target )
     return mid;
    if(array[mid] > target )
      low=mid;
     else
      high=mid;

    if((high -low)<= 1)
     {
       return low ;
     }
  }

  return -1; //error,Low>high or not find;
}


/*******************************************************************************
* NAME:             PL455_CheckCellVolt
* CALLED BY:        PL455_MainTask
* PRECONDITIONS:
* INPUT PARAMETERS: Void
* RETURN VALUES:    Void
* DESCRIPTION:     Cell voltage debonce   .
*******************************************************************************/
static void PL455_CheckCellVolt(unsigned char *pBuf, unsigned char nDev_ID, unsigned int packetNo)
{
  unsigned char i,j ;
  int tmp ;

  /* Check Cell Volt */
   j = MAX_CELL_NUM_PER_MOUDLE ;

   for( i=0; i<MAX_CELL_NUM_PER_MOUDLE; i++ )
    {
	 	 j-- ;
		 tmp = *(pBuf+i*2) ;
		 tmp = (tmp<<8) | *(pBuf+(i*2+1)) ;

		if( tmp != 0xFFFF )
		 {
			  if( PL455_CellVoltNormalCnt[packetNo][nDev_ID][j] >= PL455_CV_NORMAL_DOBONCE_TIMES  )
				{
				  PL455_IsCellVoltNormal[packetNo][nDev_ID][j] = 1 ;
				}
			  else
				{
				  PL455_CellVoltNormalCnt[packetNo][nDev_ID][j]++ ;
				}

			  PL455_CellVoltErrCnt[packetNo][nDev_ID][j] = 0 ;

			  if( PL455_IsCellVoltNormal[packetNo][nDev_ID][j] == 1 )
				{
          			tmp = tmp*PL455_VREF_MV*2/0xFFFF + Bmu_Config_C.CellVoltOffset[nDev_ID] ;

					if( PL455_CellVolt[packetNo][nDev_ID][j] != 0xFFFF )
					{
					//  PL455_CellVolt[nDev_ID][j] = (tmp+PL455_CellVolt[nDev_ID][j]*15)>>4 ;
					  PL455_CellVolt[packetNo][nDev_ID][j] = tmp ;
					}
					else
					{
					  PL455_CellVolt[packetNo][nDev_ID][j] = tmp ;
					}
				}
		  }
		else
		 {
			  if( PL455_CellVoltErrCnt[packetNo][nDev_ID][j] >= PL455_CV_ERROR_DOBONCE_TIMES )
				{
				PL455_CellVolt[packetNo][nDev_ID][j] = 0xFFFF ;
				PL455_IsCellVoltNormal[packetNo][nDev_ID][j] = 0 ;
				}
			  else
				{
  				PL455_CellVoltErrCnt[packetNo][nDev_ID][j]++ ;
				}
			  PL455_CellVoltNormalCnt[packetNo][nDev_ID][j] = 0 ;
		 }
    }
}

int PL455_CheckCellVolt_normal_st( void )
{
	#define CELL_VOLT_DIFF_TH        300

	for(unsigned char nDev_ID=0; nDev_ID <2; nDev_ID++ )
	{
		for(unsigned char i=0 ; i <MAX_CELL_NUM_PER_MOUDLE ; i++ )
		{
			int diff = PL455_CellVolt[ATE_TEST_BOARD_IDX][nDev_ID][i] - PL455_CellVolt[ATE_STD_BOARD_IDX][nDev_ID][i] ;
			
			if( diff < 0)
			{
				diff = -diff ;
			}

			if( diff > CELL_VOLT_DIFF_TH )
			{
				printf(" cell No:m%d c%d. P0%d-P1%d.\n", nDev_ID, i,
					PL455_CellVolt[ATE_TEST_BOARD_IDX][nDev_ID][i], PL455_CellVolt[ATE_STD_BOARD_IDX][nDev_ID][i]) ;
				return -1 ;
			}
			else
			{
				// do nothing
			}
		}
	}

	return 1 ;
}

int PL455_CheckCellTemp_normal_st( void )
{
	#define CELL_TEMP_DIFF_TH        30 //3.0C

	for(unsigned char nDev_ID=0; nDev_ID <2; nDev_ID++ )
	{
		for(unsigned char i=0 ; i <MAX_TEMP_NUM_PER_MOUDLE ; i++ )
		{
			int diff = PL455_CellTemp[ATE_TEST_BOARD_IDX][nDev_ID][i] - PL455_CellTemp[ATE_STD_BOARD_IDX][nDev_ID][i] ;
			
			if( diff < 0)
			{
				diff = -diff ;
			}

			if( diff > CELL_TEMP_DIFF_TH )
			{
				return -1 ;
			}
			else
			{
				// do nothing
			}
		}
	}

	return 1 ;
}
/*******************************************************************************
* NAME:             PL455_CheckCellTemp
* CALLED BY:        PL455_MainTask
* PRECONDITIONS:
* INPUT PARAMETERS: Void
* RETURN VALUES:    Void
* DESCRIPTION:     Cell temperature debonce   .
*******************************************************************************/
static void PL455_CheckCellTemp(unsigned char *pBuf, unsigned char nDev_ID, unsigned int packetNo)
{
  int tmp ;
  unsigned char i,j ;

/* Check Cell Volt */
   j = MAX_TEMP_NUM_PER_MOUDLE ;

  for(i=0; i<MAX_TEMP_NUM_PER_MOUDLE; i++)
  {
    j-- ;
    tmp = *(pBuf+i*2) ;
    tmp = (tmp<<8) | *(pBuf+(i*2+1)) ;

    //printf("Temp sensor sample Physcial =%d; \n", tmp );

    if( (tmp >= 63500) || (tmp <14195) )//temp < -21.3 (temp invaild) or temp >105C
    {
      if( PL455_CellTempErrCnt[packetNo][nDev_ID][j] >= PL455_CT_ERROR_DOBONCE_TIMES )
	    {
        	PL455_CellTemp[packetNo][nDev_ID][j] = 0x7FFF ;
        	PL455_IsCellTempNormal[packetNo][nDev_ID][j] = 0 ;
	    }
      	else
	    {
        	PL455_CellTempErrCnt[packetNo][nDev_ID][j]++ ;
	    }
      PL455_CellTempNormalCnt[packetNo][nDev_ID][j] = 0 ;
    }
    else
    {
      if( PL455_CellTempNormalCnt[packetNo][nDev_ID][j] >= PL455_CT_NORMAL_DOBONCE_TIMES  )
      {
        PL455_IsCellTempNormal[packetNo][nDev_ID][j] = 1 ;
      }
      else
      {
        PL455_CellTempNormalCnt[packetNo][nDev_ID][j]++ ;
      }

      PL455_CellTempErrCnt[packetNo][nDev_ID][j] = 0 ;

      if( PL455_IsCellTempNormal[packetNo][nDev_ID][j] == 1 )
	    {
	   	  int idx ;
        /* tmp rang -27 -- +105C */
        tmp = tmp*PL455_VREF_MV*2/0xFFFF ;
///////////////////////////////////////////////
//if( (Bmu_Config_C.CellTempVref[nDev_ID] - tmp) == 0 )
//printf("PL455 error ..............................\n\n\n");
        tmp = (tmp*10000/(Bmu_Config_C.CellTempVref[nDev_ID]-tmp)) - 2000 + Bmu_Config_C.CellTempOffset[nDev_ID][j] ;

        idx = PL455_BinSearch((int*)PL455_Temp_table, 146, tmp);

        tmp = (idx-40)*10+(unsigned int)(PL455_Temp_table[idx]-tmp)*10/(unsigned int)(PL455_Temp_table[idx] - PL455_Temp_table[idx+1] );

	      if( PL455_CellTemp[packetNo][nDev_ID][j] != 0x7FFF )
        {
          PL455_CellTemp[packetNo][nDev_ID][j] = (tmp+PL455_CellTemp[packetNo][nDev_ID][j])>>1 ;
        }
        else
        {
          PL455_CellTemp[packetNo][nDev_ID][j] = tmp ;
        }

	    }
    }
  }
}


/*******************************************************************************
* NAME:             PL455_CheckPacketVolt
* CALLED BY:        PL455_MainTask
* PRECONDITIONS:
* INPUT PARAMETERS: Void
* RETURN VALUES:    Void
* DESCRIPTION:     Cell temperature debonce   .
*******************************************************************************/
static void PL455_CheckPacketVolt(unsigned char *pBuf,unsigned int packetNo)
{
  int tmp ;

  /* Check Packet Voltage */
    tmp = *pBuf ;
    tmp = (tmp<<8) | *(pBuf+1);

	if( tmp != 0xFFFF )
	{
		if( PL455_PacketVoltNormalCnt[packetNo] >= PL455_PV_NORMAL_DOBONCE_TIMES  )
        {
	  		 PL455_IsPacketVoltNormal[packetNo] = 1 ;
		    }
		   else
		    {
	  		 PL455_PacketVoltNormalCnt[packetNo]++ ;
		    }

       PL455_PacketVoltErrCnt[packetNo] = 0 ;

        if( PL455_IsPacketVoltNormal[packetNo] == 1 )
        {
        	tmp = (tmp*PL455_VREF_MV*2/0xFFFF)*100/323 ;
          //tmp = (tmp*PL455_VREF_MV*2/0xFFFF)*100/370 ;
        	//printf("PV rp %d.",tmp);

        	tmp = tmp*PL455_Cali[packetNo].PVGain/1000 + PL455_Cali[packetNo].PVOffset ;

        	//printf("PV Cali %d.gain %d.offset %d.\n",tmp,PL455_Cali[packetNo].PVGain,PL455_Cali[packetNo].PVOffset);


          	if( PL455_PacketVolt[packetNo]!= 0xFFFF )
  		    {
            	PL455_PacketVolt[packetNo] = (tmp+PL455_PacketVolt[packetNo])>>1 ;
  		    }
		    else
  		    {
            	PL455_PacketVolt[packetNo] = tmp ;
  		    }
	     }

	  }
	else
	 {
		  //printf("PL455 P%d error ..............................\n",packetNo);
      if( PL455_PacketVoltErrCnt[packetNo] >= PL455_PV_ERROR_DOBONCE_TIMES )
		    {
  			 PL455_PacketVolt[packetNo] = 0xFFFF ;
  			 PL455_IsPacketVoltNormal[packetNo] = 0 ;
		    }
		  else
		    {
         	 PL455_PacketVoltErrCnt[packetNo]++ ;
	    	}
	  		PL455_PacketVoltNormalCnt[packetNo] = 0 ;
	}

 }

static void PL455_CheckRefVolt(unsigned char *pBuf, unsigned char nDev_ID, unsigned int packetNo)
{
	int tmp ;

	if (packetNo != ATE_STD_BOARD_IDX)
	{
		return ;
	}

	if( nDev_ID == 0 )
	{
		tmp = (*(pBuf+6)<<8) | *(pBuf+7);
		if( tmp != 0xFFFF)
		{
			tmp = (tmp*PL455_VREF_MV*2/0xFFFF)*2 ;
		}
		//printf(" tmp =%d..\n",tmp) ;
		PL455_TestBoardRef1 = tmp ;

	}
	else
	{
		tmp = (*(pBuf+6)<<8) | *(pBuf+7);
		if( tmp != 0xFFFF)
		{
			tmp = (tmp*PL455_VREF_MV*2/0xFFFF)*2 ;
		}
		//printf(" tmp =%d.\n",tmp) ;
		PL455_TestBoardRef2 = tmp ;
	}

 }


/*******************************************************************************
* NAME:             PL455_CheckPacketCurr
* CALLED BY:        PL455_MainTask
* PRECONDITIONS:
* INPUT PARAMETERS: Void
* RETURN VALUES:    Void
* DESCRIPTION:     packet current debonce   .
*******************************************************************************/
static void PL455_CheckPacketCurr(unsigned char *pBuf, unsigned int packetNo)
{
  int tmp ;
	unsigned char i ;
	static int currBuf[MAX_PACKET_NUM][8] ;
	static unsigned char currIdx[MAX_PACKET_NUM] ={0,0} ;

  tmp = *pBuf ;
  tmp = (tmp<<8) | *(pBuf+1) ;

  if( tmp != 0xFFFF )
   {
		if( PL455_PacketCurrNormalCnt[packetNo] >= PL455_PC_NORMAL_DOBONCE_TIMES  )
		 {
		    PL455_IsPacketCurrNormal[packetNo] = 1 ;
		 }
 		else
		 {
			 PL455_PacketCurrNormalCnt[packetNo]++ ;
		 }

    PL455_PacketCurrErrCnt[packetNo] = 0 ;

	    if( PL455_IsPacketCurrNormal[packetNo] == 1 )
	     {
          tmp = (tmp*PL455_VREF_MV*2/0xFFFF) ; // adc value

//#ifdef APOLLO_PL455_DEBUG
#if 0
          printf("PC sample current Voltage =%d; packetNo =%d.\n", tmp, packetNo );
#endif

        //tmp = (10*tmp - 25000)/3 ;
        tmp = (10*tmp - 25000)/3 ;

        if( tmp >= 0 )
        {
            tmp = tmp*PL455_Cali[packetNo].PCGainOfChg/1000 + PL455_Cali[packetNo].PCOffsetOfChg ;
		}
		else if( tmp < 0 )
          {
			tmp = tmp*PL455_Cali[packetNo].PCGainOfDischg/1000 + PL455_Cali[packetNo].PCOffsetOfDischg ;
          }


          if( PL455_PacketCurr[packetNo] != 0x7FFF )
          {
            int sum ;

            currBuf[packetNo][ currIdx[packetNo] ] = tmp;

            currIdx[packetNo]++;
            if( currIdx[packetNo] >= 4 )
		         {
              currIdx[packetNo] = 0;
		         }

            sum = 0;
            for(i=0;i<4;i++)
		         {
              sum += currBuf[packetNo][i] ;
		         }

            PL455_PacketCurr[packetNo] = sum/4 ;
          //  PL455_PacketCurr = (tmp+PL455_PacketCurr)>>1 ;

          }
          else
          {
            PL455_PacketCurr[packetNo] = tmp ;

            for(i=0; i<4;i++)
		         {
              currBuf[packetNo][i] = tmp ;
		         }

            currIdx[packetNo] = 0 ;
          }
	     }
	}
	else
	{
		  if( PL455_PacketCurrErrCnt[packetNo] >= PL455_PC_ERROR_DOBONCE_TIMES )
		    {
    			 PL455_PacketCurr[packetNo] = 0x7FFF ;
	    		 PL455_IsPacketCurrNormal[packetNo] = 0 ;
		    }
		  else
		    {
         	 PL455_PacketCurrErrCnt[packetNo]++ ;
	    	}
	  		PL455_PacketCurrNormalCnt[packetNo] = 0 ;
	}
}


/*******************************************************************************
* NAME:             PL455_CheckDieTemp
* CALLED BY:        PL455_MainTask
* PRECONDITIONS:
* INPUT PARAMETERS: Void
* RETURN VALUES:    Void
* DESCRIPTION:     packet current debonce   .
*******************************************************************************/
static void PL455_CheckDieTemp(unsigned char *pBuf, unsigned char nDev_ID, unsigned int packetNo)
{
  int tmp ;

  tmp = *pBuf ;
  tmp = (tmp<<8) | *(pBuf+1) ;
  tmp = tmp*PL455_VREF_MV*2/0xFFFF  ;

  //Internal Digital Die Temperature °C = (VADC – 2.287) × 131.944
  PL455_DigDieTemp[packetNo][nDev_ID] = (tmp - 2287)*1319/1000 ; //uint 0.1 C
}

/*******************************************************************************
* NAME:             PL455_CheckMoudleVolt
* CALLED BY:        PL455_MainTask
* PRECONDITIONS:
* INPUT PARAMETERS: Void
* RETURN VALUES:    Void
* DESCRIPTION:     packet current debonce   .
*******************************************************************************/
static void PL455_CheckMoudleVolt(unsigned char *pBuf, unsigned char nDev_ID,unsigned int packetNo)
{
  int tmp ;

  tmp = *pBuf ;
  tmp = (tmp<<8) | *(pBuf+1) ;
  tmp = tmp*PL455_VREF_MV*2/0xFFFF  ;

  /* VMODULE = ([(2 × VREF) / 65535] × READ_ADC_VALUE) × 25  */
  PL455_VoltMoudle[packetNo][nDev_ID] = tmp/4 ; //unit 0.1V
}


/*******************************************************************************
* NAME:             PL455_CheckComm
* CALLED BY:        PL455_MainTask
* PRECONDITIONS:
* INPUT PARAMETERS: Void
* RETURN VALUES:    Void
* DESCRIPTION:     packet current debonce   .
*******************************************************************************/
static void PL455_CheckComm(unsigned char ErrSt,unsigned int packetNo)
{
	if( (ErrSt == PL455_COMM_ST_NORMAL) && (PL455_CommErrSt[packetNo] == PL455_COMM_ST_ERROR) )
		{
	 	   if( PL455_CommNormalCnt[packetNo] >= PL455_COMM_NORMAL_DOBONCE_TIMES  )
        {
	  		  PL455_CommErrSt[packetNo] = PL455_COMM_ST_NORMAL ;
		    }
		   else
		    {
	  		  PL455_CommNormalCnt[packetNo]++ ;
		    }

       PL455_CommErrCnt[packetNo] = 0 ;
    }
    else if( (ErrSt == PL455_COMM_ST_ERROR) && (PL455_CommErrSt[packetNo] == PL455_COMM_ST_NORMAL)  )
    {
	 	   if( PL455_CommErrCnt[packetNo] >= PL455_COMM_ERROR_DOBONCE_TIMES  )
        {
	  		  PL455_CommErrSt[packetNo] = PL455_COMM_ST_ERROR ;
	  		  PL455_ReInitEn[packetNo]  = TRUE ;
		    }
		   else
		    {
	  		  PL455_CommErrCnt[packetNo]++ ;
		    }

       PL455_CommNormalCnt[packetNo] = 0 ;
    }
}



/*******************************************************************************
 Deubug function
*******************************************************************************/

#if( APOLLO_PRIMEVAL_DEBUG == 1)
void PL455_PrimevalValDebug(unsigned char *pBuf , unsigned int packetNo)
{
#if 0
    LOG_I("the PL455 primeval value list as: " );
    LOG_I("CV01 value = %d;   ", (*(pBuf+1)<<8) | *(pBuf+2) );
    LOG_I("CV02 value = %d; \n", (*(pBuf+3)<<8) | *(pBuf+4) );
    LOG_I("CV03 value = %d;   ", (*(pBuf+5)<<8) | *(pBuf+6) );
    LOG_I("CV04 value = %d; \n", (*(pBuf+7)<<8) | *(pBuf+8) );

    LOG_I("CV05 value = %d;   ", (*(pBuf+9)<<8) | *(pBuf+10) );
    LOG_I("CV06 value = %d; \n", (*(pBuf+11)<<8) | *(pBuf+12) );
    LOG_I("CV07 value = %d;   ", (*(pBuf+13)<<8) | *(pBuf+14) );
    LOG_I("CV08 value = %d; \n", (*(pBuf+15)<<8) | *(pBuf+16) );

    LOG_I("CV09 value = %d;   ", (*(pBuf+17)<<8) | *(pBuf+18) );
    LOG_I("CV10 value = %d; \n", (*(pBuf+19)<<8) | *(pBuf+20) );
    LOG_I("CV11 value = %d;   ", (*(pBuf+21)<<8) | *(pBuf+22) );
    LOG_I("CV12 value = %d; \n", (*(pBuf+23)<<8) | *(pBuf+24) );

    LOG_I("CV13 value = %d;   ", (*(pBuf+25)<<8) | *(pBuf+26) );
    LOG_I("CV14 value = %d; \n", (*(pBuf+27)<<8) | *(pBuf+28) );
    LOG_I("CV15 value = %d;   ", (*(pBuf+29)<<8) | *(pBuf+30) );
    LOG_I("CV16 value = %d; \n", (*(pBuf+31)<<8) | *(pBuf+32) );


    LOG_I("Digital Die temp = %d;", (*(pBuf+33)<<8) | *(pBuf+34) );
    LOG_I("Moudle  Volt = %d; \n",  (*(pBuf+35)<<8) | *(pBuf+36) );

    LOG_I("Temp01 value = %d;   ", (*(pBuf+37)<<8) | *(pBuf+38) );
    LOG_I("Temp02 value = %d; \n", (*(pBuf+39)<<8) | *(pBuf+40) );
    LOG_I("Temp03 value = %d;   ", (*(pBuf+41)<<8) | *(pBuf+42) );
    LOG_I("Temp04 value = %d; \n", (*(pBuf+43)<<8) | *(pBuf+44) );

    LOG_I("Packet Current value = %dmA; ",(*(pBuf+45)<<8) | *(pBuf+46)  );
    LOG_I("Packet Volt value = %d; \n\n", (*(pBuf+47)<<8) | *(pBuf+48)  );
#endif

   // for(packetNo =0 ; packetNo <2; packetNo++ )
	{
		for( unsigned char nDev_ID = 0; nDev_ID < MAX_MOUDLE_NUM; nDev_ID++ )
		{
			LOG_I("P%d M%d CV01-08:%d, %d, %d, %d, %d, %d, %d, %d;",  packetNo, nDev_ID,
				PL455_CellVolt[packetNo][nDev_ID][0], PL455_CellVolt[packetNo][nDev_ID][1],
				PL455_CellVolt[packetNo][nDev_ID][2], PL455_CellVolt[packetNo][nDev_ID][3],
				PL455_CellVolt[packetNo][nDev_ID][4], PL455_CellVolt[packetNo][nDev_ID][5],
				PL455_CellVolt[packetNo][nDev_ID][6], PL455_CellVolt[packetNo][nDev_ID][7] );

			LOG_I("P%d M%d CV09-16:%d, %d, %d, %d, %d, %d, %d, %d;",  packetNo, nDev_ID,
					PL455_CellVolt[packetNo][nDev_ID][8],  PL455_CellVolt[packetNo][nDev_ID][9],
				PL455_CellVolt[packetNo][nDev_ID][10], PL455_CellVolt[packetNo][nDev_ID][11],
					PL455_CellVolt[packetNo][nDev_ID][12], PL455_CellVolt[packetNo][nDev_ID][13],
				PL455_CellVolt[packetNo][nDev_ID][14], PL455_CellVolt[packetNo][nDev_ID][15] );

			LOG_I("P%d M%d CT01-04:%0.1f, %0.1f, %0.1f, %0.1f, DigitalDie:%d, MV:%d;",  packetNo, nDev_ID,
					PL455_CellTemp[packetNo][nDev_ID][0]/10.0f, PL455_CellTemp[packetNo][nDev_ID][1]/10.0f,
					PL455_CellTemp[packetNo][nDev_ID][2]/10.0f, PL455_CellTemp[packetNo][nDev_ID][3]/10.0f,
					PL455_DigDieTemp[packetNo][nDev_ID], PL455_VoltMoudle[packetNo][nDev_ID] );
		}


		LOG_I("P%d:Packet Current =%dmA,Packet Volt =%0.1fV ",
				packetNo, PL455_PacketCurr[packetNo], PL455_PacketVolt[packetNo]/10.0f );
	}

}
#endif


void PL455_ExceptionHand(unsigned int packetNo)
{
  static unsigned int excepCnt[MAX_PACKET_NUM]  = {0,0} ;
  static unsigned int excepEn[MAX_PACKET_NUM]   = {0,0} ;
  static unsigned int excepStep[MAX_PACKET_NUM] = {0,0} ;

 // if(excepEn ==  0)
 // return;

  excepCnt[packetNo]++ ;

  if( excepCnt[packetNo] >= 5) // 2S -> 500ms
  {
    excepStep[packetNo]++ ;
    excepCnt[packetNo] = 0 ;
    excepEn[packetNo] = 1 ;
  }

  if(excepEn[packetNo] == 1)
  {
    excepEn[packetNo] = 0 ;

  switch(excepStep[packetNo])
  {
    unsigned char bFrame[4];
    case 0:
    {
    }
    break;

    case 1:
    {
       Ioa_BmuWakeupLow();     // assert wake ;
       PL455_Delay5ms();

      /* send out broadcast pwrdown command */
        bFrame[0] = 0x40 ;
        PL455_WriteReg(0, PL455_REG_DEV_CTRL, bFrame, 1, FRMWRT_ALL_NR);

        PL455_Delay5ms();
#if( APOLLO_PL455_REINIT_DEBUG == 1 )
        LOG_E("<PL455> - Exception:Power down.");
#endif
    }
    break;

    case 2:
    {
    /* WAKEUP hold time (high-pulse width) :At least 3.7ms */
     Ioa_BmuWakeupHigh();
#if( APOLLO_PL455_REINIT_DEBUG == 1 )
     LOG_E("<PL455> - Exception:Wake Up.");
#endif
    }
    break;

    case 3:
    {
      PL455_SetAddress();
      PL455_ComPortInit(packetNo);
      PL455_AdcConfigure();
      //PL455_FaultSensorInit();

	    for(unsigned char nDev_ID=0;nDev_ID < MAX_MOUDLE_NUM;nDev_ID++)
	    {
	      for(unsigned char i=0;i<MAX_CELL_NUM_PER_MOUDLE;i++)
		    {
		      PL455_CellVolt[packetNo][nDev_ID][i] = 0xFFFF ;
				PL455_IsCellVoltNormal[packetNo][nDev_ID][i] = 1 ;
		      PL455_CellVoltNormalCnt[packetNo][nDev_ID][i] = 0 ;
		      PL455_CellVoltErrCnt[packetNo][nDev_ID][i] = 0 ;
		    }
	    }

	    for(unsigned char nDev_ID=0;nDev_ID < MAX_MOUDLE_NUM;nDev_ID++)
	    {
	      for(unsigned char i=0;i<MAX_TEMP_NUM_PER_MOUDLE;i++)
		    {
		      PL455_CellTemp[packetNo][nDev_ID][i] = 0x7FFF ;
	      	PL455_IsCellTempNormal[packetNo][nDev_ID][i] = 1 ;
		      PL455_CellTempNormalCnt[packetNo][nDev_ID][i] = 0 ;
		      PL455_CellTempErrCnt[packetNo][nDev_ID][i] = 0 ;
		    }
	    }

      PL455_PacketVolt[packetNo] = 0xFFFF ;
      PL455_IsPacketVoltNormal[packetNo] = 1 ;
      PL455_PacketVoltNormalCnt[packetNo] = 0 ;
      PL455_PacketVoltErrCnt[packetNo] = 0 ;

      PL455_PacketCurr[packetNo]  = 0x7FFF ;
      PL455_IsPacketCurrNormal[packetNo] = 1 ;
      PL455_PacketCurrNormalCnt[packetNo] = 0 ;
      PL455_PacketCurrErrCnt[packetNo] = 0 ;

      PL455_CommNormalCnt[packetNo] = 0 ;
      PL455_CommErrCnt[packetNo] = 0 ;
      PL455_CommErrSt[packetNo] = PL455_COMM_ST_NORMAL ;
#if( APOLLO_PL455_REINIT_DEBUG == 1 )
      LOG_E("<PL455> - PL455 Exception:Do Init.");
#endif
    }
    break;

    case 4:
    {
      /* The WAKEUP pin is usually kept in the de-asserted (low) state. */
      Ioa_BmuWakeupLow();
      excepEn[packetNo] = 0 ;
      excepCnt[packetNo] = 0 ;
      excepStep[packetNo] = 0 ;
      PL455_ReInitEn[packetNo] =  FALSE ;
    }
    break;

    default:
    break;
   }

  }

}



#if( APOLLO_PL455_DEBUG_FUNC == 1)
int PL455_DebugFunc(unsigned int packetNo)
{
  unsigned char nDev_ID ;
  char name[64] ;
	if( packetNo == ATE_STD_BOARD_IDX)
	{
		strcpy(name,"Stand BMU Board") ;
	}
	else if( packetNo == ATE_TEST_BOARD_IDX)
	{
		strcpy(name,"Test BMU Board") ;
	}
	else
	{
		strcpy(name,"BMU Board No errror") ;
	}

#if 0
  for( nDev_ID = 0; nDev_ID < 2; nDev_ID++ )
  {
    printf("%s M%d; \n",name, nDev_ID );
    printf("CV01 value  =%d;   ", PL455_CellVolt[packetNo][nDev_ID][0] );
    printf("CV02 value  =%d; \n", PL455_CellVolt[packetNo][nDev_ID][1] );
    printf("CV03 value  =%d;   ", PL455_CellVolt[packetNo][nDev_ID][2] );
    printf("CV04 value  =%d; \n", PL455_CellVolt[packetNo][nDev_ID][3] );

    printf("CV05 value  =%d;   ", PL455_CellVolt[packetNo][nDev_ID][4] );
    printf("CV06 value  =%d; \n", PL455_CellVolt[packetNo][nDev_ID][5] );
    printf("CV07 value  =%d;   ", PL455_CellVolt[packetNo][nDev_ID][6] );
    printf("CV08 value  =%d; \n", PL455_CellVolt[packetNo][nDev_ID][7] );

    printf("CV09 value  =%d;   ", PL455_CellVolt[packetNo][nDev_ID][8] );
    printf("CV10 value  =%d; \n", PL455_CellVolt[packetNo][nDev_ID][9] );
    printf("CV11 value  =%d;   ", PL455_CellVolt[packetNo][nDev_ID][10] );
    printf("CV12 value  =%d; \n", PL455_CellVolt[packetNo][nDev_ID][11] );

    printf("CV13 value  =%d;   ", PL455_CellVolt[packetNo][nDev_ID][12] );
    printf("CV14 value  =%d; \n", PL455_CellVolt[packetNo][nDev_ID][13] );
    printf("CV15 value  =%d;   ", PL455_CellVolt[packetNo][nDev_ID][14] );
    printf("CV16 value  =%d; \n", PL455_CellVolt[packetNo][nDev_ID][15] );


    printf("Digital Die temp  =%d;  ", PL455_DigDieTemp[packetNo][nDev_ID] );
    printf("Moudle  Volt  =%d; \n", PL455_VoltMoudle[packetNo][nDev_ID] );

    printf("Temp01 value  =%d;   ", PL455_CellTemp[packetNo][nDev_ID][0] );
    printf("Temp02 value  =%d; \n", PL455_CellTemp[packetNo][nDev_ID][1] );
    printf("Temp03 value  =%d;   ", PL455_CellTemp[packetNo][nDev_ID][2] );
    printf("Temp04 value  =%d; \n", PL455_CellTemp[packetNo][nDev_ID][3] );
  }
#endif
    // LOG_I("Packet Current=%dmA;Packet Volt=%d.", PL455_PacketCurr[packetNo],PL455_PacketVolt[packetNo] );

    for( nDev_ID = 0; nDev_ID < 2; nDev_ID++ )
    {
    	unsigned char cell_no = 0 ;
    	for( cell_no = 0; cell_no < 16; cell_no++ )
    	{
    		if( (PL455_CellVolt[packetNo][nDev_ID][cell_no] > 4400) ||
    			(PL455_CellVolt[packetNo][nDev_ID][cell_no] < 3000) )
    		{
    			return -1 ;
    		}
    	}
    }

    return 1 ;

 }
#endif


