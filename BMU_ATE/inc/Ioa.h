#ifndef _IOA_H_
 #define _IOA_H_


#include "Variant_Cfg.h"


#define  IOA_BMU_PWR_ON                  1
#define  IOA_BMU_PWR_OFF                 0

#define  IOA_RELAY_TURN_ON               0
#define  IOA_RELAY_TURN_OFF              1

#define  IOA_SW_DELAY_RELAY_DISABLE      0
#define  IOA_SW_DELAY_RELAY_ENABLE       1


#define IOA_BMU12_POWER_PIN                 11
#define IOA_BMU12_WAKE_PIN                  43
#define IOA_BMU12_FAULT_PIN                 27 //????29

#define IOA_BMU1_MAIN_RELAY_PIN             41  // 41
#define IOA_BMU1_AUX_RELAY_PIN              23
#define IOA_BMU1_OVERCURRENT_PIN            42

#define IOA_BMU2_MAIN_RELAY_PIN             40 // 40
#define IOA_BMU2_AUX_RELAY_PIN              22
#define IOA_BMU2_OVERCURRENT_PIN            39

#define IOA_BMU_UART_SEL_PIN                26
#define IOA_AC_OUT_S2_PIN                   21
#define IOA_AC_OUT_S1_PIN                   28
#define IOA_BEC_RELAY_PIN                   25
#define IOA_WPC_EN_PIN                      27
#define IOA_LED_PWM_PIN                     15


#define IOA_TEST_ST_DOING  					0
#define IOA_TEST_ST_PASS  					1
#define IOA_TEST_ST_FAILD 					2


#define IOA_ATE_KEY_STOP  						0
#define IOA_ATE_KEY_START						1

typedef struct
{
	unsigned char MainRelayCtrl[MAX_PACKET_NUM] ;
	unsigned char MainRelaySt[MAX_PACKET_NUM] ;
	unsigned char AuxRelayCtrl[MAX_PACKET_NUM] ;
	unsigned char AuxRelaySt[MAX_PACKET_NUM] ;

	unsigned char BecRelayCtrl ;
	unsigned char BecRelaySt ;

//	unsigned char AC1RelayCtrl ;
//	unsigned char AC1RelaySt ;
//	unsigned char AC2RelayCtrl ;
//	unsigned char AC2RelaySt ;

	//unsigned short MainRelayOffTime[MAX_PACKET_NUM] ;
	//unsigned short AuxRelayChangeTime ;
}Ioa_CtrlTypes;


typedef struct
{
	unsigned char BmuChipFaultSt ;
	unsigned char HWOverCurrSt ;
}Ioa_FaultTypes;



extern Ioa_FaultTypes ioa_BmuFaultSt[MAX_PACKET_NUM] ;
extern Ioa_CtrlTypes ioa_BmuRelay ;
extern char ioa_test_state ;


extern int Ioa_Init(void) ;
extern void Ioa_Task(void) ;
extern int Ioa_BmuUartChEn(char ChNo) ;


extern int Ioa_BmuPwrOn(void);
extern int Ioa_BmuPwrOff(void);
extern int Ioa_BmuWakeupHigh(void);
extern int Ioa_BmuWakeupLow(void);
extern int Ioa_BmuFaultMonSts(void);
extern unsigned char Ioa_GetBmuRelaySt(unsigned int packetNo);
extern unsigned char Ioa_GetBmuAuxRelaySt(unsigned int packetNo);
extern int Ioa_BmuMainRelayOn( unsigned int packetNo ) ;
extern int Ioa_BmuMainRelayOff( unsigned int packetNo ) ;
extern int Ioa_BmuRelayStFeedback( unsigned int packetNo );
extern unsigned char Ioa_GetBmuAuxRelayAllOnSt(void) ;
extern int Ioa_BecRelayOn(void);
extern int Ioa_BecRelayOff(void) ;
extern int Ioa_BmuAuxRelayOn( unsigned int packetNo ) ;
extern int Ioa_BmuAuxRelayOff( unsigned int packetNo ) ;

extern unsigned char Ioa_IsBmuMainRelayTurnOffCfm( unsigned int packetNo ) ;
extern unsigned char Ioa_IsBmuMainRelayTurnOnCfm( unsigned int packetNo ) ;
extern unsigned char Ioa_IsBmuMainRelayInDelay( unsigned int packetNo ) ;
extern unsigned char Ioa_IsBmuMainRelayProtected( unsigned int packetNo ) ;


extern int Ioa_CheckStartkeySt(void);
extern int Ioa_GetStartkeySt(void);

extern void Ioa_CleanHwProtect(void) ;


#endif

