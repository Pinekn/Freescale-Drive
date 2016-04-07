#include "../inc/eload.h"
#include "../inc/uart.h"
#include "../inc/net_log.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>



#define EPWR_SET_TARGET_VOLT  			0
#define EPWR_SET_READ_STATUS 			1
#define EPWR_DO_READ_WAIT 	 			2
#define EPWR_DO_READ_STATUS 			3


static char ELOAD_TYPE_USED[6] ={'8','5','1','2','B'} ;
static char EPOWER_TYPE_USED[6] ={'6','8','3','4',NULL} ; //0x32 is space key ASCII
static int eload_writeFrame( unsigned char cmd, unsigned char *pFrame );
static int eload_readFrame( unsigned char *cmd, unsigned char *pFrame, unsigned char len );
int ITECH_getDevSn(int dev_type);
int ITECH_SetRemoteCtrlMode(int dev_type) ;
int ITECH_TurnOnDev(bool enable, int dev_type);
int ITECH_eLoadSetPwrMode(char pwr_mode);
int ITECH_eLoadSetPwrPar(char pwr_mode, int pwr_par);

int ITECH_ePwrSetVolt(bool enable, int volt) ;
int ITECH_ePwrSetCurrent(int curr);

void eload_Init(void)
{
	Uart1_Init();
	Uart2_Init();
	//LOG_I("<eload> - Do init.") ;
}

void eload_Task(void)
{
	static int eload_step = 1 ;

	switch( eload_step )
	{
		case 1 : //get eload device sn ;
		{
			//int ret = ITECH_getDevSn(ITECH_DEV_ELOAD);
			int ret = ITECH_getDevSn(ITECH_DEV_EPOWER);
			if( ret == 1)
			{
				eload_step = 2 ;
				LOG_I("<eLoad> - Get eload device sn ok.");
			}
			else
			{
				//eload_step = 20 ;
				LOG_I("<eLoad> - Get eload device sn - FAILD.");
			}
		}
		break ;

		case 2 :
		{
			int ret = ITECH_SetRemoteCtrlMode(ITECH_DEV_EPOWER);
			if( ret >= 0)
			{
				LOG_I("<eLoad> - set eload work to remote mode ok.");
				eload_step = 7 ;
			}
			else
			{
				LOG_I("<eLoad> - set eload work to remote mode faild.");
			}
		}
		break ;

		case 3:
		{
			int ret = ITECH_TurnOnDev( true, ITECH_DEV_EPOWER);
			if( ret >= 0)
			{
				LOG_I("<eLoad> - turn on eload remote mode ok.");
				eload_step = 24 ;
			}
		}
		break;

		case 4:
		{
			int ret = ITECH_eLoadSetPwrMode(ELOAD_IN_CC) ;
			if( ret >= 0)
			{
				LOG_I("<eLoad> - set eload work in CC mode ok.");
				eload_step = 5 ;
			}

		}
		break;

		case 5:
		{
			int ret = ITECH_eLoadSetPwrPar(ELOAD_IN_CC, 100) ;
			if( ret >= 0)
			{
				LOG_I("<eLoad> - set 100mA cc.");
				eload_step = 18 ;
			}

		}
		break ;

		case 6:
		{
			ITECH_TurnOnDev( false, ITECH_DEV_EPOWER);
		}
		break ;

		case 7 :
		{
			int ret = ITECH_ePwrSetCurrent(20) ; //20mA
			if( ret >= 0)
			{
				LOG_I("<ePower> - set output current 20mA.");
				eload_step = 8 ;
			}
		}
		break ;

		case 8 :
		{
			int ret = ITECH_ePwrSetVolt(true, 600) ;
			if( ret > 0)
			{
				LOG_I("<ePower> - set 60.0V.");
				eload_step = 3 ;
			}

		}
		break ;

		case 18:
		{
			static unsigned int cnt = 0 ;

			if( cnt < 5 )
			{
				cnt++ ;
				usleep(1000);
			}
			else
			{
				eload_step = 6 ;
			}
		}
		break ;
		default:
		break;
	}
}



int ITECH_SetRemoteCtrlMode(int dev_type)
{
	int cmd ;
	if( dev_type == ITECH_DEV_ELOAD )
	{
		cmd = ELOAD_CMD_CTRL_MODE ;
	}
	else if( dev_type == ITECH_DEV_EPOWER )
	{
		cmd = EPWR_CMD_CTRL_MODE ;
	}
	else
	{
		return 0xFF ;
	}

	unsigned char buff[22];

	memset(buff, 0x00, sizeof(buff) );
	buff[0] = ELOAD_OP_MODE_REMOTE ;

	int ret = eload_writeFrame( cmd, buff );

	if( ret < 0 )
	{
		LOG_I("<eLoad> - set remote mode faild. %d.",ret);
		return -1 ;
	}

	return ret ;
}


