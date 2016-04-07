#ifndef _ATF_H_
	#define _ATF_H_


/*
#define ATE_TEST_STEP_IDLE 					0
#define ATE_TEST_STEP_SELF_CHECK 			1
#define ATE_TEST_STEP_COM 					2
#define ATE_TEST_STEP_PWR 					3
#define ATE_TEST_STEP_RELAY 				4
*/
#define ATE_TEST_OV_UP					1340
#define ATE_TEST_OV_DW				    1320

#define ATE_TEST_UV_UP					935
#define ATE_TEST_UV_DW				    915

#define ATE_TEST_DEFAULT_VOLT		    900
#define ATE_TEST_PV_CAL_VOLT1 			1200

#define ATF_PV_CFM_VOLT							1120

extern void Atf_Task(void) ;
extern void Atf_Init(void) ;

#endif
