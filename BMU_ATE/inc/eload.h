#ifndef _E_LOAD_H_
	#define _E_LOAD_H_

#define ITECH_DEV_ELOAD 		0
#define ITECH_DEV_EPOWER 		1


#define ELOAD_BAUD_RATE 		9600


#define ELOAD_CTRL_DATA_LEN  	22
#define ELOAD_ADD 		        0

// frame formate
#define ELOAD_SYNC_HEAD         0xAA
#define ELOAD_ADD_BOARDCAST 	0xFF
#define ELOAD_DEV_SN 		 	0x6A
#define ELOAD_PWR_SN 		 	0x6B


// eLoad command type
#define ELOAD_CMD_CTRL_MODE 					0x20
#define ELOAD_CMD_INPUT_ST   					0x21
#define ELOAD_CMD_PWR_MODE 						0x28
#define ELOAD_CMD_READ_PWR_MODE 				0x29
#define ELOAD_CMD_WRITE_CC 						0x2A
#define ELOAD_CMD_READ_CC 						0x2B
#define ELOAD_CMD_WRITE_CV 						0x2C
#define ELOAD_CMD_READ_CV 						0x2D
#define ELOAD_CMD_READ_VCPS 					0x5F  
#define ELOAD_CMD_SET_CV_LOW_TH_CC 				0xB6 // read 
#define ELOAD_CMD_SET_CV_HIGH_TH_CC 			0xB4 // read 

// ePower command type
#define EPWR_CMD_CTRL_MODE			0x20
#define EPWR_CMD_INPUT_ST   		0x21
#define EPWR_CMD_SET_VOLT 			0x23
#define EPWR_CMD_SET_CURR			0x24
#define EPWR_CMD_READ_PAR 			0x26
#define EPWR_CMD_DEV_SN 			0x31

//command info
#define ELOAD_OP_MODE_PANEL 		0
#define ELOAD_OP_MODE_REMOTE 		1

//e-load work mode:CC,CV,CW,CR
#define ELOAD_IN_CC					0
#define ELOAD_IN_CV 				1
#define ELOAD_IN_CW 				2
#define ELOAD_IN_CR 				3

// e-load read input info
#define ELOAD_READ_INPUT_VOLT 				0
#define ELOAD_READ_INPUT_CURR 				1
#define ELOAD_READ_INPUT_PWR_RATE 			2


extern void eload_Init(void);
extern void eload_Task(void);
extern int ITECH_getDevSn(int dev_type);
extern int eload_SetDCVoltOV(void);
extern int eload_SetDCVoltNormal(void);
extern int eload_SetDCVoltUV(void) ;
extern int ITECH_ePwrSetVolt(bool enable, int volt) ;
extern int ITECH_ePwrSetCurrent(int curr) ;
extern int ITECH_TurnOnDev(bool enable, int dev_type) ;
extern int ITECH_SetRemoteCtrlMode(int dev_type) ;
extern int ITECH_eLoadSetPwrMode(char pwr_mode);
extern int ITECH_eLoadSetPwrPar(char pwr_mode, int pwr_par);
extern int ITECH_eLoadSetMaxCurr(void);
extern int ITECH_eLoad_getCurrVolt(void);

extern int ITECH_eLoad_read_Input_St( int type) ;

#endif