int ITECH_TurnOnDev(bool enable, int dev_type)
{
	int cmd ;
	if( dev_type == ITECH_DEV_ELOAD )
	{
		cmd = ELOAD_CMD_INPUT_ST ;
	}
	else if( dev_type == ITECH_DEV_EPOWER )
	{
		cmd = EPWR_CMD_INPUT_ST ;
	}
	else
	{
		return 0xFF ;
	}

	unsigned char buff[22];
	memset(buff, 0x00, sizeof(buff) );

	buff[0] = enable ;

	int ret = eload_writeFrame( ELOAD_CMD_INPUT_ST, buff );

	if( ret < 0 )
	{
		LOG_I("<eLoad> - send turn on/off eload remote mode faild. %d.",ret);
		return -1 ;
	}

	return ret ;
}

 int ITECH_getDevSn(int dev_type)
{
	unsigned char readInfo,raedBuf[256] ;

	memset(raedBuf, 0x00, sizeof(raedBuf) );

	int cmd ;
	if( dev_type == ITECH_DEV_ELOAD )
	{
		cmd = ELOAD_DEV_SN ;
	}
	else if( dev_type == ITECH_DEV_EPOWER )
	{
		cmd = EPWR_CMD_DEV_SN ;
	}
	else
	{
		return 0xFF ;
	}

	int ret = eload_writeFrame( cmd, raedBuf );

	if( ret < 0 )
	{
		LOG_I("<eLoad> - send poll eload device sn faild. %d.",ret);
		return -1 ;
	}

	ret = eload_readFrame(&readInfo, raedBuf, sizeof(raedBuf));

	if( ret < 0 )
	{
		LOG_I("<eLoad> - Device Type:read uart error.");
		return -1 ;
	}

	LOG_I("<eLoad> - Device Type:%2x %2x %2x %2x %2x.",raedBuf[0],raedBuf[1],raedBuf[2],raedBuf[3],raedBuf[4] );
	LOG_I("<eLoad> - Device Type:%c%c%c%c%c",raedBuf[0],raedBuf[1],raedBuf[2],raedBuf[3],raedBuf[4] );
	LOG_I("<eLoad> - Device Software V%x.%x",raedBuf[6],raedBuf[5] ); //BCD
	LOG_I("<eLoad> - Device No:%c%c%c%c%c-%c%c%c%c%c",raedBuf[7], raedBuf[8], raedBuf[9], raedBuf[10], raedBuf[11],
													 raedBuf[12], raedBuf[13], raedBuf[14], raedBuf[15], raedBuf[16] );

	int err_cnt = 0 ;
	if( dev_type == ITECH_DEV_ELOAD )
	{
		for(unsigned char i=0; i<5; i++)
		{
			if( raedBuf[i] != ELOAD_TYPE_USED[i] )
			{
				//LOG_I("<eLoad> - Device Type:error." );
				//return 0 ;
				err_cnt++ ;
			}
		}

		if( err_cnt > 2)
		{
			LOG_I("<eLoad> - Device Type:error." );
			//return 0 ;
		}
	}
	else if( dev_type == ITECH_DEV_EPOWER )
	{
		for(unsigned char i=0; i<5; i++)
		{
			if( raedBuf[i] != EPOWER_TYPE_USED[i] )
			{
				//LOG_I("<ePower> - Device Type:error." );
				//return 0 ;
				err_cnt++ ;
			}
		}


		if( err_cnt > 2)
		{
			LOG_I("<eLoad> - Device Type:error." );
			return 0 ;
		}

	}


	for(unsigned char i=7; i<17; i++)
	{
	}

	return 1 ;
}


int ITECH_eLoadSetPwrMode(char pwr_mode)
{
	unsigned char buff[22];

	memset(buff, 0x00, sizeof(buff) );

	if( pwr_mode > 3 )
	{
		LOG_I("<eLoad> - set eload power mode faild. beyond the range.");
		return -2 ;
	}
	else
	{
		buff[0] = pwr_mode ;
	}

	int ret = eload_writeFrame( ELOAD_CMD_PWR_MODE, buff );

	if( ret < 0 )
	{
		LOG_I("<eLoad> - set remote mode faild. %d.",ret);
		return -1 ;
	}

	return ret ;
}

int ITECH_eLoadSetMaxCurr(void)
{
	unsigned char buff[22];

	memset(buff, 0x00, sizeof(buff) );

	buff[0] = 0x70 ;
	buff[1] = 0x11 ;
	buff[2] = 0x01 ;

	int ret = eload_writeFrame( 0x24, buff );

	if( ret < 0 )
	{
		LOG_I("<eLoad> - set remote mode faild. %d.",ret);
		return -1 ;
	}

	return ret ;
}

int ITECH_eLoadSetPwrPar(char pwr_mode, int pwr_par)
{
	unsigned char buff[22];

	memset(buff, 0x00, sizeof(buff) );

	if( pwr_mode > 3 )
	{
		LOG_I("<eLoad> - set eload power mode faild. beyond the range.");
		return -2 ;
	}

	unsigned char cmd ;

	switch( pwr_mode )
	{
		case ELOAD_IN_CC :
		{
			cmd = ELOAD_CMD_WRITE_CC ;

			int curr = pwr_par*10 ; // gain =0.1
			buff[0] = (unsigned char)curr ;
			buff[1] = (unsigned char)(curr>>8) ;
			buff[2] = (unsigned char)(curr>>16) ;
			buff[3] = (unsigned char)(curr>>24) ;
		}
		break ;

		case ELOAD_IN_CV :
		{
			cmd = ELOAD_CMD_WRITE_CV ;
		}
		break ;

		case ELOAD_IN_CW :
		{

		}
		break ;

		case ELOAD_IN_CR :
		{

		}
		break ;

		default:
		break;
	}

	int ret = eload_writeFrame( cmd, buff );

	if( ret < 0 )
	{
		//LOG_I("<eLoad> - set remote mode faild. %d.",ret);
		return -1 ;
	}

	return ret ;
}

int eload_SetDCVoltOV(void)
{
	return 1 ;
}

int eload_SetDCVoltUV(void)
{
	return 1 ;
}

int eload_SetDCVoltNormal(void)
{
	return 1 ;
}

int ITECH_ePwrSetVolt_zero(void)
{
	int ret = ITECH_ePwrSetVolt(true, 0) ;
	return ret ;
}

int ITECH_ePwrSetVolt(bool enable, int volt)
{
	static int set_step = EPWR_SET_TARGET_VOLT ;
	static int set_cnt = 0 ;

	unsigned char readInfo,buff[256] ;
	int result ;


	if( enable == false )
	{
		set_step = EPWR_SET_TARGET_VOLT ;
		set_cnt = 0 ;
		return 1 ;
	}

	switch( set_step )
	{
		case EPWR_SET_TARGET_VOLT :
		{
			// set the target voltage
			memset(buff, 0x00, sizeof(buff) );
			int volt_t = volt*100 ; // volt_t: unit -- 1mV ; volt: unit -- 1V ;
			buff[0] = (unsigned char)volt_t ;
			buff[1] = (unsigned char)(volt_t>>8) ;
			buff[2] = (unsigned char)(volt_t>>16) ;
			buff[3] = (unsigned char)(volt_t>>24) ;

			int ret = eload_writeFrame( EPWR_CMD_SET_VOLT, buff );

			if( ret < 0 )
			{
				LOG_I("<ePower> - send set ePower volt to 0 faild. %d.",ret);
				result = -1 ; //error, stop
			}
			else
			{
				//set_step = EPWR_SET_READ_STATUS ;
				//set_cnt = 0 ;
				result = 0 ; //do next step
				ITECH_TurnOnDev( true, ITECH_DEV_EPOWER);
				LOG_I("<ePower> - send set ePower volt=%d.(mV)",volt_t);

		///////////////////////////////////////////////////////////////////////////
				set_cnt++ ;
				if( set_cnt > 3)
				{
					result = 1 ; //do next step
					set_cnt = 0 ;
				}
				else
				{

				}

		/////////////////////////////////////////////////////////////////////////
			}
		}
		break ;

		case EPWR_SET_READ_STATUS :
		{
			// read the ePower voltage,current and status
			memset(buff, 0x00, sizeof(buff) );
			int ret = eload_writeFrame( EPWR_CMD_READ_PAR, buff );
			if( ret < 0 )
			{
				LOG_I("<ePower> - send read ePower status command faild. %d.",ret);
				result = -1 ; //error, stop
			}
			else
			{
				set_step = EPWR_DO_READ_WAIT ;
				set_cnt = 0 ;
				result = 0 ; //do next step
			}

		}
		break ;

		case EPWR_DO_READ_WAIT :
		{
			set_cnt++ ;

			if( set_cnt >= 50)
			{
				set_cnt = 0 ;
				set_step = EPWR_DO_READ_STATUS ;
			}
			result = 0 ; //do next step
		}
		break;

		case EPWR_DO_READ_STATUS :
		{

			set_cnt++ ;

			int try_tmie = 6 ;
			do
			{
				usleep(10000); //1ms
				int ret = eload_readFrame(&readInfo, buff, sizeof(buff));
				if( ret < 0 )
				{
					//LOG_I("<ePower> - Device Type:read uart error.");

					if( set_cnt >= 5 )
					{
						set_step = EPWR_SET_TARGET_VOLT ;
					}

					result = -1 ; //error, stop
					continue ;
				}


				if(readInfo != EPWR_CMD_READ_PAR )
				{
					LOG_I("<ePower> -command type error.0x%x.", readInfo);
					if( set_cnt >= 5 )
					{
						set_step = EPWR_SET_TARGET_VOLT ;
					}
					result = 0 ; //do next step
					continue ;
				}
				else
				{
					LOG_I("<ePower> -command type OK.0x%x.", readInfo);
					int eVolt_a, eCurr_a, eSt, eCurr_s, eMaxVolt, eVolt_s ;
					eCurr_a = ((int)buff[1]<<8) | buff[0] ;
					eVolt_a = ((int)buff[5]<<24) | ((int)buff[4]<<16) | ((int)buff[3]<<8) | buff[2] ;
					eSt     = buff[6] ;
					eCurr_s = ((int)buff[8]<<8) | buff[7] ;
					eMaxVolt = ((int)buff[12]<<24) | ((int)buff[11]<<16) | ((int)buff[10]<<8) | buff[9] ;
					eVolt_s  = ((int)buff[16]<<24) | ((int)buff[15]<<16) | ((int)buff[14]<<8) | buff[13] ;

					LOG_I("<ePower> - Actual Current:%d, Actual Volt:%d, Status:%d.", eCurr_a, eVolt_a, eSt );
					LOG_I("<ePower> - Set Current:%d, Set max Volt:%d, Set Volt:%d.", eCurr_s, eMaxVolt, eVolt_s );


					int tmp = volt*1000 ;
					tmp = tmp - eVolt_a ;

					if( (tmp < 200) && ( tmp > -200) )
					{
						result = 1 ; //finished
						LOG_I("<ePower> -Set Volt Ok.%d.",tmp );
						break;
					}
					else
					{
						if( set_cnt <= 8 )
						{
							set_step = EPWR_SET_TARGET_VOLT ;
						}
						result = -1 ; //error, stop
					}

					LOG_I("<ePower> - target volt=%dmV.diff=%d.mV", volt*1000 );
				}
			}while(try_tmie--);
		}
		break ;

		default:
		{
			result = -1 ; //error, stop
		}
		break;
	}

	return result ;
}

int ITECH_ePwrSetCurrent(int curr)
{
	unsigned char buff[8] ;
	int set_curr = curr ; // mA

	if( (set_curr > 1200) || (set_curr < 0) ) // max current 1.2A
	{
		LOG_I("<ePower> - Current setting error.") ;
		return - 2 ;
	}

	buff[0] = (unsigned char)set_curr ;
	buff[1] = (unsigned char)(set_curr>>8) ;


	int ret = eload_writeFrame( EPWR_CMD_SET_CURR, buff );

	if( ret < 0 )
	{
		//LOG_I("<eLoad> - set remote mode faild. %d.",ret);
		return -1 ;
	}

	return ret ;
}

int ITECH_eLoad_read_Input_St( int type)
{
	unsigned char buff[64] ;
	unsigned char readInfo ;
	int eVolt_a, eCurr_a, ePwr_a, ops_Reg, look_Reg, rev_Reg, dev_temp, work_mode ;

	int try_cnt = 0 ;
	//do
	{
		memset(buff, 0x00, sizeof(buff) );
		int ret = eload_writeFrame( ELOAD_CMD_READ_VCPS, buff );

		usleep(100000); //1s
		ret = eload_readFrame(&readInfo, buff, sizeof(buff));
		if( ret > 0 )
		{
			if(readInfo == ELOAD_CMD_READ_VCPS )
			{
				eVolt_a = ((int)buff[3]<<24) | ((int)buff[2]<<16) | ((int)buff[1]<<8) | buff[0] ;
				eCurr_a = ((int)buff[7]<<24) | ((int)buff[6]<<16) | ((int)buff[5]<<8) | buff[4] ;
				ePwr_a  = ((int)buff[11]<<24) | ((int)buff[10]<<16) | ((int)buff[9]<<8) | buff[8] ;
				
				ops_Reg  = buff[12] ;
				look_Reg = ((int)buff[14]<<8) | buff[13] ;
				rev_Reg  = ((int)buff[16]<<8) | buff[15] ;
				dev_temp = buff[17] ;
				work_mode = buff[18] ;

				LOG_I("<ePower> - Actual Volt:%d, Actual Curr:%d,  Actual Power Rate:%d.", eVolt_a,eCurr_a, ePwr_a );
				LOG_I("<ePower> - Operate Reg:%d, lookup Reg:%d, device temp:%d.work mode:%d", ops_Reg, look_Reg, dev_temp, work_mode );
				//break ;
			}
			else
			{
				LOG_I("<ePower> - Act") ;
				return -1 ;
			}
		}
		else
		{
			LOG_I("<ePower> - error.") ;
			return -1 ;
		}

		try_cnt++ ;

	}//while( try_cnt <= 10 ) ;

	if( try_cnt > 10 )
	{
		return 0xFF ;
	}

	if( type == ELOAD_READ_INPUT_VOLT)
	{
		return eVolt_a ;
	}
	else if( type == ELOAD_READ_INPUT_CURR )
	{
		return eCurr_a ;
	}
	else if( type == ELOAD_READ_INPUT_PWR_RATE )
	{
		return ePwr_a ;
	}
	else
	{
		return 0xFF ;
	}
	
}


static int eload_writeFrame( unsigned char cmd, unsigned char *pFrame )
{
	unsigned char bFrame[64] ;
	unsigned char sum = 0 ;

	bFrame[0] = ELOAD_SYNC_HEAD ;
	bFrame[1] = ELOAD_ADD ;
	bFrame[2] = cmd ;

	for( unsigned char i=0; i<ELOAD_CTRL_DATA_LEN; i++ )
	{
		bFrame[3+i] = *(pFrame+i) ;
	}

	//check sum
	for( unsigned char i=0; i<(ELOAD_CTRL_DATA_LEN+3); i++ )
	{
		sum += bFrame[i] ;
	}

	bFrame[ELOAD_CTRL_DATA_LEN+3] = sum ;

	//if( cmd == EPWR_CMD_READ_PAR )
	if(0)
	{
		LOG_I("<eLoad> - write list:");
		for( unsigned char i=0; i<=(ELOAD_CTRL_DATA_LEN+3); i++ )
		{
			printf("%2x ",bFrame[i]);
			if( (i+1)%8 == 0 )
			{
				printf("\n");
			}
		}
		printf("\n");
	}

	int ret = Uart1_Write((char *)bFrame, (unsigned int)(ELOAD_CTRL_DATA_LEN+4) );
	return ret ;
}

static int eload_readFrame( unsigned char *cmd, unsigned char *pFrame, unsigned char len )
{
	int RxFlg ;
	unsigned char bFrame[ELOAD_CTRL_DATA_LEN+10] ;
	unsigned char checksum = 0 ;

	int bLen = ELOAD_CTRL_DATA_LEN + 4 ;

	memset(bFrame, 0xFF, bLen );

	RxFlg = Uart1_Read( (char*)bFrame, bLen) ;

	if( (RxFlg < 0) || (bFrame[0] == 0xFF) )
	{
		LOG_I("<eLoad> - (read frame) eload No Responce.RxFlg =%d.",RxFlg);
		return -1 ; // error.no responce .
	}

	checksum = 0 ;
	for(unsigned char i=0; i<(ELOAD_CTRL_DATA_LEN+3); i++ )
	{
		checksum += bFrame[i] ;
	}
	if( checksum != bFrame[ELOAD_CTRL_DATA_LEN+3] )
	{
		LOG_I("<eLoad> - (read frame) Responce message checksum error. ");
		return -2 ;
	}

	if( bFrame[0] != ELOAD_SYNC_HEAD )
	{
		LOG_I("<eLoad> - (read frame) Responce message sync head  error. ");
		return -3 ;
	}

	*cmd = bFrame[2] ;

	for(unsigned char i=3; i<(ELOAD_CTRL_DATA_LEN+3); i++ )
	{
		pFrame[i-3] = bFrame[i] ;
	}

	//memcpy( pFrame, (bFrame+3), ELOAD_CTRL_DATA_LEN) ;

	return 1 ;
}

static void eload_CheckRxMsg(void)
{
	unsigned char cmd, bFrame[64] ;

	cmd = 0 ;

	int st = eload_readFrame(&cmd , bFrame, sizeof(bFrame)) ;

	if( st ==  1 ) //responce message received;
	{
		switch( cmd )
		{
			case ELOAD_DEV_SN : //device sn ,mode and software version ;
			{

			}
			break ;
		}
	}
}


int ITECH_eLoad_getCurrVolt(void)
{
	return 0 ;
}