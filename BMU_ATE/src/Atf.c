#include "../inc/Atf.h"
#include "../inc/Ioa.h"
#include "../inc/PL455A.h"
#include "../inc/eload.h"
#include "../inc/net_log.h"
#include "../inc/Variant_Cfg.h"

#define ATF_SELF_CHECK_INIT_FAILD 			0
#define ATF_SELF_CHECK_INIT_DOING 			1
#define ATF_SELF_CHECK_INIT_OK 		 		2

//mian test step  define
#define ATE_TEST_STEP_IDLE 					1
#define ATE_TEST_STEP_SELF_CHECK 			2
#define ATE_TEST_STEP_COM 					3
#define ATE_TEST_STEP_PWR 					4
#define ATE_TEST_STEP_OUV_OC 				5
#define ATE_TEST_PV_CALIBRATE				6
#define ATE_TEST_PC_CALIBRATE 				7
#define ATE_TEST_STEP_OC 					8
#define ATE_TEST_STEP_CHECK_CV_CT 			9
#define ATE_TEST_STEP_CHECK_BAL 			10
#define ATE_TEST_STEP_HW 					11
#define ATE_TEST_PASS 						12
#define ATE_TEST_FAILD 						13

//self check substep index
#define ATF_SC_SUB_STEP_DEV_INIT 			10
#define ATF_SC_SUB_STEP_DEV_SN 				0
#define ATF_SC_SUB_STEP_INIT_EPWR 			1
#define ATF_SC_SUB_STEP_EPWR_READY 			2
#define ATF_SC_SUB_STEP_PL455_INIT 			3
#define ATF_SC_SUB_STEP_DONE 				4
#define ATF_SC_SUB_STEP_FAILD 				5
#define ATF_SC_SUB_STEP_PL455_HAND 			6
#define ATF_SC_SUB_STEP_INIT_ELOAD			7
#define ATF_SC_SUB_STEP_CHECK_ELOAD_CV 		8


//communication check substep index
#define ATF_COMM_SUB_STEP_INIT 				1
#define ATF_COMM_SUB_STEP_DATE 				2
#define ATF_COMM_SUB_STEP_DONE 				3
#define ATF_COMM_SUB_STEP_FAILD 			4

//Pl455 Power check substep index
#define ATF_PWR_SUB_STEP_SLEEP 				1
#define ATF_PWR_SUB_STEP_SLEEP_SAMPLE 		2
#define ATF_PWR_SUB_STEP_WAKEUP 			3
#define ATF_PWR_SUB_STEP_WAKEUP_SAMPLE 		4
#define ATF_PWR_SUB_STEP_DONE 				5
#define ATF_PWR_SUB_STEP_FAILD 				6

// check PL455 sample cell volt & cell temp 
#define ATF_CHECK_CV_SUB_STEP_INIT 			1
#define ATF_CHECK_CV_STEP_SAMPLE_CV 		2
#define ATF_CHECK_CV_STEP_SAMPLE_CT 		4
#define ATF_CHECK_CV_STEP_SAMPLE 			5
#define ATF_CHECK_CV_STEP_DONE 				6
#define ATF_CHECK_CV_STEP_FAILD 			3

//status
#define ATF_SC_ST_PL455_INIT_OK             0x01
#define ATF_SC_ST_PL455_INIT_DOING 			0x02
#define ATF_SC_ST_PL455_INIT_FAILD 			0x04

#define ATF_SC_ST_ELOAD_INIT_OK 			0x08
#define ATF_SC_ST_ELOAD_INIT_DOING 			0x10
#define ATF_SC_ST_ELOAD_INIT_FAILD 			0x20

#define ATF_COMM_ST_PL455_INIT_OK 			0x01
#define ATF_COMM_ST_PL455_INIT_DOING 		0x02
#define ATF_COMM_ST_PL455_INIT_FAILD 		0x04

#define ATF_COMM_ST_DATE_OK 				0x08
#define ATF_COMM_ST_DATE_DOING				0x10
#define ATF_COMM_ST_DATE_FAILD				0x20

#define ATF_PWR_ST_SLEEP 	  				0x01
#define ATF_PWR_ST_SLEEP_SAMPLE_DOING 		0x02
#define ATF_PWR_ST_SLEEP_SAMPLE_OK 			0x04
#define ATF_PWR_ST_SLEEP_SAMPLE_FAILD 		0x08
#define ATF_PWR_ST_WAKEUP 	 				0x10
#define ATF_PWR_ST_WAKEUP_SAMPLE_DOING 		0x20
#define ATF_PWR_ST_WAKEUP_SAMPLE_OK 		0x40
#define ATF_PWR_ST_WAKEUP_SAMPLE_FAILD 		0x80

#define ATF_PWR_ST_DATE_OK  				0x01
#define ATF_PWR_ST_DATE_DOING 				0x02
#define ATF_PWR_ST_DATE_FAILD 				0x04

#define ATF_OUVOC_ST_DATE_OK  				0x01
#define ATF_OUVOC_ST_DATE_DOING 			0x02
#define ATF_OUVOC_ST_DATE_FAILD 			0x04

#define ATF_OUVOC_ST_SLEEP_OK  				0x01
#define ATF_OUVOC_ST_SLEEP_DOING 			0x02
#define ATF_OUVOC_ST_SLEEP_FAILD 			0x04


// packet volt calibrate step index
#define ATF_CALI_PV_SUB_STEP_INIT				1
#define ATF_CALI_PV_SUB_STEP_INIT_EPWR 			2
#define ATF_CALI_PV_SUB_STEP_EPWR_S1 			3
#define ATF_CALI_PV_SUB_STEP_SAMPLE_S1 			4
#define ATF_CALI_PV_SUB_STEP_EPWR_S2			5
#define ATF_CALI_PV_SUB_STEP_SAMPLE_S2 			6
#define ATF_CALI_PV_SUB_STEP_CALI 				7
#define ATF_CALI_PV_SUB_STEP_SAVE 				8
#define ATF_CALI_PV_SUB_STEP_EPWR_S3 			9
#define ATF_CALI_PV_SUB_STEP_UPDATE_CALI 		10
#define ATF_CALI_PV_SUB_STEP_SAMPLE_S3 			11
#define ATF_CALI_PV_SUB_STEP_DONE 				12
#define ATF_CALI_PV_SUB_STEP_END 				13

// packet current calibrate step index
#define ATF_CALI_PC_SUB_STEP_INIT_PL455			1
#define ATF_CALI_PC_SUB_STEP_INIT_ELOAD 		2
#define ATF_CALI_PC_SUB_STEP_ELOAD_S1 			3
#define ATF_CALI_PC_SUB_STEP_SAMPLE_S1 			4
#define ATF_CALI_PC_SUB_STEP_ELOAD_S2			5
#define ATF_CALI_PC_SUB_STEP_SAMPLE_S2 			6
#define ATF_CALI_PC_SUB_STEP_CALI 				7
#define ATF_CALI_PC_SUB_STEP_SAVE 				8
#define ATF_CALI_PC_SUB_STEP_ELOAD_S3 			9
#define ATF_CALI_PC_SUB_STEP_UPDATE_CALI 		10
#define ATF_CALI_PC_SUB_STEP_SAMPLE_S3 			11
#define ATF_CALI_PC_SUB_STEP_ELOAD_S4			12
#define ATF_CALI_PC_SUB_STEP_SAMPLE_S4 			13
#define ATF_CALI_PC_SUB_STEP_ELOAD_S5			14
#define ATF_CALI_PC_SUB_STEP_SAMPLE_S5			15
#define ATF_CALI_PC_SUB_STEP_DONE 				16
#define ATF_CALI_PC_SUB_STEP_END 				17

#define ATF_CALI_PC_SUB_STEP_ELOAD_CHECK_VOLT 	18

//Pl455 hardware OV,UV,OC protect check substep index
#define ATF_OUVOC_SUB_STEP_INIT 			0
#define ATF_OUVOC_SUB_STEP_EPWR_NV 			1
#define ATF_OUVOC_SUB_STEP_SAMPLE_NV 		2
#define ATF_OUVOC_SUB_STEP_CHECK_NV 		3
#define ATF_OUVOC_SUB_STEP_EPWR_OV 			4
#define ATF_OUVOC_SUB_STEP_SAMPLE_OV 		5
#define ATF_OUVOC_SUB_STEP_CHECK_OV 		6
#define ATF_OUVOC_SUB_STEP_EPWR_NV_1 		18
#define ATF_OUVOC_SUB_STEP_SAMPLE_NV_1 		28
#define ATF_OUVOC_SUB_STEP_CHECK_NV_1 		38
#define ATF_OUVOC_SUB_STEP_EPWR_UV 			7
#define ATF_OUVOC_SUB_STEP_SAMPLE_UV 		8
#define ATF_OUVOC_SUB_STEP_CHECK_UV 		9
#define ATF_OUVOC_SUB_STEP_DONE 			10
#define ATF_OUVOC_SUB_STEP_FAILD 			11

//Pl455 hardware OV,UV,OC protect check substep index
#define ATF_OC_SUB_STEP_INIT 			0
#define ATF_OC_SUB_STEP_EPWR_NV 			1
#define ATF_OC_SUB_STEP_SAMPLE_NV 		2
#define ATF_OC_SUB_STEP_CHECK_NV 		3
#define ATF_OC_SUB_STEP_EPWR_OV 			4
#define ATF_OC_SUB_STEP_SAMPLE_OV 		5
#define ATF_OC_SUB_STEP_CHECK_OV 		6
#define ATF_OC_SUB_STEP_EPWR_NV_1 		18
#define ATF_OC_SUB_STEP_SAMPLE_NV_1 		28
#define ATF_OC_SUB_STEP_CHECK_NV_1 		38
#define ATF_OC_SUB_STEP_EPWR_UV 			7
#define ATFVOC_SUB_STEP_SAMPLE_UV 		8
#define ATF_OC_SUB_STEP_CHECK_UV 		9
#define ATF_OC_SUB_STEP_DONE 			10
#define ATF_OC_SUB_STEP_FAILD 			11

#define ATF_OC_SUB_STEP_INIT_PL455 		0
#define ATF_OC_SUB_STEP_INIT_ELOAD		1
#define ATF_OC_SUB_STEP_ELOAD_S1 		2
#define ATF_OC_SUB_STEP_SAMPLE_S1 		3
#define ATF_OC_SUB_STEP_CHECK_S1 		4
#define ATF_OC_SUB_STEP_CHANGE_DIR 		5
#define ATF_OC_SUB_STEP_ELOAD_S2 		6
#define ATF_OC_SUB_STEP_SAMPLE_S2 		7
#define ATF_OC_SUB_STEP_CHECK_S2 		8
#define ATF_OC_SUB_STEP_DONE 			9
#define ATF_OC_SUB_STEP_END 			10

#define ATF_CHECK_BAL_SUB_STEP_INIT 				0
#define ATF_CHECK_BAL_SUB_STEP_SAMPLE_IDLE 			1
#define ATF_CHECK_BAL_SUB_STEP_BAL_ODD 				2
#define ATF_CHECK_BAL_SUB_STEP_ODD_WAIT 			3
#define ATF_CHECK_BAL_SUB_STEP_ODD_COMPARE 			4
#define ATF_CHECK_BAL_SUB_STEP_BAL_EVEN 			5
#define ATF_CHECK_BAL_SUB_STEP_EVEN_WAIT 			6
#define ATF_CHECK_BAL_SUB_STEP_EVEN_COMPARE 		7
#define ATF_CHECK_BAL_SUB_STEP_FAILD 				8
#define ATF_CHECK_BAL_SUB_STEP_DONE 				9


int atf_testStep ;
int atf_itech_dev ;
static int Atf_StartKetSt ;
static int atf_continueMode ; //signle step test or continue test step ;

static int Atf_TestIdle(void);
static int Atf_SelfCheck( bool act, int *errCode ) ;
static int Atf_CheckComm( bool act, int *errCode ) ;
static int Atf_CheckPwr( bool act, int *errCode ) ;
static int Atf_CheckOUVOC( bool act, int *errCode );
static int Atf_Calibrate_PV( bool act, int *errCode ) ;
static int Atf_Calibrate_PC( bool act, int *errCode ) ;
static int Atf_InitPL455( int boardNo, int *time_cnt, int time_out, int *check_st );
static int Atf_Check_OC( bool act, int *errCode );
static int Atf_Check_CellVolt( bool act, int *errCode );
static int Atf_Check_CellTemp( bool act, int *errCode );
static int Atf_Check_Balance( bool act, int *errCode );


void Atf_Init(void)
{

	atf_continueMode = 1 ;
	atf_testStep = ATE_TEST_STEP_IDLE ;
	//atf_itech_dev = ITECH_DEV_EPOWER ;
	//atf_itech_dev = ITECH_DEV_ELOAD ;
///////////////////////////////////////////////////////////////////////////////////
	atf_itech_dev = ITECH_DEV_ELOAD ; //  debug 
	return ;
///////////////////////////////////////////////////////////////////////////////////
	int file_st = open_cfg("BMU_ATE.cfg", READ_MODULE_F) ;

	if( file_st >=  0 ) // the configure file is exist
	{
		atf_itech_dev =  get_cfg_int("BmuAteCfg","Type", 2) ;
		if( atf_itech_dev >= 2 )
		{
			printf("\n\nBMU ATE configure file has rong parameter...................\n");
			while(1) ;
		}
	}
	else
	{
		printf("\n\nThis has no BMU ATE configure file....................\n");
		while(1) ;
	}

}

void Atf_Task(void)
{
	// Current sample calibrate device
	// 1. self check ;
	// 2. communication ;
	// 3. cell volt & cell temperature check
	// 4. balance 
	// 5. currernt calibrate
	// 6. oc protect check ;


	// Current sample calibrate device
	// 1. self check ;
	// 2. communication ;
	// 3. power check
	// 4. packet voltage calibrate
	// 5. PV protect check ;
	// 6. Hardware vresion check ; 

	// 3. 
	if( Atf_StartKetSt != Ioa_GetStartkeySt() )
	{
		Atf_StartKetSt = Ioa_GetStartkeySt() ;
		
		if( Atf_StartKetSt == IOA_ATE_KEY_START )
		{
			atf_testStep = ATE_TEST_STEP_SELF_CHECK ;
		}
		else
		{
			atf_testStep = ATE_TEST_STEP_IDLE ;
		}
	}
	else
	{
		if( Atf_StartKetSt == IOA_ATE_KEY_STOP )
		{
			atf_testStep = ATE_TEST_STEP_IDLE ;
		}
	}

		switch( atf_testStep )
		{
			case ATE_TEST_STEP_IDLE:
			{
				int tmp ;
				//Atf_TestIdle();
				Atf_SelfCheck(false, &tmp) ;
				//ITECH_TurnOnDev( false, atf_itech_dev);

				//LOG_I("<ATF> - Step:IDLE.");
			}
			break;

			case ATE_TEST_STEP_SELF_CHECK :
			{

				int selfcheckCode ;
				int st = Atf_SelfCheck(true, &selfcheckCode) ;

				if( st == 1)
				{
					// self check passed .
					if( atf_continueMode == 1 )
					{
						atf_testStep = ATE_TEST_STEP_COM ;
					}
					else
					{
						atf_testStep = ATE_TEST_PASS ;
					}
					LOG_I("<ATF> - [main Step] - [Self Check] ==== Ok.%d.\n", selfcheckCode );
					Atf_SelfCheck(false, &selfcheckCode) ;// reset selfcheck status
				}
				else if( st == -1 )
				{
					//self check faild.
					atf_testStep = ATE_TEST_FAILD ;
					LOG_I("<ATF> - [main Step] - [Self Check] ==== Faild.%d.\n", selfcheckCode );
					Atf_SelfCheck(false, &selfcheckCode) ; // reset selfcheck status
					//atf_testStep = ATE_TEST_STEP_COM ;

				}
				else
				{
					//check is doing
					//continue
					static int self_check_mask = 0xFFFF ;
					if( selfcheckCode != self_check_mask )
					{
						LOG_I("<ATF> - [main Step] - [Self Check] ==== Doing.%d.\n", selfcheckCode );
						self_check_mask = selfcheckCode ;
					}
				}

			}
			break;

			// start real test from here.
			case ATE_TEST_STEP_COM :
			{
				int commCheckCode ;
				int st = Atf_CheckComm(true, &commCheckCode) ;

				if( st == 1)
				{
					LOG_I("<ATF> - [main Step] - [communication Check] ==== Ok.%d.\n", commCheckCode );

					// self check passed .
					if( atf_continueMode == 1 )
					{
						
						if( atf_itech_dev == ITECH_DEV_ELOAD )
						{
							//atf_testStep = ATE_TEST_PC_CALIBRATE ;
							//LOG_I("<ATF> - [main Step] - [PC Calibrate] ==== start..........." );
							atf_testStep = ATE_TEST_STEP_CHECK_CV_CT ;
							LOG_I("<ATF> - [main Step] - [Cell Volt & Cell Temp] ==== start..........." );							
						}
						else if( atf_itech_dev == ITECH_DEV_EPOWER )
						{
							//atf_testStep = ATE_TEST_PV_CALIBRATE ;
							//LOG_I("<ATF> - [main Step] - [PV Calibrate] ==== start............." );
							//atf_testStep =  ATE_TEST_STEP_OUV_OC ;
							atf_testStep = ATE_TEST_STEP_PWR ;
						}
					}
					else
					{
						atf_testStep = ATE_TEST_PASS ;
					}


					Atf_CheckComm(false, &commCheckCode) ;// reset selfcheck status
				}
				else if( st == -1 )
				{
					//self check faild.
					atf_testStep = ATE_TEST_FAILD ;

					LOG_I("<ATF> - [main Step] - [communication Check] ==== faild.%d.\n", commCheckCode );
					Atf_CheckComm(false, &commCheckCode) ; // reset selfcheck status
				}
				else
				{
					//check is doing
					//continue

					static int com_check_mask = 0xFFFF ;
					if( commCheckCode != com_check_mask )
					{
						LOG_I("<ATF> - [main Step] - [communication Check] ==== Doing.%d.\n", commCheckCode );
						com_check_mask = commCheckCode ;
					}

				}
			}
			break;

			case ATE_TEST_STEP_CHECK_CV_CT :
			{
				int cv_ct_check_code ;
				int st = Atf_Check_CellVolt(true, &cv_ct_check_code) ;

				if( st == 1)
				{
					// self check passed .
					if( atf_continueMode == 1 )
					{
						//atf_testStep = ATE_TEST_STEP_PWR ;
						atf_testStep = ATE_TEST_STEP_CHECK_BAL ;
					}
					else
					{
						atf_testStep = ATE_TEST_PASS ;
					}

					LOG_I("<ATF> - [main Step] - [CV CT check] ==== Ok.%d.", cv_ct_check_code );
					Atf_Check_OC(false, &cv_ct_check_code) ;// reset selfcheck status
				}
				else if( st == -1 )
				{
					//self check faild.
					atf_testStep = ATE_TEST_FAILD ;

					LOG_I("<ATF> - [main Step] - [CV CT check] ==== faild.%d.", cv_ct_check_code );
					Atf_Check_OC(false, &cv_ct_check_code) ; // reset selfcheck status
				}
				else
				{
					//check is doing
					//continue
					static int cv_ct_check_mask = 0xFFFF ;
					if( cv_ct_check_code != cv_ct_check_mask )
					{
						LOG_I("<ATF> - [main Step] - [CV CT check] ==== Doing.%d.", cv_ct_check_code );
						cv_ct_check_mask = cv_ct_check_code ;
					}
				}

			}
			break ;

			case ATE_TEST_STEP_CHECK_BAL :
			{
				int bal_Check_Code ;
				int st = Atf_Check_Balance(true, &bal_Check_Code) ;

				if( st == 1)
				{
					// self check passed .
					if( atf_continueMode == 1 )
					{
						//atf_testStep = ATE_TEST_STEP_PWR ;
					}
					else
					{
						atf_testStep = ATE_TEST_PASS ;
					}

					LOG_I("<ATF> - [main Step] - [Balance] ==== Ok.%d.", bal_Check_Code );
					Atf_Check_Balance(false, &bal_Check_Code) ;// reset selfcheck status
				}
				else if( st == -1 )
				{
					//self check faild.
					atf_testStep = ATE_TEST_FAILD ;

					LOG_I("<ATF> - [main Step] - [Balance] ==== faild.%d.", bal_Check_Code );
					Atf_Check_Balance(false, &bal_Check_Code) ; // reset selfcheck status
				}
				else
				{
					//check is doing
					//continue
					static int bal_Check_mask = 0xFFFF ;
					if( bal_Check_Code != bal_Check_mask )
					{
						LOG_I("<ATF> - [main Step] - [Balance] ==== Doing.%d.", bal_Check_Code );
						bal_Check_mask = bal_Check_Code ;
					}
				}

			}
			break ;

			case ATE_TEST_STEP_PWR :
			{
				int pwrCheckCode ;
				int st = Atf_CheckPwr(true, &pwrCheckCode) ;

				if( st == 1)
				{
					// self check passed .
					if( atf_continueMode == 1 )
					{
						atf_testStep = ATE_TEST_PV_CALIBRATE ;
						LOG_I("<ATF> - [main Step] - [PV Calibrate] ==== start............." );

					}
					else
					{
						atf_testStep = ATE_TEST_PASS ;
					}

					LOG_I("<ATF> - [main Step] - [target board power Check] ==== Ok.%d.", pwrCheckCode );
					Atf_CheckPwr(false, &pwrCheckCode) ;// reset selfcheck status
				}
				else if( st == -1 )
				{
					//self check faild.
					atf_testStep = ATE_TEST_FAILD ;

					LOG_I("<ATF> - [main Step] - [target board power Check] ==== faild.%d.", pwrCheckCode );
					Atf_CheckPwr(false, &pwrCheckCode) ; // reset selfcheck status
				}
				else
				{
					//check is doing
					//continue
					static int pwr_check_mask = 0xFFFF ;
					if( pwrCheckCode != pwr_check_mask )
					{
						LOG_I("<ATF> - [main Step] - [target board power Check] ==== Doing.%d.", pwrCheckCode );
						pwr_check_mask = pwrCheckCode ;
					}
				}
			}
			break ;

			case ATE_TEST_STEP_OUV_OC :
			{
				int ouv_oc_check_code ;
				int st = Atf_CheckOUVOC(true, &ouv_oc_check_code) ;

				if( st == 1)
				{
					// self check passed .
					if( atf_continueMode == 1 )
					{
						//atf_testStep = ATE_TEST_STEP_PWR ;
						atf_testStep = ATE_TEST_STEP_HW ;
					}
					else
					{
						atf_testStep = ATE_TEST_PASS ;
					}

					LOG_I("<ATF> - [main Step] - [OU-OV-OC check] ==== Ok.%d.", ouv_oc_check_code );
					Atf_CheckOUVOC(false, &ouv_oc_check_code) ;// reset selfcheck status
				}
				else if( st == -1 )
				{
					//self check faild.
					atf_testStep = ATE_TEST_FAILD ;

					LOG_I("<ATF> - [main Step] - [OU-OV-OC check] ==== faild.%d.", ouv_oc_check_code );
					Atf_CheckOUVOC(false, &ouv_oc_check_code) ; // reset selfcheck status
				}
				else
				{
					//check is doing
					//continue
					static int ouv_oc_check_mask = 0xFFFF ;
					if( ouv_oc_check_code != ouv_oc_check_mask )
					{
						LOG_I("<ATF> - [main Step] - [OU-OV-OC check] ==== Doing.%d.", ouv_oc_check_code );
						ouv_oc_check_mask = ouv_oc_check_code ;
					}
				}

			}
			break;

			case ATE_TEST_PV_CALIBRATE :
			{
				int caliPvCheckCode ;
				int st = Atf_Calibrate_PV(true, &caliPvCheckCode) ;

				if( st == 1)
				{

					LOG_I("<ATF> - [main Step] - [PV Calibrate] ==== Ok.%d.", caliPvCheckCode );

					// self check passed .
					if( atf_continueMode == 1 )
					{
						atf_testStep = ATE_TEST_STEP_OUV_OC ;
						LOG_I("\n<ATF> - [main Step] - [OU-OV-OC check] ==== start..........." );
					}
					else
					{
						atf_testStep = ATE_TEST_PASS ;
					}

					Atf_Calibrate_PV(false, &caliPvCheckCode) ;// reset selfcheck status
				}
				else if( st == -1 )
				{
					//self check faild.
					atf_testStep = ATE_TEST_FAILD ;

					LOG_I("<ATF> - [main Step] - [PV Calibrate] ==== faild.%d.", caliPvCheckCode );
					Atf_Calibrate_PV(false, &caliPvCheckCode) ; // reset selfcheck status
				}
				else
				{
					//check is doing
					//continue
					static int pv_cali_check_mask = 0xFFFF ;
					if( caliPvCheckCode != pv_cali_check_mask )
					{
						LOG_I("<ATF> - [main Step] - [PV Calibrate] ==== Doing.%d.", caliPvCheckCode );
						pv_cali_check_mask = caliPvCheckCode ;
					}
				}

			}
			break;

			case ATE_TEST_PC_CALIBRATE :
			{
				int pc_check_code ;
				int st = Atf_Calibrate_PC(true, &pc_check_code) ;

				if( st == 1)
				{
					// self check passed .
					if( atf_continueMode == 1 )
					{
						//atf_testStep = ATE_TEST_STEP_PWR ;
						atf_testStep = ATE_TEST_STEP_OC ;
					}
					else
					{
						atf_testStep = ATE_TEST_PASS ;
					}

					LOG_I("<ATF> - [main Step] - [PC Calibrate] ==== Ok.%d.", pc_check_code );
					Atf_Calibrate_PC(false, &pc_check_code) ;// reset selfcheck status
				}
				else if( st == -1 )
				{
					//self check faild.
					atf_testStep = ATE_TEST_FAILD ;

					LOG_I("<ATF> - [main Step] - [PC Calibrate] ==== faild.%d.", pc_check_code );
					Atf_Calibrate_PC(false, &pc_check_code) ; // reset selfcheck status
				}
				else
				{
					//check is doing
					//continue
					static int pc_cali_check_mask = 0xFFFF ;
					if( pc_check_code != pc_cali_check_mask )
					{
						LOG_I("<ATF> - [main Step] - [PC Calibrate] ==== Doing.%d.", pc_check_code );
						pc_cali_check_mask = pc_check_code ;
					}
				}

			}
			break;

			case ATE_TEST_STEP_OC :
			{
				int oc_check_code ;
				int st = Atf_Check_OC(true, &oc_check_code) ;

				if( st == 1)
				{
					// self check passed .
					if( atf_continueMode == 1 )
					{
						//atf_testStep = ATE_TEST_STEP_PWR ;
						atf_testStep = ATE_TEST_PASS ;
					}
					else
					{
						atf_testStep = ATE_TEST_PASS ;
					}

					LOG_I("<ATF> - [main Step] - [OC check] ==== Ok.%d.", oc_check_code );
					Atf_Check_OC(false, &oc_check_code) ;// reset selfcheck status
				}
				else if( st == -1 )
				{
					//self check faild.
					atf_testStep = ATE_TEST_FAILD ;

					LOG_I("<ATF> - [main Step] - [OC check] ==== faild.%d.", oc_check_code );
					Atf_Check_OC(false, &oc_check_code) ; // reset selfcheck status
				}
				else
				{
					//check is doing
					//continue
					static int oc_check_mask = 0xFFFF ;
					if( oc_check_code != oc_check_mask )
					{
						LOG_I("<ATF> - [main Step] - [OU-OV-OC check] ==== Doing.%d.", oc_check_code );
						oc_check_mask = oc_check_code ;
					}
				}

			}
			break;

			case ATE_TEST_STEP_HW :
			{
				PL455_Set_HWVersion( ATE_TEST_BOARD_IDX) ;
				atf_testStep = ATE_TEST_PASS ;
			}
			break ;

			case ATE_TEST_FAILD:
			{

			}
			break;

			case ATE_TEST_PASS:
			{

			}
			break;

			default:
			break;
		}


		if (atf_testStep == ATE_TEST_FAILD)
		{
			ioa_test_state = IOA_TEST_ST_FAILD ;
		}
		else if (atf_testStep == ATE_TEST_PASS)
		{
			ioa_test_state = IOA_TEST_ST_PASS ;
		}
		else
		{
			ioa_test_state = IOA_TEST_ST_DOING ;
		}

		//LOG_I(" ATF test state=%d.-test state=%d.", atf_testStep, ioa_test_state) ;
}


void Atf_TestStop(void)
{
	//Ioa_TestStopEn();
	//atf_testStep = ATE_TEST_STEP_IDLE ;
}


int Atf_SelfCheck( bool act, int *errCode )
{
	static int slef_check_substep = ATF_SC_SUB_STEP_DEV_INIT ;
	static int slef_check_cnt = 0 ;
	static int checkSt = 0xFF;

	if(act == 0 ) // reset the self check status
	{
		slef_check_substep = ATF_SC_SUB_STEP_DEV_INIT ;
		slef_check_cnt = 0 ;
		checkSt = 0xFF;
		*errCode = 0 ;
		return 0 ;
	}

	switch( slef_check_substep )
	{
		case ATF_SC_SUB_STEP_DEV_INIT :
		{
			ITECH_TurnOnDev( true, atf_itech_dev);
			slef_check_substep = ATF_SC_SUB_STEP_DEV_SN ;
			//slef_check_substep = ATF_SC_SUB_STEP_PL455_HAND ;
		}
		break ;
		
		case ATF_SC_SUB_STEP_DEV_SN :
		{

			int ret = ITECH_getDevSn(atf_itech_dev) ;

			if( ret == 1 )
			{
				checkSt = ATF_SC_ST_ELOAD_INIT_OK ;
				if( atf_itech_dev == ITECH_DEV_EPOWER )
				{
					slef_check_substep = ATF_SC_SUB_STEP_INIT_EPWR ;
				}
				else if( atf_itech_dev == ITECH_DEV_ELOAD )
				{
					slef_check_substep = ATF_SC_SUB_STEP_INIT_ELOAD ;
				}
				else
				{
					//
				}

				//slef_check_substep = ATF_SC_SUB_STEP_DEV_INIT ;
				slef_check_cnt = 0 ;
				LOG_I("<ATF> - [Self Check] -- Get eload Type OK.");
			}
			else
			{
				checkSt = ATF_SC_ST_ELOAD_INIT_DOING ;
				slef_check_cnt++ ;
			}

			if( slef_check_cnt >= 10)
			{
				checkSt = ATF_SC_ST_ELOAD_INIT_FAILD ;
				slef_check_substep = ATF_SC_SUB_STEP_FAILD ;
				slef_check_cnt = 0 ;
				LOG_I("<ATF> - [Self Check] -- Get eload Type - Faild.");
			}

			if( slef_check_cnt == 1)
			{
				LOG_I("<ATF> - [Self Check] -- Get eload Type -  Doing.");
			}
		}
		break ;

		case ATF_SC_SUB_STEP_INIT_EPWR :
		{
			int ret_m = ITECH_SetRemoteCtrlMode(ITECH_DEV_EPOWER);
			int ret_c = ITECH_eLoadSetPwrPar(ELOAD_IN_CC, 0) ; // 1500
			
			if( (ret_m >= 0) && (ret_c >= 0) )
			{
				slef_check_substep = ATF_SC_SUB_STEP_EPWR_READY ;
				slef_check_cnt = 0 ;
			}

			if( slef_check_cnt >= 10 )
			{
				slef_check_substep = ATF_SC_SUB_STEP_FAILD ;
				slef_check_cnt = 0 ;
			}
			else
			{
				slef_check_cnt++ ;
			}

			if( ret_m >= 0)
			{
				LOG_I("<ePower> - set ePower work to remote mode ok.");
			}
			else
			{
				LOG_I("<ePower> - set ePower work to remote mode faild.");
			}
		}
		break ;

		case ATF_SC_SUB_STEP_EPWR_READY :
		{
			int ret = ITECH_ePwrSetVolt(true, ATE_TEST_DEFAULT_VOLT); //90V
			//if( ret == 1 )
			if( ret > 0 )
			{
				ITECH_TurnOnDev( true, ITECH_DEV_EPOWER);
				//ITECH_ePwrSetVolt(false, 0xFF ) ;

				slef_check_substep = ATF_SC_SUB_STEP_PL455_HAND ;
				slef_check_cnt = 0 ;
			}
		}
		break ;


		case ATF_SC_SUB_STEP_INIT_ELOAD :
		{
			int ret_m  = ITECH_SetRemoteCtrlMode(ITECH_DEV_ELOAD);
			int ret_c  = ITECH_eLoadSetPwrMode(ELOAD_IN_CC) ; //set eload in cc mode
			int ret_sc = ITECH_eLoadSetPwrPar(ELOAD_IN_CC, 1500) ; // 1500
			
			ITECH_TurnOnDev( true, ITECH_DEV_ELOAD);

			if( (ret_m >= 0) && (ret_c >= 0) && (ret_sc >= 0) )
			{
				slef_check_substep = ATF_SC_SUB_STEP_CHECK_ELOAD_CV ;
				slef_check_cnt = 0 ;
			}

			if( slef_check_cnt >= 10 )
			{
				slef_check_substep = ATF_SC_SUB_STEP_FAILD ;
				slef_check_cnt = 0 ;
			}
			else
			{
				slef_check_cnt++ ;
			}

			if( ret_m >= 0)
			{
				LOG_I("<ePower> - set eload work to remote mode ok.");
			}
			else
			{
				LOG_I("<ePower> - set eload work to remote mode faild.");
			}

			if( ret_c >= 0)
			{
				LOG_I("<ePower> - set eLoad work in constance current mode.");
			}
		}
		break ;

		case ATF_SC_SUB_STEP_CHECK_ELOAD_CV :
		{
			#define ELOAD_CV_DIFF_TH 			50  // 0.1 V
			#define ELOAD_CV_DIFF_CFM_CNT 		4   //  
			#define ELOAD_CV_LOW_ALM_TH 		3400  // 3.4 V

			static int last_cv_volt, cv_sample_cfm_cnt = 0 ;

			int volt = ITECH_eLoad_read_Input_St(ELOAD_READ_INPUT_VOLT);
			int diff = volt - last_cv_volt ;
			last_cv_volt = volt ;
			if( (diff <= ELOAD_CV_DIFF_TH) &&(volt != -1) )
			{
				cv_sample_cfm_cnt++ ;

				if( cv_sample_cfm_cnt > ELOAD_CV_DIFF_CFM_CNT)
				{
					ITECH_TurnOnDev( false, ITECH_DEV_ELOAD);
					cv_sample_cfm_cnt = 0 ; 
					slef_check_cnt = 0 ;
		
					if( last_cv_volt >= ELOAD_CV_LOW_ALM_TH )
					{
						//cell volt normal
						slef_check_substep = ATF_SC_SUB_STEP_PL455_HAND ;
						LOG_I("<ATF> - [self check] --  cells volt(provide for current source).%d. ", last_cv_volt);
					}
					else
					{
						LOG_I("\n.<ATF> - [self check] -- Please do charge the cells provide for current source. ");
						slef_check_substep = ATF_SC_SUB_STEP_FAILD ;
					}
				}

			}
			else
			{
				cv_sample_cfm_cnt = 0 ;
			}

			//LOG_I("<ATF> - [self check] -- diff low =%d.  %d. ",diff, volt);

			slef_check_cnt++ ;

			if( slef_check_cnt >= 40)
			{
				checkSt = ATF_SC_ST_PL455_INIT_FAILD ;
				slef_check_substep = ATF_SC_SUB_STEP_FAILD ;
				slef_check_cnt = 0 ;
				ITECH_TurnOnDev( false, ITECH_DEV_ELOAD);
				LOG_I("<ATF> - [Self Check] -- Check CV - Faild.");
			}

			if( slef_check_cnt == 3)
			{
				LOG_I("<ATF> - [Self Check] -- Check CV  Doing.");
			}
			
		}
		break ;

		case ATF_SC_SUB_STEP_PL455_HAND :
		{
			if( atf_itech_dev == ITECH_DEV_EPOWER)
			{
				//PL455_TestBoardInit(ATE_TEST_BOARD_IDX);
			}

			slef_check_substep = ATF_SC_SUB_STEP_PL455_INIT ;
			slef_check_cnt = 0 ;
		}
		break ;

		case ATF_SC_SUB_STEP_PL455_INIT :
		{
			PL455_Init(ATE_STD_BOARD_IDX);

			int st = PL455_GetInitSt(ATE_STD_BOARD_IDX);
			if( st == PL455_INIT_ST_OK ) // init successded.
			{
				checkSt = ATF_SC_ST_PL455_INIT_OK ;
				slef_check_substep = ATF_SC_SUB_STEP_DONE ;
				slef_check_cnt = 0 ;
				LOG_I("<ATF> - [Self Check] -- Init Standard BMU OK.");
			}
			else
			{
				checkSt = ATF_SC_ST_PL455_INIT_DOING ;
				slef_check_cnt++ ; //try again ;
			}

			if( slef_check_cnt >= 10)
			{
				checkSt = ATF_SC_ST_PL455_INIT_FAILD ;
				slef_check_substep = ATF_SC_SUB_STEP_FAILD ;
				slef_check_cnt = 0 ;
				LOG_I("<ATF> - [Self Check] -- Init Standard BMU - Faild.");
			}

			if( slef_check_cnt == 1)
			{
				LOG_I("<ATF> - [Self Check] -- Init Standard BMU Doing.");
			}

		}
		break ;


		case ATF_SC_SUB_STEP_DONE :
		{
			slef_check_cnt = 0 ;
			//slef_check_substep = 0 ;
		}
		break ;

		case ATF_SC_SUB_STEP_FAILD :
		{
			slef_check_cnt = 0 ;
			//slef_check_substep = 0 ;
		}
		break ;

		default:
			checkSt = 0 ;
		break;
	}

	//Init standard BMU board
	if( slef_check_substep == ATF_SC_SUB_STEP_DONE )
	{
		*errCode = 0 ;
		return 1 ;
	}
	else if( slef_check_substep == ATF_SC_SUB_STEP_FAILD )
	{
		*errCode = checkSt ;
		return -1 ;
	}
	else
	{
		*errCode = checkSt ;
		return 0 ;
	}


}




int Atf_CheckComm( bool act, int *errCode )
{
	static int comm_substep = ATF_COMM_SUB_STEP_INIT ;
	static int comm_cnt = 0 ;
	static int comm_checkSt = 0xFF;

	if( act == 0 ) // reset the self check status
	{
		comm_substep = ATF_COMM_SUB_STEP_INIT ;
		comm_cnt = 0 ;
		comm_checkSt = 0xFF;
		*errCode = 0 ;
		return 0 ;
	}

	switch( comm_substep )
	{
		case ATF_COMM_SUB_STEP_INIT:
		{

			PL455_Init(ATE_TEST_BOARD_IDX);
			int st = PL455_GetInitSt(ATE_TEST_BOARD_IDX);

			if( st == PL455_INIT_ST_OK ) // init successded.
			{
				comm_checkSt = ATF_COMM_ST_PL455_INIT_OK ;
				//comm_substep = ATF_COMM_SUB_STEP_DATE ;
				comm_substep = ATF_COMM_SUB_STEP_DONE ;
				comm_cnt = 0 ;
				LOG_I("<ATF> - [com Check] -- Test BMU Board -- PL455 Init - OK.");
			}
			else
			{
				comm_checkSt = ATF_COMM_ST_PL455_INIT_DOING ;
				comm_cnt++ ; //try again ;
			}

			if( comm_cnt >= 10)
			{
				comm_checkSt = ATF_COMM_ST_PL455_INIT_FAILD ;
				comm_substep = ATF_COMM_SUB_STEP_FAILD ;
				comm_cnt = 0 ;
				LOG_I("<ATF> - [com Check] -- Test BMU Board -- PL455 Init - FAILD.");
			}

			if ( comm_cnt == 1 )
			{
				LOG_I("<ATF> - [com Check] -- Test BMU Board -- PL455 Init - Doing.");
			}

		}
		break ;

		case ATF_COMM_SUB_STEP_DATE :
		{
			int st = PL455_Task(ATE_TEST_BOARD_IDX);

			LOG_I("<ATF> - [com Check] -- ATF_COMM_SUB_STEP_DATE - %d.",st);

			if( st == 1 ) // init successded.
			{
				comm_checkSt = ATF_COMM_ST_DATE_OK ;
				comm_substep = ATF_COMM_SUB_STEP_DONE ;
				comm_cnt = 0 ;
				LOG_I("<ATF> - [com Check] -- Test BMU Board -- PL455 Sample - Ok.");
			}
			else
			{
				comm_checkSt = ATF_COMM_ST_DATE_DOING ;
				comm_cnt++ ; //try again ;
			}

			if( comm_cnt >= 40 )
			{
				comm_checkSt = ATF_COMM_ST_DATE_FAILD ;
				comm_substep = ATF_COMM_SUB_STEP_FAILD ;
				comm_cnt = 0 ;
				LOG_I("<ATF> - [com Check] -- Test BMU Board -- PL455 Sample - FAILD.");
			}

			if ( comm_cnt == 1 )
			{
				LOG_I("<ATF> - [com Check] -- Test BMU Board -- PL455 Sample - Doing.");
			}

		}
		break ;

		case ATF_COMM_SUB_STEP_DONE :
		{

		}
		break ;

		case ATF_COMM_SUB_STEP_FAILD :
		{

		}
		break ;

		default:
		{
			comm_checkSt = 0 ;
		}
		break;
	}

	//Init standard BMU board
	if( comm_substep == ATF_COMM_SUB_STEP_DONE )
	{
		*errCode = 0 ;
		return 1 ;
	}
	else if( comm_substep == ATF_COMM_SUB_STEP_FAILD )
	{
		*errCode = comm_checkSt ;
		return -1 ;
	}
	else
	{
		*errCode = comm_checkSt ;
		return 0 ;
	}


}

int Atf_CheckPwr( bool act, int *errCode )
{
	// 1. sleep test target PL455 ;
	// 2. check test target PL455 +10V & VP to confirm PL455 entry sleep mode ;
	// 3. wakeup test target PL455 ; 
	// 4. check test target PL455 +10V & VP to confirm PL455 entry wakeup mode ;

	static int pwr_substep = 0 ;
	static int pwr_cnt = 0 ;
	static int pwr_checkSt = 0xFF;

	if( act == 0 ) // reset the self check status
	{
		pwr_substep = 0 ;
		pwr_cnt = 0 ;
		pwr_checkSt = 0xFF;
		*errCode = 0 ;
		return 0 ;
	}

	switch( pwr_substep )
	{
		case  0:
		{
			// for PL455 init
			PL455_Init(ATE_STD_BOARD_IDX);
			pwr_substep = ATF_PWR_SUB_STEP_WAKEUP ;
		}
		break ;

		case ATF_PWR_SUB_STEP_SLEEP:
		{
			PL455_Sleep(ATE_TEST_BOARD_IDX);
			//PL455_WakeUp(ATE_TEST_BOARD_IDX);
			pwr_checkSt = ATF_PWR_ST_SLEEP ;
			pwr_substep = ATF_PWR_SUB_STEP_SLEEP_SAMPLE ;
			pwr_cnt = 0 ;
			LOG_I("<ATF> - [test board Check] -- test board PL455 sleep - OK.");
		}
		break ;

		case ATF_PWR_SUB_STEP_SLEEP_SAMPLE:
		{
			#define REF_IDLE_CFM_CNT    	10 
			#define REF_IDLE_TIMEOUT_CNT    5000

			static int ref_sample_cnt = 0 ;
			
			PL455_Task(ATE_STD_BOARD_IDX);

			int st = PL455_IsPowerIdle(ATE_TEST_BOARD_IDX);

			if( st != 1 ) // power idle error
			{
				ref_sample_cnt = 0 ;	
			}
			else // power idle normal,deonce
			{
				ref_sample_cnt++ ;

				if( ref_sample_cnt >= REF_IDLE_CFM_CNT )
				{
					ref_sample_cnt = 0 ;

					pwr_checkSt = ATF_PWR_ST_SLEEP_SAMPLE_OK ;
					pwr_substep = ATF_PWR_SUB_STEP_WAKEUP ;
					pwr_cnt = 0 ;
					LOG_I("<ATF> - [Power Check] -- Test board PL455 sleep sample - OK.");
				}
			}

			pwr_cnt++ ; //try again ;

			if( pwr_cnt == 1 )
			{
				pwr_checkSt = ATF_PWR_ST_SLEEP_SAMPLE_DOING ;
				LOG_I("<ATF> - [Power Check] -- Test board PL455 sleep sample - Doing.");
			}
			else if( pwr_cnt >= REF_IDLE_TIMEOUT_CNT)
			{
				pwr_checkSt = ATF_PWR_ST_SLEEP_SAMPLE_FAILD ;
				pwr_substep = ATF_PWR_SUB_STEP_FAILD ;
				pwr_cnt = 0 ;
				LOG_I("<ATF> - [Power Check] -- Test board PL455 sleep - FAILD.");
			}

		}
		break ;


		case ATF_PWR_SUB_STEP_WAKEUP :
		{
			PL455_WakeUp(ATE_TEST_BOARD_IDX);

			pwr_checkSt = ATF_PWR_ST_WAKEUP ;
			pwr_substep = ATF_PWR_SUB_STEP_WAKEUP_SAMPLE ;
			pwr_cnt = 0 ;
			LOG_I("<ATF> - [Power Check] -- test board PL455 sleep - OK.");
		}
		break ;

		case ATF_PWR_SUB_STEP_WAKEUP_SAMPLE:
		{
			#define REF_IDLE_CFM_CNT    	10 
			#define REF_IDLE_TIMEOUT_CNT    50

			static int ref_sample_cnt = 0 ;
			
			PL455_Task(ATE_STD_BOARD_IDX);

			int st = PL455_IsPowerNormal(ATE_TEST_BOARD_IDX);

			if( st != 1 ) 
			{
				ref_sample_cnt = 0 ;			
			}
			else
			{
				ref_sample_cnt++ ;

				if( ref_sample_cnt >= REF_IDLE_CFM_CNT )
				{
					ref_sample_cnt = 0 ;

					pwr_checkSt = ATF_PWR_ST_WAKEUP_SAMPLE_OK ;
					pwr_substep = ATF_PWR_SUB_STEP_DONE ;
					pwr_cnt = 0 ;
					LOG_I("<ATF> - [Power Check] -- Test board PL455 -- Ref-Volt noraml.");
				}
			}
			//LOG_I("<ATF> - [Power Check] -- Test board PL455 -- Ref-Volt noraml.=%d.", st);
			pwr_cnt++ ; //try again ;

			if( pwr_cnt == 1 )
			{
				pwr_checkSt = ATF_PWR_ST_WAKEUP_SAMPLE_DOING ;
				LOG_I("<ATF> - [Power Check] -- Test board PL455 wakeup - Doing.");
			}
			else if( pwr_cnt >= REF_IDLE_TIMEOUT_CNT)
			{
				pwr_checkSt = ATF_PWR_ST_WAKEUP_SAMPLE_FAILD ;
				pwr_substep = ATF_PWR_SUB_STEP_FAILD ;
				pwr_cnt = 0 ;
				LOG_I("<ATF> - [Power Check] -- Test board PL455 - Ref-Volt error.");
			}

			//LOG_I("<ATF> - [Power Check] -- Test board PL455 - -----------------------------.");
		}
		break ;

		case ATF_PWR_SUB_STEP_DONE :
		{

		}
		break ;

		case ATF_PWR_SUB_STEP_FAILD :
		{

		}
		break ;

		default:
		{
			pwr_checkSt = 0 ;
		}
		break;
	}

	//Init standard BMU board
	if( pwr_substep == ATF_PWR_SUB_STEP_DONE )
	{
		*errCode = 0 ;
		return 1 ;
	}
	else if( pwr_substep == ATF_PWR_SUB_STEP_FAILD )
	{
		*errCode = pwr_checkSt ;
		return -1 ;
	}
	else
	{
		*errCode = pwr_checkSt ;
		return 0 ;
	}


}

int Atf_CheckOUVOC( bool act, int *errCode )
{
	// check packet voltage over/under protect function
	// step 1: output 115V ; and turn on Test BMU Board main relay
	// step 2: use BMU to confirm the 115 voltage ;
	// step 3: check voltage from Test BMU Board main relay;
	// 		   normal voltage should be 2.5V

	// set Output voltage at 135V/90V; do it the same.

	static int ouvoc_substep = ATF_OUVOC_SUB_STEP_EPWR_NV ;
	static int ouvoc_cnt = 0 ;
	static int ouvoc_checkSt = 0xFF;

	if( act == 0 ) // reset the self check status
	{
		ouvoc_substep = ATF_OUVOC_SUB_STEP_EPWR_NV ;
		ouvoc_cnt = 0 ;
		ouvoc_checkSt = 0xFF;
		*errCode = 0 ;
		return 0 ;
	}

	switch( ouvoc_substep )
	{
		case ATF_OUVOC_SUB_STEP_EPWR_NV:
		{

			int epowr_st = ITECH_ePwrSetVolt( true, ATE_TEST_OV_DW) ;

			if( epowr_st ==1 ) // init successded.
			{
				ouvoc_checkSt = ATF_OUVOC_ST_SLEEP_OK ;
				ouvoc_substep = ATF_OUVOC_SUB_STEP_INIT ;
				ouvoc_cnt = 0 ;
				LOG_I("<ATF> - [UV-OV-OC check] -- [NV(132V)] -- Set ePower OK");
				usleep(10000);
				Ioa_CleanHwProtect();
				usleep(10000);

			}
			else if( epowr_st == -1 )
			{
				ouvoc_checkSt = ATF_OUVOC_ST_SLEEP_FAILD ;
				ouvoc_substep = ATF_OUVOC_SUB_STEP_FAILD ;
				ouvoc_cnt = 0 ;
				LOG_I("<ATF> - [UV-OV-OC check] -- [NV(132V)] -- Set ePower Faild");
			}
			else
			{
				if( ouvoc_cnt == 2 )
				{
					LOG_I("<ATF> - [UV-OV-OC check] -- [NV(132V)] -- Set ePower handing");
				}
			}
		}
		break;


		case ATF_OUVOC_SUB_STEP_INIT:
		{
			Ioa_BmuMainRelayOn(ATE_TEST_BOARD_IDX);
			Ioa_BmuMainRelayOn(ATE_STD_BOARD_IDX);
			Ioa_BmuPwrOn();
			usleep(10000);

			int std_bmu_st = Atf_InitPL455( ATE_STD_BOARD_IDX, &ouvoc_cnt, 10, &ouvoc_checkSt ) ;

			if( std_bmu_st  == 1 ) // init successded.
			{
				ouvoc_checkSt = ATF_OUVOC_ST_SLEEP_OK ;
				ouvoc_substep = ATF_OUVOC_SUB_STEP_SAMPLE_NV ;
				ouvoc_cnt = 0 ;
				LOG_I("<ATF> - [UV-OV-OC check] -- [NV(132V)] -- Init Pl455 Ok.");
			}
			else if( std_bmu_st  == -1 )
			{
				ouvoc_checkSt = ATF_OUVOC_ST_SLEEP_FAILD ;
				ouvoc_substep = ATF_OUVOC_SUB_STEP_FAILD ;
				ouvoc_cnt = 0 ;
				LOG_I("<ATF> - [UV-OV-OC check] -- [NV(132V)] -- Init Pl455  Faild");
			}
			else
			{
				if( ouvoc_cnt == 2 )
				{
					LOG_I("<ATF> - [UV-OV-OC check] -- [NV(132V)] -- Init Pl455  handing");
				}
			}
		}
		break ;

		case ATF_OUVOC_SUB_STEP_SAMPLE_NV:
		{
			static int last_volt ,volt_sample_cnt = 0  ;

			//Ioa_CleanHwProtect();//2016-3-8
			PL455_Task(ATE_STD_BOARD_IDX);

			int volt = PL455_GetPacketVlot(ATE_STD_BOARD_IDX) ;
			int tmp = volt - last_volt ;
			//LOG_I("<ATF> - [UV-OV-OC check] -- [S NV(115V)] -- Sample - Volt.=%d.last_volt=%d. diff=%d..",volt,last_volt, tmp);
			last_volt = volt ;
			if( tmp != 0 ) // < 0.2V
			{
				volt_sample_cnt = 0 ;
			}
			else
			{
				volt_sample_cnt++ ;

				if( volt_sample_cnt >= 20 )
				{
					volt_sample_cnt = 0 ;

					tmp = volt - ATE_TEST_OV_DW ;
					if( (tmp <= 4 ) && (tmp >= -4 ) )
					{
						ouvoc_substep = ATF_OUVOC_SUB_STEP_CHECK_NV ;
						LOG_I("<ATF> - [UV-OV-OC check] -- [S NV(132V)]- - Sample - OK.volt=%0.1fV. diff=%0.1fV.",(float)volt/10.0f, (float)tmp/10.0f );
					}
					else
					{
						ouvoc_substep = ATF_OUVOC_SUB_STEP_FAILD ;
						LOG_I("<ATF> - [UV-OV-OC check] -- [S NV(132V)]- - Sample - FAILD.volt=%0.1fV.diff=%0.1fV.",(float)volt/10.0f, (float)tmp/10.0f);
					}

					ouvoc_cnt = 0 ;

				}
			}

			if( ouvoc_cnt >= 40)
			{
				ouvoc_checkSt = ATF_OUVOC_ST_SLEEP_FAILD ;
				ouvoc_substep = ATF_OUVOC_SUB_STEP_FAILD ;
				ouvoc_cnt = 0 ;
			}
		}
		break ;

		case ATF_OUVOC_SUB_STEP_CHECK_NV:
		{
			static int relay_on_cnt = 0 ;
			ouvoc_cnt++ ;
			if( ouvoc_cnt >= 50)
			{
				ouvoc_substep = ATF_OUVOC_SUB_STEP_FAILD ;
				LOG_I("<ATF> - [UV-OV-OC check] -- [check NV(132V)]-- Relay - FAILD. R:%d.",Ioa_BmuRelayStFeedback(ATE_TEST_BOARD_IDX));
			}

			if( Ioa_BmuRelayStFeedback(ATE_TEST_BOARD_IDX) == IOA_RELAY_TURN_ON )
			{
				relay_on_cnt++;
				if( relay_on_cnt >= 8)
				{
					ouvoc_substep = ATF_OUVOC_SUB_STEP_EPWR_OV ;
					relay_on_cnt = 0 ;
					LOG_I("<ATF> - [UV-OV-OC check] -- [check NV(132V)]-- Relay - OK.R:%d.",Ioa_BmuRelayStFeedback(ATE_TEST_BOARD_IDX));
				}
			}
			else
			{
				relay_on_cnt = 0 ;
			}


			//LOG_I("<ATF> - [UV-OV-OC check] -- [check NV(115V)]- - std=%d.test=%d..",Ioa_BmuRelayStFeedback(ATE_STD_BOARD_IDX), Ioa_BmuRelayStFeedback(ATE_TEST_BOARD_IDX));
		}
		break ;

		case ATF_OUVOC_SUB_STEP_EPWR_OV:
		{

			int epowr_st = ITECH_ePwrSetVolt( true, ATE_TEST_OV_UP) ;

			if( epowr_st ==1 ) // init successded.
			{
				ouvoc_checkSt = ATF_OUVOC_ST_SLEEP_OK ;
				ouvoc_substep = ATF_OUVOC_SUB_STEP_SAMPLE_OV ;
				ouvoc_cnt = 0 ;
				LOG_I("<ATF> - [UV-OV-OC check] -- [OV(134V)] -- Set ePower OK");
				//Ioa_BmuPwrOn();
			}
			else if( epowr_st == -1 )
			{
				ouvoc_checkSt = ATF_OUVOC_ST_SLEEP_FAILD ;
				ouvoc_substep = ATF_OUVOC_SUB_STEP_FAILD ;
				ouvoc_cnt = 0 ;
				LOG_I("<ATF> - [UV-OV-OC check] -- [OV(134V)] -- Set ePower Faild");
			}
			else
			{
				if( ouvoc_cnt == 2 )
				{
					LOG_I("<ATF> - [UV-OV-OC check] -- [OV(134V)] -- Set ePower handing");
				}
			}
		}
		break;

		case ATF_OUVOC_SUB_STEP_SAMPLE_OV:
		{
			static int last_volt ,volt_sample_cnt = 0  ;

			PL455_Task(ATE_STD_BOARD_IDX);


			int volt = PL455_GetPacketVlot(ATE_STD_BOARD_IDX) ;
			int tmp = volt - last_volt ;
			//LOG_I("<ATF> - [UV-OV-OC check] ---- Sample - Volt.=%d.last_volt=%d. diff=%d..",volt,last_volt, tmp);
			last_volt = volt ;
			if( tmp != 0 ) // < 0.2V
			{
				volt_sample_cnt = 0 ;
			}
			else
			{
				volt_sample_cnt++ ;

				if( volt_sample_cnt >= 20 )
				{
					volt_sample_cnt = 0 ;

					tmp = volt - ATE_TEST_OV_UP ;
					if( (tmp <= 4 ) && (tmp >= -4 ) )
					{
						ouvoc_substep = ATF_OUVOC_SUB_STEP_CHECK_OV ;
						LOG_I("<ATF> - [UV-OV-OC check] -- [S OV(134V)]- - Sample - OK.%0.1f.",(float)volt/10.0f);
					}
					else
					{
						ouvoc_substep = ATF_OUVOC_SUB_STEP_FAILD ;
						LOG_I("<ATF> - [UV-OV-OC check] -- [S OV(134V)]- - Sample - FAILD.%0.1f.",(float)volt/10.0f);
					}

					ouvoc_cnt = 0 ;

				}
			}

			if( ouvoc_cnt >= 40)
			{
				ouvoc_checkSt = ATF_OUVOC_ST_SLEEP_FAILD ;
				ouvoc_substep = ATF_OUVOC_SUB_STEP_FAILD ;
				ouvoc_cnt = 0 ;
			}
		}
		break ;


		case ATF_OUVOC_SUB_STEP_CHECK_OV:
		{
			static int relay_on_cnt = 0 ;
			ouvoc_cnt++ ;
			if( ouvoc_cnt >= 50)
			{
				ouvoc_substep = ATF_OUVOC_SUB_STEP_FAILD ;
				LOG_I("<ATF> - [UV-OV-OC check] -- [check OV(134V)]- - Relay - FAILD.%d .",Ioa_BmuRelayStFeedback(ATE_TEST_BOARD_IDX));
			}

			if( Ioa_BmuRelayStFeedback(ATE_TEST_BOARD_IDX) == IOA_RELAY_TURN_OFF )
			{
				relay_on_cnt++;
				if( relay_on_cnt >= 15)
				{
					ouvoc_substep = ATF_OUVOC_SUB_STEP_EPWR_NV_1 ;
					relay_on_cnt = 0 ;
					LOG_I("<ATF> - [UV-OV-OC check] -- [check OV(134V)]- - Relay - OK.%d .",Ioa_BmuRelayStFeedback(ATE_TEST_BOARD_IDX));
				}
			}
			else
			{
				relay_on_cnt = 0 ;
			}
			LOG_I("<ATF> - [UV-OV-OC check] -- - std=%d.test=%d..",Ioa_BmuRelayStFeedback(ATE_STD_BOARD_IDX), Ioa_BmuRelayStFeedback(ATE_TEST_BOARD_IDX));
		}
		break ;


		case ATF_OUVOC_SUB_STEP_EPWR_NV_1:
		{
			Ioa_CleanHwProtect();
			int epowr_st = ITECH_ePwrSetVolt( true, ATE_TEST_UV_UP) ;
			Ioa_BmuPwrOn();
			usleep(10000);

			if( epowr_st ==1 ) // init successded.
			{
				ouvoc_checkSt = ATF_OUVOC_ST_SLEEP_OK ;
				ouvoc_substep = ATF_OUVOC_SUB_STEP_SAMPLE_NV_1 ;
				ouvoc_cnt = 0 ;
				LOG_I("<ATF> - [UV-OV-OC check] -- [NV1(96V)] -- Set ePower OK");
			}
			else if( epowr_st == -1 )
			{
				ouvoc_checkSt = ATF_OUVOC_ST_SLEEP_FAILD ;
				ouvoc_substep = ATF_OUVOC_SUB_STEP_FAILD ;
				ouvoc_cnt = 0 ;
				//LOG_I("<ATF> - [UV-OV-OC check] -- [NV(130V)] -- Set ePower Faild");
			}
			else
			{
				if( ouvoc_cnt == 2 )
				{
					//LOG_I("<ATF> - [UV-OV-OC check] -- [NV(130V)] -- Set ePower handing");
				}
			}
		}
		break;

		case ATF_OUVOC_SUB_STEP_SAMPLE_NV_1:
		{
			static int last_volt ,volt_sample_cnt = 0  ;

			PL455_Task(ATE_STD_BOARD_IDX);

			int volt = PL455_GetPacketVlot(ATE_STD_BOARD_IDX) ;
			int tmp = volt - last_volt ;
			last_volt = volt ;
			if( tmp != 0 ) // < 0.2V
			{
				volt_sample_cnt = 0 ;
			}
			else
			{
				volt_sample_cnt++ ;

				if( volt_sample_cnt >= 20 )
				{
					volt_sample_cnt = 0 ;

					tmp = volt - ATE_TEST_UV_UP ;
					if( (tmp <= 4 ) && (tmp >= -4 ) )
					{
						ouvoc_substep = ATF_OUVOC_SUB_STEP_CHECK_NV_1 ;
						LOG_I("<ATF> - [UV-OV-OC check] -- [S NV(96V)]- - Sample - OK.volt=%0.1f.diff=%d..",(float)volt/10.0f, tmp );
					}
					else
					{
						ouvoc_substep = ATF_OUVOC_SUB_STEP_FAILD ;
						LOG_I("<ATF> - [UV-OV-OC check] -- [S NV(96V)]- - Sample - FAILD.volt=%0.1f.diff=%d..",(float)volt/10.0f, tmp);
					}

					ouvoc_cnt = 0 ;

				}
			}

			if( ouvoc_cnt >= 40)
			{
				ouvoc_checkSt = ATF_OUVOC_ST_SLEEP_FAILD ;
				ouvoc_substep = ATF_OUVOC_SUB_STEP_FAILD ;
				ouvoc_cnt = 0 ;
			}
		}
		break ;

		case ATF_OUVOC_SUB_STEP_CHECK_NV_1:
		{
			static int relay_on_cnt = 0 ;
			ouvoc_cnt++ ;
			if( ouvoc_cnt >= 50)
			{
				ouvoc_substep = ATF_OUVOC_SUB_STEP_FAILD ;
				LOG_I("<ATF> - [UV-OV-OC check] -- [check NV1(96V)]- - Relay - FAILD.%d .",Ioa_BmuRelayStFeedback(ATE_STD_BOARD_IDX));
			}

			if( Ioa_BmuRelayStFeedback(ATE_TEST_BOARD_IDX) == IOA_RELAY_TURN_ON )
			{
				relay_on_cnt++;
				if( relay_on_cnt >= 15)
				{
					ouvoc_substep = ATF_OUVOC_SUB_STEP_EPWR_UV ;
					relay_on_cnt = 0 ;
					LOG_I("<ATF> - [UV-OV-OC check] -- [check NV1(96V)]- - Relay - OK.%d .",Ioa_BmuRelayStFeedback(ATE_STD_BOARD_IDX));
				}
			}
			else
			{
				relay_on_cnt = 0 ;
			}
			LOG_I("<ATF> - [UV-OV-OC check] -- - std=%d.test=%d..",Ioa_BmuRelayStFeedback(ATE_STD_BOARD_IDX), Ioa_BmuRelayStFeedback(ATE_TEST_BOARD_IDX));
		}
		break ;

		case ATF_OUVOC_SUB_STEP_EPWR_UV:
		{

			int epowr_st = ITECH_ePwrSetVolt( true, ATE_TEST_UV_DW) ;//94V

			if( epowr_st ==1 ) // init successded.
			{
				ouvoc_checkSt = ATF_OUVOC_ST_SLEEP_OK ;
				ouvoc_substep = ATF_OUVOC_SUB_STEP_SAMPLE_UV ;
				ouvoc_cnt = 0 ;
				LOG_I("<ATF> - [UV-OV-OC check] -- [UV(94V)] -- Set ePower OK");
			}
			else if( epowr_st == -1 )
			{
				ouvoc_checkSt = ATF_OUVOC_ST_SLEEP_FAILD ;
				ouvoc_substep = ATF_OUVOC_SUB_STEP_FAILD ;
				ouvoc_cnt = 0 ;
				LOG_I("<ATF> - [UV-OV-OC check] -- [UV(94V)] -- Set ePower Faild");
			}
			else
			{
				if( ouvoc_cnt == 2 )
				{
					LOG_I("<ATF> - [UV-OV-OC check] -- [UV(94V)] -- Set ePower handing");
				}
			}
		}
		break;

		case ATF_OUVOC_SUB_STEP_SAMPLE_UV:
		{
			static int last_volt ,volt_sample_cnt = 0  ;

			PL455_Task(ATE_STD_BOARD_IDX);


			int volt = PL455_GetPacketVlot(ATE_STD_BOARD_IDX) ;
			int tmp = volt - last_volt ;
			//LOG_I("<ATF> - [UV-OV-OC check] -- [S UV(94V)] -- Sample - Volt.=%d.last_volt=%d. diff=%d..",volt,last_volt, tmp);
			last_volt = volt ;
			if( tmp != 0 ) // < 0.2V
			{
				volt_sample_cnt = 0 ;
			}
			else
			{
				volt_sample_cnt++ ;

				if( volt_sample_cnt >= 20 )
				{
					volt_sample_cnt = 0 ;

					tmp = volt - ATE_TEST_UV_DW ;
					if( (tmp <= 4 ) && (tmp >= -4 ) )
					{
						ouvoc_substep = ATF_OUVOC_SUB_STEP_CHECK_UV ;
						LOG_I("<ATF> - [UV-OV-OC check] -- [S UV(94V)] -- Sample - OK.%0.1f.",(float)volt/10.0f);
					}
					else
					{
						ouvoc_substep = ATF_OUVOC_SUB_STEP_FAILD ;
						LOG_I("<ATF> - [UV-OV-OC check] -- [S UV(94V)] -- Sample - FAILD.%0.1f.",(float)volt/10.0f);
					}

					ouvoc_cnt = 0 ;

				}
			}



			if( ouvoc_cnt >= 40)
			{
				ouvoc_checkSt = ATF_OUVOC_ST_SLEEP_FAILD ;
				ouvoc_substep = ATF_OUVOC_SUB_STEP_FAILD ;
				ouvoc_cnt = 0 ;
			}
		}
		break ;


		case ATF_OUVOC_SUB_STEP_CHECK_UV:
		{
			static int relay_on_cnt = 0 ;
			ouvoc_cnt++ ;
			if( ouvoc_cnt >= 50)
			{
				ouvoc_substep = ATF_OUVOC_SUB_STEP_FAILD ;
				LOG_I("<ATF> - [UV-OV-OC check] -- [check UV(94V)]- - Relay - FAILD.%d .", Ioa_BmuRelayStFeedback(ATE_STD_BOARD_IDX));
			}

			//if( Ioa_BmuRelayStFeedback(ATE_STD_BOARD_IDX) == IOA_RELAY_TURN_OFF )
			if( Ioa_BmuRelayStFeedback(ATE_TEST_BOARD_IDX) == IOA_RELAY_TURN_OFF )
			{
				relay_on_cnt++;
				if( relay_on_cnt >= 8)
				{
					ouvoc_substep = ATF_OUVOC_SUB_STEP_DONE ;
					relay_on_cnt = 0 ;
					LOG_I("<ATF> - [UV-OV-OC check] -- [check UV(94V)]- - Relay - OK.%d .", Ioa_BmuRelayStFeedback(ATE_STD_BOARD_IDX));
				}
			}
			else
			{
				relay_on_cnt = 0 ;
			}

			LOG_I("<ATF> - [UV-OV-OC check] -- - std=%d.test=%d..",Ioa_BmuRelayStFeedback(ATE_STD_BOARD_IDX), Ioa_BmuRelayStFeedback(ATE_TEST_BOARD_IDX));
		}
		break ;

		case ATF_OUVOC_SUB_STEP_DONE :
		{

		}
		break ;

		case ATF_OUVOC_SUB_STEP_FAILD :
		{

		}
		break ;

		default:
		{
			ouvoc_checkSt = 0 ;
		}
		break;
	}

	//Init standard BMU board
	if( ouvoc_substep == ATF_OUVOC_SUB_STEP_DONE )
	{
		*errCode = 0 ;
		return 1 ;
	}
	else if( ouvoc_substep == ATF_OUVOC_SUB_STEP_FAILD )
	{
		*errCode = ouvoc_checkSt ;
		return -1 ;
	}
	else
	{
		*errCode = ouvoc_checkSt ;
		return 0 ;
	}


}


int Atf_Calibrate_PV( bool act, int *errCode )
{
	static int cali_pv_substep = ATF_CALI_PV_SUB_STEP_INIT ;
	static int cali_pv_cnt = 0 ;
	static int cali_pv_checkSt = 0xFF;
	static int cali_pv_s1_volt = 0 ;
	static int cali_pv_s2_volt = 0 ;
	static int cali_pv_gain = 0 ;
	static int cali_pv_offset = 0 ;


	if( act == 0 ) // reset the self check status
	{
		cali_pv_substep = ATF_CALI_PV_SUB_STEP_INIT ;
		cali_pv_cnt = 0 ;
		cali_pv_checkSt = 0xFF;
		*errCode = 0 ;
		return 0 ;
	}

	switch( cali_pv_substep )
	{
		case ATF_CALI_PV_SUB_STEP_INIT:
		{
			int st = Atf_InitPL455( ATE_TEST_BOARD_IDX, &cali_pv_cnt, 10, &cali_pv_checkSt ) ;
			if( st == 1 )
			{
				//cali_pv_substep = ATF_CALI_PV_SUB_STEP_INIT_EPWR ;
				cali_pv_substep = ATF_CALI_PV_SUB_STEP_EPWR_S1 ;
				cali_pv_cnt = 0 ;
				LOG_I("<ATF> - [PV Calibrate] -- [Test BMU Board] -- PL455 Init - OK.");
			}
			else if( st == 0 )
			{
				if( cali_pv_cnt == 2)
				{
					LOG_I("<ATF> - [PV Calibrate] -- [Test BMU Board] -- PL455 Init - Doing.");
				}
			}
			else if( st == -1 )
			{
				cali_pv_substep = ATF_CALI_PV_SUB_STEP_END ;
				cali_pv_cnt = 0 ;
				LOG_I("<ATF> - [PV Calibrate] -- [Test BMU Board]- - PL455 Init - FAILD.");
			}
		}
		break ;

		case ATF_CALI_PV_SUB_STEP_INIT_EPWR :
		{
			int ret_m = ITECH_SetRemoteCtrlMode(ITECH_DEV_EPOWER);
			if( ret_m >= 0)
			{
				LOG_I("<ePower> - set ePower work to remote mode ok.");
			}
			else
			{
				LOG_I("<ePower> - set ePower work to remote mode faild.");
			}

			int ret_c = ITECH_ePwrSetCurrent(20) ; //20mA
			if( ret_c >= 0)
			{
				LOG_I("<ePower> - set ePower output current 20mA.");
			}

			if( (ret_m >= 0) && (ret_c >= 0) )
			{
				cali_pv_substep = ATF_CALI_PV_SUB_STEP_EPWR_S1 ;
				cali_pv_cnt = 0 ;
			}

			if( cali_pv_cnt >= 10 )
			{
				cali_pv_substep = ATF_CALI_PV_SUB_STEP_END ;
				cali_pv_cnt = 0 ;
			}
			else
			{
				cali_pv_cnt++ ;
			}
		}
		break ;

		case ATF_CALI_PV_SUB_STEP_EPWR_S1 :
		{
			int ret = ITECH_ePwrSetVolt(true, ATE_TEST_DEFAULT_VOLT); //90V
			if( ret > 0 )
			{
				ITECH_TurnOnDev( true, ITECH_DEV_EPOWER);
				ITECH_ePwrSetVolt(false, 0xFF ) ;
				PL455_ResetCorrectPar(ATE_TEST_BOARD_IDX);

				cali_pv_substep = ATF_CALI_PV_SUB_STEP_SAMPLE_S1 ;
				cali_pv_cnt = 0 ;
				LOG_I("<ATF> - [PV Calibrate] -- [ePower]- - Set PV 90 - OK.");
			}
		}
		break ;

		case ATF_CALI_PV_SUB_STEP_SAMPLE_S1 :
		{
			static int last_volt, volt_sample_cnt = 0 ;
			PL455_Task(ATE_TEST_BOARD_IDX);

			if( cali_pv_cnt < 50 )
			{
				cali_pv_cnt++ ;
			}
			else
			{
				//time out
				cali_pv_substep = ATF_CALI_PV_SUB_STEP_END ;
				cali_pv_cnt = 0 ;
				LOG_I("<ATF> - [PV Calibrate] -- [S1(90V)]- - Sample - FAILD.");
			}

			int volt = PL455_GetPacketVlot(ATE_TEST_BOARD_IDX) ;
			int tmp = volt - last_volt ;
			last_volt = volt ;
			if( tmp != 0 ) // < 0.2V
			{
				volt_sample_cnt = 0 ;
			}
			else
			{
				volt_sample_cnt++ ;

				if( volt_sample_cnt >= 20 )
				{
					volt_sample_cnt = 0 ;

					cali_pv_s1_volt = volt ;
					cali_pv_substep = ATF_CALI_PV_SUB_STEP_EPWR_S2 ;
					cali_pv_cnt = 0 ;
					LOG_I("<ATF> - [PV Calibrate] -- [S1(90V)]- - Sample - OK.%0.1f.",(float)cali_pv_s1_volt/10.0f);
				}
			}
		}
		break ;

		case ATF_CALI_PV_SUB_STEP_EPWR_S2 :
		{
			int ret = ITECH_ePwrSetVolt(true, ATE_TEST_PV_CAL_VOLT1); //90V
			if( ret >  0 )
			{
				ITECH_ePwrSetVolt(false, 0xFF ) ;
				//PL455_ResetCorrectPar(ATE_TEST_BOARD_IDX);
				if( cali_pv_cnt >= 20)
				{
					cali_pv_substep = ATF_CALI_PV_SUB_STEP_SAMPLE_S2 ;
					cali_pv_cnt = 0 ;
					LOG_I("<ATF> - [PV Calibrate] -- [ePower]- - Set PV 120 - OK.");
				}
			}

			cali_pv_cnt++ ;

		}
		break ;

		case ATF_CALI_PV_SUB_STEP_SAMPLE_S2 :
		{
			static int last_volt, volt_sample_cnt = 0 ;
			PL455_Task(ATE_TEST_BOARD_IDX);

			if( cali_pv_cnt < 100 )
			{
				cali_pv_cnt++ ;
			}
			else
			{
				//time out
				cali_pv_substep = ATF_CALI_PV_SUB_STEP_END ;
				cali_pv_cnt = 0 ;
				LOG_I("<ATF> - [PV Calibrate] -- [S2(120V)]- - Sample - FAILD.");
			}

			int volt = PL455_GetPacketVlot(ATE_TEST_BOARD_IDX) ;
			int tmp = volt - last_volt ;
			last_volt = volt ;
			if( tmp  != 0 )
			{
				volt_sample_cnt = 0 ;
			}
			else
			{
				volt_sample_cnt++ ;

				if( volt_sample_cnt >= 8 )
				{
					volt_sample_cnt = 0 ;

					cali_pv_s2_volt = volt ;
					cali_pv_substep = ATF_CALI_PV_SUB_STEP_CALI ;
					cali_pv_cnt = 0 ;
					LOG_I("<ATF> - [PV Calibrate] -- [S2(120V)]- - Sample - OK.%0.1f.",(float)cali_pv_s2_volt/10.0f);
				}
			}
		}
		break ;

		case ATF_CALI_PV_SUB_STEP_CALI :
		{
			float tmp = 300000.0f/(cali_pv_s2_volt - cali_pv_s1_volt) ;

			cali_pv_gain = (short)(tmp) ;
			cali_pv_offset = 9000 - cali_pv_s1_volt*cali_pv_gain/100 ;
			LOG_I("<ATF> - [PV Calibrate] -- cali_pv_gain %f. cali_pv_offset%d..",tmp,cali_pv_offset);

			cali_pv_offset = cali_pv_offset/10 ;
			int normal_st = 1 ;
			//check range
			if( (cali_pv_gain > 1200) || (cali_pv_gain < 900) )
			{
				LOG_I("<ATF> - [PV Calibrate] -- calibrate gain over range.");
				normal_st = 0 ;
				cali_pv_substep = ATF_CALI_PV_SUB_STEP_END ;
			}

			if( (cali_pv_offset > 100) || (cali_pv_offset < -100) )
			{
				LOG_I("<ATF> - [PV Calibrate] -- calibrate offset over range.");
				normal_st = 0 ;
				cali_pv_substep = ATF_CALI_PV_SUB_STEP_END ;
			}

			LOG_I("<ATF> - [PV Calibrate] -- calibrate parameter: %d,%d.",cali_pv_gain, cali_pv_offset);

			if( normal_st == 1 )
			{
				cali_pv_substep = ATF_CALI_PV_SUB_STEP_SAVE ;
			}

			cali_pv_cnt = 0 ;
		}
		break ;

		case ATF_CALI_PV_SUB_STEP_SAVE :
		{
			char buff[10] ;

			//int tmp = PL455_LoadCorrectPar(ATE_TEST_BOARD_IDX, buff) ;
			int tmp = PL455_GetCorrectPar(ATE_TEST_BOARD_IDX) ;

			for(char i=0; i<8; i++)
			{
				printf("%d  ",buff[i]) ;
			}
			printf("\n");


			//if( tmp == 1 )
			{
				PL455_LoadCorrectPar(ATE_TEST_BOARD_IDX, buff) ;
				buff[6] = cali_pv_gain - ATE_TEST_DEFAULT_VOLT ;
				buff[7] = cali_pv_offset ;

				PL455_SaveReg(ATE_TEST_BOARD_IDX, buff) ;
				cali_pv_substep = ATF_CALI_PV_SUB_STEP_EPWR_S3 ;
				LOG_I("<ATF> - [PV Calibrate] -- Load calibrate parameter:Ok.");
			}

			cali_pv_cnt = 0 ;
		}
		break ;

		case ATF_CALI_PV_SUB_STEP_EPWR_S3 :
		{
			int ret = ITECH_ePwrSetVolt(true, ATF_PV_CFM_VOLT); //105V
			if( ret >= 0 )
			{
				ITECH_ePwrSetVolt(false, 0xFF ) ;
				//PL455_ResetCorrectPar(ATE_TEST_BOARD_IDX);
				cali_pv_substep = ATF_CALI_PV_SUB_STEP_UPDATE_CALI ;
				cali_pv_cnt = 0 ;

				LOG_I("<ATF> - [PV Calibrate] --- [S2(120V)] --- Set Ok.");
			}

		}
		break ;

		case ATF_CALI_PV_SUB_STEP_UPDATE_CALI :
		{
			int tmp = PL455_GetCorrectPar(ATE_TEST_BOARD_IDX) ;

			if( tmp == 1 )
			{
				cali_pv_substep = ATF_CALI_PV_SUB_STEP_SAMPLE_S3 ;
				cali_pv_cnt = 0 ;
				LOG_I("<ATF> - [PV Calibrate] -- Get calibrate parameter:Ok.");
			}

			cali_pv_cnt++ ;
			if( cali_pv_cnt > 10 )
			{
				cali_pv_substep = ATF_CALI_PV_SUB_STEP_END ;
				cali_pv_cnt = 0 ;
			}

		}
		break ;

		case ATF_CALI_PV_SUB_STEP_SAMPLE_S3 :
		{
			static int last_volt, volt_sample_cnt = 0 ;
			PL455_Task(ATE_TEST_BOARD_IDX);

			if( cali_pv_cnt < 100 )
			{
				cali_pv_cnt++ ;
			}
			else
			{
				//time out
				cali_pv_substep = ATF_CALI_PV_SUB_STEP_END ;
				cali_pv_cnt = 0 ;
			}

			int volt = PL455_GetPacketVlot(ATE_TEST_BOARD_IDX) ;
			int tmp = volt - last_volt ;
			last_volt = volt ;
			if( tmp != 0 )
			{
				volt_sample_cnt = 0 ;
			}
			else
			{
				volt_sample_cnt++ ;
				int tmpA = ATF_PV_CFM_VOLT ;
				tmp = volt - tmpA ;

				if( volt_sample_cnt >= 4 )
				{
					volt_sample_cnt = 0 ;

					if( (tmp > 4) || (tmp < -4) )
					{
						LOG_I("<ATF> - [PV Calibrate] -- calibrate confirm faild.volt=%d.diff=%d.",volt, tmp);
						cali_pv_substep = ATF_CALI_PV_SUB_STEP_END ;
					}
					else
					{
						LOG_I("<ATF> - [PV Calibrate] -- calibrate confirm OK. volt=%d.diff=%d.",volt, tmp);
						cali_pv_substep = ATF_CALI_PV_SUB_STEP_DONE ;
					}

					cali_pv_cnt = 0 ;
				}
			}

		}
		break ;

		case ATF_CALI_PV_SUB_STEP_DONE :
		{
			ITECH_ePwrSetVolt(true, 0) ;
			ITECH_TurnOnDev( false, ITECH_DEV_EPOWER);
		}
		break;

		case ATF_CALI_PV_SUB_STEP_END :
		{
			ITECH_ePwrSetVolt(true, 0) ;
			ITECH_TurnOnDev( false, ITECH_DEV_EPOWER);
		}
		break;

		default:
		break;
	}


	if( cali_pv_substep == ATF_CALI_PV_SUB_STEP_DONE )
	{
		ITECH_TurnOnDev( false, ITECH_DEV_EPOWER);
		*errCode = 0 ;
		return 1 ;
	}
	else if( cali_pv_substep == ATF_CALI_PV_SUB_STEP_END )
	{
		ITECH_TurnOnDev( false, ITECH_DEV_EPOWER);
		*errCode = 11 ;
		return -1 ;
	}
	else
	{
		*errCode = 22 ;
		return 0 ;
	}

}


int Atf_Calibrate_PC( bool act, int *errCode )
{
	static int cali_pc_substep = ATF_CALI_PC_SUB_STEP_INIT_PL455 ;
	static int cali_pc_cnt = 0 ;
	static int cali_pc_checkSt = 0xFF;
	static int cali_pc_s1_curr = 0 ;
	static int cali_pc_s2_curr = 0 ;
	static int cali_pc_s3_curr = 0 ;
	static int cali_pc_chg_gain = 0 ;
	static int cali_pc_chg_offset = 0 ;
	static int cali_pc_dischg_gain = 0 ;
	static int cali_pc_dischg_offset = 0 ;

	if( act == 0 ) // reset the self check status
	{
		cali_pc_substep = ATF_CALI_PC_SUB_STEP_INIT_PL455 ;
		cali_pc_cnt = 0 ;
		cali_pc_checkSt = 0xFF;
		*errCode = 0 ;
		return 0 ;
	}

	switch( cali_pc_substep )
	{
		case ATF_CALI_PC_SUB_STEP_INIT_PL455:
		{
			
			Ioa_CleanHwProtect();
			usleep(10000);
			Ioa_BmuPwrOn();
			usleep(10000);
			
			int st = Atf_InitPL455( ATE_TEST_BOARD_IDX, &cali_pc_cnt, 10, &cali_pc_checkSt ) ;
			if( st == 1 )
			{
				Ioa_BmuMainRelayOn(ATE_TEST_BOARD_IDX) ;
				Ioa_BmuMainRelayOn(ATE_STD_BOARD_IDX) ;
				cali_pc_substep = ATF_CALI_PC_SUB_STEP_INIT_ELOAD ;
				cali_pc_cnt = 0 ;
				LOG_I("<ATF> - [PC Calibrate] -- test board PL455 Init - OK.");
			}
			else if( st == 0 )
			{
				if( cali_pc_cnt == 2)
				{
					LOG_I("<ATF> - [PC Calibrate] -- test board PL455 Init - Doing.");
				}
			}
			else if( st == -1 )
			{
				cali_pc_substep = ATF_CALI_PC_SUB_STEP_END ;
				cali_pc_cnt = 0 ;
				LOG_I("<ATF> - [PC Calibrate] -- test board PL455 Init - FAILD.");
			}
		}
		break ;

		case ATF_CALI_PC_SUB_STEP_INIT_ELOAD :
		{
			int ret_m = ITECH_SetRemoteCtrlMode(ITECH_DEV_ELOAD);
			if( ret_m >= 0)
			{
				LOG_I("<ePower> - set eload work to remote mode ok.");
			}
			else
			{
				LOG_I("<ePower> - set eload work to remote mode faild.");
			}

			int ret_c = ITECH_eLoadSetPwrMode(ELOAD_IN_CC) ; //set eload in cc mode
			if( ret_c >= 0)
			{
				LOG_I("<ePower> - set eLoad work in constance current mode.");
			}

			if( (ret_m >= 0) && (ret_c >= 0) )
			{
				cali_pc_substep = ATF_CALI_PC_SUB_STEP_ELOAD_S1 ;
				cali_pc_cnt = 0 ;
			}

			if( cali_pc_cnt >= 10 )
			{
				cali_pc_substep = ATF_CALI_PC_SUB_STEP_END ;
				cali_pc_cnt = 0 ;
			}
			else
			{
				cali_pc_cnt++ ;
			}
		}
		break ;

		case ATF_CALI_PC_SUB_STEP_ELOAD_S1 :
		{
			int ret = ITECH_eLoadSetPwrPar(ELOAD_IN_CC, 0) ; // 1500
			if( ret >= 0 )
			{
				ITECH_TurnOnDev( true, ITECH_DEV_ELOAD);
				//ITECH_ePwrSetVolt(false, 0xFF ) ;
				PL455_ResetCorrectPar(ATE_TEST_BOARD_IDX);

				cali_pc_substep = ATF_CALI_PC_SUB_STEP_SAMPLE_S1 ;
				cali_pc_cnt = 0 ;
				LOG_I("<ATF> - [PC Calibrate] -- Set Current to 0.");
			}
			else
			{
				LOG_I("<ATF> - [PC Calibrate] -- Set Current to 0. doing ");
			}
		}
		break ;


		case ATF_CALI_PC_SUB_STEP_SAMPLE_S1 :
		{
			static int last_curr_zero, curr_sample_cnt = 0 ;
			PL455_Task(ATE_TEST_BOARD_IDX);

			if( cali_pc_cnt < 100 )
			{
				cali_pc_cnt++ ;
			}
			else
			{
				//time out
				cali_pc_substep = ATF_CALI_PC_SUB_STEP_END ;
				cali_pc_cnt = 0 ;
				LOG_I("<ATF> - [PC Calibrate] -- [S1(0 mA)] --- Time Out");
			}

			if(  cali_pc_cnt == 2 )
			{
				LOG_I("<ATF> - [PC Calibrate] -- [S1(0 mA)] --- Doing ");
			}

			int curr = PL455_GetPacketCurrent(ATE_TEST_BOARD_IDX) ;
			int tmp = curr - last_curr_zero ;
			last_curr_zero = curr ;
			if(  (tmp > 15) || (tmp < -15) ) // <15mA
			{
				curr_sample_cnt = 0 ;
			}
			else
			{
				curr_sample_cnt++ ;

				if( curr_sample_cnt > 10 )
				{
					curr_sample_cnt = 0 ;

					cali_pc_s1_curr = curr ;
					cali_pc_substep = ATF_CALI_PC_SUB_STEP_ELOAD_S2 ;
					cali_pc_cnt = 0 ;
					LOG_I("<ATF> - [PC Calibrate] -- [S1(0 mA)] --- Sample OK. ");
				}
			}

		}
		break ;

		case ATF_CALI_PC_SUB_STEP_ELOAD_S2 :
		{
			int ret = ITECH_eLoadSetPwrPar(ELOAD_IN_CC, 1500) ; // 1500
			if( ret >=  0 )
			{
				//ITECH_ePwrSetVolt(false, 0xFF ) ;
				//PL455_ResetCorrectPar(ATE_TEST_BOARD_IDX);
				cali_pc_substep = ATF_CALI_PC_SUB_STEP_SAMPLE_S2 ;
				cali_pc_cnt = 0 ;
				LOG_I("<ATF> - [PC Calibrate] -- Set Current to 1.5A.");
			}
			else
			{
				LOG_I("<ATF> - [PC Calibrate] -- Set Current to 1.5A. doing ");
			}

		}
		break ;

		case ATF_CALI_PC_SUB_STEP_SAMPLE_S2 :
		{
			static int last_curr, curr_sample_cnt = 0 ;
			PL455_Task(ATE_TEST_BOARD_IDX);
			//PL455_Task(ATE_STD_BOARD_IDX);

			if( cali_pc_cnt < 100 )
			{
				cali_pc_cnt++ ;
			}
			else
			{
				//time out
				cali_pc_substep = ATF_CALI_PC_SUB_STEP_END ;
				cali_pc_cnt = 0 ;
				LOG_I("<ATF> - [PC Calibrate] -- [S2(+1.5A)] --- Sample Faild. ");
			}

			if( cali_pc_cnt == 2 )
			{
				LOG_I("<ATF> - [PC Calibrate] -- [S2(+1.5 A)] --- Doing.  ");
			}

			int curr = PL455_GetPacketCurrent(ATE_TEST_BOARD_IDX) ;
			int tmp = curr - last_curr ;
			last_curr = curr ;
			if(  (tmp > 15) || (tmp < -15)  || (curr < 1200))
			{
				curr_sample_cnt = 0 ;
			}
			else
			{
				curr_sample_cnt++ ;

				if( curr_sample_cnt > 10 )
				{
					curr_sample_cnt = 0 ;
					cali_pc_s2_curr = curr ;
					cali_pc_substep = ATF_CALI_PC_SUB_STEP_ELOAD_S3 ;
					cali_pc_cnt = 0 ;
					LOG_I("<ATF> - [PC Calibrate] -- [S2(+1.5A)] --- Sample OK. I=%d.",cali_pc_s2_curr);
				}
			}
		}
		break ;

		case ATF_CALI_PC_SUB_STEP_ELOAD_S3 :
		{
			//int ret = ITECH_eLoadSetPwrPar(ELOAD_IN_CC, 1500) ; // 1500
			Ioa_BmuMainRelayOff(ATE_TEST_BOARD_IDX) ;
			Ioa_BmuMainRelayOff(ATE_STD_BOARD_IDX) ;

			//if( ret == 1 )
			if( (Ioa_BmuRelayStFeedback(ATE_TEST_BOARD_IDX) == IOA_RELAY_TURN_OFF ) &&
				(Ioa_BmuRelayStFeedback(ATE_STD_BOARD_IDX)  == IOA_RELAY_TURN_OFF) )
			{
				cali_pc_substep = ATF_CALI_PC_SUB_STEP_SAMPLE_S3 ;
				cali_pc_cnt = 0 ;
			}

			cali_pc_cnt++ ;

			if( cali_pc_cnt > 50 )
			{
				cali_pc_substep = ATF_CALI_PC_SUB_STEP_END ;
				cali_pc_cnt = 0 ;
			}

		}
		break ;

		case ATF_CALI_PC_SUB_STEP_SAMPLE_S3 :
		{
			static int last_curr, curr_sample_cnt = 0 ;
			PL455_Task(ATE_TEST_BOARD_IDX);

			if( cali_pc_cnt < 100 )
			{
				cali_pc_cnt++ ;
			}
			else
			{
				//time out
				cali_pc_substep = ATF_CALI_PC_SUB_STEP_END ;
				cali_pc_cnt = 0 ;
				LOG_I("<ATF> - [PC Calibrate] -- [S3(-1.5A)] --- Sample Faild. ");
			}

			if( cali_pc_cnt == 2 )
			{
				LOG_I("<ATF> - [PC Calibrate] -- [S3(-1.5 A)] --- Doing.  ");
			}

			int curr = PL455_GetPacketCurrent(ATE_TEST_BOARD_IDX) ;
			int tmp = curr - last_curr ;
			last_curr = curr ;
			if(  (tmp > 15) || (tmp < -15) ||(curr > -1200) )
			{
				curr_sample_cnt = 0 ;
			}
			else
			{
				curr_sample_cnt++ ;

				if( curr_sample_cnt > 10 )
				{
					curr_sample_cnt = 0 ;
					cali_pc_s3_curr = curr ;
					cali_pc_substep = ATF_CALI_PC_SUB_STEP_CALI ;
					cali_pc_cnt = 0 ;
					LOG_I("<ATF> - [PC Calibrate] -- [S3(-1.5A)] --- Sample OK. I=%d.",cali_pc_s3_curr);
				}
			}

		}
		break ;

		case ATF_CALI_PC_SUB_STEP_CALI :
		{
			float tmp = 1500.0f/(cali_pc_s2_curr - cali_pc_s1_curr) ;

			cali_pc_chg_gain = (short)(tmp*1000) ;
			cali_pc_chg_offset = 1500 - cali_pc_s2_curr*cali_pc_chg_gain/1000  ;

			LOG_I("<ATF> - [PC Calibrate] -- cahrge gain/offset=%d/%d.", cali_pc_chg_gain,cali_pc_chg_offset );

			int normal_st = 1 ;
			//check range
			if( (cali_pc_chg_gain > 1200) || (cali_pc_chg_gain < 900) )
			{
				LOG_I("<ATF> - [PC Calibrate] -- calibrate gain over range.");
				normal_st = 0 ;
				cali_pc_substep = ATF_CALI_PC_SUB_STEP_END ;
			}

			if( (cali_pc_chg_offset > 100) || (cali_pc_chg_offset < -100) )
			{
				LOG_I("<ATF> - [PC Calibrate] -- calibrate offset over range.");
				normal_st = 0 ;
				cali_pc_substep = ATF_CALI_PC_SUB_STEP_END ;
			}


			tmp = 1500.0f/(cali_pc_s1_curr - cali_pc_s3_curr) ;

			cali_pc_dischg_gain = (short)(tmp*1000) ;
			cali_pc_dischg_offset = -1500 - cali_pc_s3_curr*cali_pc_dischg_gain/1000  ;
			LOG_I("<ATF> - [PC Calibrate] -- discahrge gain/offset=%d/%d.", cali_pc_dischg_gain, cali_pc_dischg_offset );
			//check range
			if( (cali_pc_dischg_gain > 1200) || (cali_pc_dischg_gain < 900) )
			{
				LOG_I("<ATF> - [PC Calibrate] -- calibrate gain over range.");
				normal_st = 0 ;
				cali_pc_substep = ATF_CALI_PC_SUB_STEP_END ;
			}

			if( (cali_pc_dischg_offset > 100) || (cali_pc_dischg_offset < -100) )
			{
				LOG_I("<ATF> - [PC Calibrate] -- calibrate offset over range.");
				normal_st = 0 ;
				cali_pc_substep = ATF_CALI_PC_SUB_STEP_END ;
			}

			if( normal_st == 1 )
			{
				cali_pc_substep = ATF_CALI_PC_SUB_STEP_SAVE ;
			}

			cali_pc_cnt = 0 ;
		}
		break ;

		case ATF_CALI_PC_SUB_STEP_SAVE :
		{
			char buff[10] ;

		//	PL455_Init(ATE_TEST_BOARD_IDX) ;
			int tmp = PL455_GetCorrectPar(ATE_TEST_BOARD_IDX) ;
			//int tmp = PL455_LoadCorrectPar(ATE_TEST_BOARD_IDX, buff) ;

			//if( tmp == 1 )
			{
				PL455_LoadCorrectPar(ATE_TEST_BOARD_IDX, buff) ;
				buff[0] = (unsigned char)(cali_pc_dischg_gain>>8);
				buff[1] = (unsigned char)cali_pc_dischg_gain;
				buff[2] = (unsigned char)(cali_pc_chg_gain>>8);
				buff[3] = (unsigned char)cali_pc_chg_gain;
				buff[4] = cali_pc_chg_offset  ;
				buff[5] = cali_pc_dischg_offset ;

				PL455_SaveReg(ATE_TEST_BOARD_IDX, buff) ;
				cali_pc_substep = ATF_CALI_PC_SUB_STEP_UPDATE_CALI ;
				LOG_I("<ATF> - [PC Calibrate] -- save calibrate gain & offset --- Ok.");
			}

			cali_pc_cnt = 0 ;
		}
		break ;

		case ATF_CALI_PC_SUB_STEP_UPDATE_CALI :
		{
			int tmp = PL455_GetCorrectPar(ATE_TEST_BOARD_IDX) ;

			if( tmp == 1 )
			{
				cali_pc_substep = ATF_CALI_PC_SUB_STEP_ELOAD_S4 ;
				cali_pc_cnt = 0 ;
				LOG_I("<ATF> - [PC Calibrate] -- Update calibrate gain & offset --- Ok.");
			}

			cali_pc_cnt++ ;
			if( cali_pc_cnt > 10 )
			{
				cali_pc_substep = ATF_CALI_PC_SUB_STEP_END ;
				cali_pc_cnt = 0 ;
				LOG_I("<ATF> - [PC Calibrate] -- Update calibrate gain & offset --- Faild.");
			}

		}
		break ;

		case ATF_CALI_PC_SUB_STEP_ELOAD_S4 :
		{
			int ret = ITECH_eLoadSetPwrPar(ELOAD_IN_CC, 2250) ; // 1500
			if( ret >= 0 )
			{
				//ITECH_ePwrSetVolt(false, 0xFF ) ;
				//PL455_ResetCorrectPar(ATE_TEST_BOARD_IDX);
				cali_pc_substep = ATF_CALI_PC_SUB_STEP_SAMPLE_S4 ;
				cali_pc_cnt = 0 ;
				LOG_I("<ATF> - [PC Calibrate] -- [S4(-2250mA)] --- set OK.");
			}

		}
		break ;


		case ATF_CALI_PC_SUB_STEP_SAMPLE_S4 :
		{
			static int last_curr, curr_sample_cnt = 0 ;
			PL455_Task(ATE_TEST_BOARD_IDX);

			if( cali_pc_cnt < 100 )
			{
				cali_pc_cnt++ ;
			}
			else
			{
				//time out
				cali_pc_substep = ATF_CALI_PC_SUB_STEP_END ;
				cali_pc_cnt = 0 ;
			}

			int curr = PL455_GetPacketCurrent(ATE_TEST_BOARD_IDX) ;
			int tmp = curr - last_curr ;
			last_curr = curr ;
			if(  (tmp > 10) || (tmp < -10) )
			{
				curr_sample_cnt = 0 ;
			}
			else
			{
				curr_sample_cnt++ ;

				if( curr_sample_cnt > 10 )
				{
					curr_sample_cnt = 0 ;
					//cali_pc_s3_curr = curr ;
					tmp = -2250 - curr  ;
					if( (tmp < 20) &&(tmp >- 20) )
					{
						cali_pc_substep = ATF_CALI_PC_SUB_STEP_ELOAD_S5 ;
						LOG_I("<ATF> - [PC Calibrate] -- discharg calibrate confirm OK.I=%d.Diff=%d", curr, tmp);
					}
					else
					{
						cali_pc_substep = ATF_CALI_PC_SUB_STEP_END ;
						LOG_I("<ATF> - [PC Calibrate] -- discharg calibrate confirm faild.I=%d.Diff=%d", curr, tmp);
					}
					cali_pc_cnt = 0 ;

					int ret = ITECH_eLoadSetPwrPar(ELOAD_IN_CC, 0) ; // 1500

				}
			}

			if( cali_pc_cnt > 30 )
			{

				LOG_I("<ATF> - [PV Calibrate] -- calibrate confirm faild.");
				cali_pc_substep = ATF_CALI_PC_SUB_STEP_END ;

				cali_pc_cnt = 0 ;
				int ret = ITECH_eLoadSetPwrPar(ELOAD_IN_CC, 0) ; // 1500

			}


		}
		break ;

		case ATF_CALI_PC_SUB_STEP_ELOAD_S5 :
		{

			//int ret = ITECH_eLoadSetPwrPar(ELOAD_IN_CC, 1500) ; // 1500
			Ioa_BmuMainRelayOn(ATE_TEST_BOARD_IDX) ;
			Ioa_BmuMainRelayOn(ATE_STD_BOARD_IDX) ;

			//if( ret == 1 )
			if( (Ioa_BmuRelayStFeedback(ATE_TEST_BOARD_IDX) == IOA_RELAY_TURN_OFF ) &&
				(Ioa_BmuRelayStFeedback(ATE_STD_BOARD_IDX)  == IOA_RELAY_TURN_OFF) )
			{

				int ret = ITECH_eLoadSetPwrPar(ELOAD_IN_CC, 2250) ; // 1500
				if( ret >= 0 )
				{
					//ITECH_ePwrSetVolt(false, 0xFF ) ;
					//PL455_ResetCorrectPar(ATE_TEST_BOARD_IDX);
					cali_pc_substep = ATF_CALI_PC_SUB_STEP_SAMPLE_S5 ;
					cali_pc_cnt = 0 ;
					LOG_I("<ATF> - [PC Calibrate] -- [S4(+2250mA)] --- set OK.");
				}
			}

			cali_pc_cnt++ ;

			if( cali_pc_cnt > 50 )
			{
				cali_pc_substep = ATF_CALI_PC_SUB_STEP_END ;
				cali_pc_cnt = 0 ;
			}
		}
		break ;


		case ATF_CALI_PC_SUB_STEP_SAMPLE_S5 :
		{
			static int last_curr, curr_sample_cnt = 0 ;
			PL455_Task(ATE_TEST_BOARD_IDX);

			if( cali_pc_cnt < 100 )
			{
				cali_pc_cnt++ ;
			}
			else
			{
				//time out
				cali_pc_substep = ATF_CALI_PC_SUB_STEP_END ;
				cali_pc_cnt = 0 ;
			}

			int curr = PL455_GetPacketCurrent(ATE_TEST_BOARD_IDX) ;
			int tmp = curr - last_curr ;
			last_curr = curr ;
			if(  (tmp > 10) || (tmp < -10) )
			{
				curr_sample_cnt = 0 ;
			}
			else
			{
				curr_sample_cnt++ ;

				if( curr_sample_cnt > 10 )
				{
					curr_sample_cnt = 0 ;
					//cali_pc_s3_curr = curr ;
					tmp = 2250 - curr  ;
					if( (tmp < 20) &&(tmp >- 20) )
					{
						cali_pc_substep = ATF_CALI_PC_SUB_STEP_DONE ;
						LOG_I("<ATF> - [PC Calibrate] -- calibrate confirm OK.I=%d.Diff=%d", curr, tmp);
					}
					else
					{
						cali_pc_substep = ATF_CALI_PC_SUB_STEP_END ;
						LOG_I("<ATF> - [PC Calibrate] -- calibrate confirm faild.I=%d.Diff=%d", curr, tmp);
					}
					cali_pc_cnt = 0 ;

					int ret = ITECH_eLoadSetPwrPar(ELOAD_IN_CC, 0) ; // 1500

				}
			}

			if( cali_pc_cnt > 30 )
			{

				LOG_I("<ATF> - [PC Calibrate] -- calibrate confirm faild. Time out");
				cali_pc_substep = ATF_CALI_PC_SUB_STEP_END ;

				cali_pc_cnt = 0 ;
				int ret = ITECH_eLoadSetPwrPar(ELOAD_IN_CC, 0) ; // 1500

			}


		}
		break ;


		case ATF_CALI_PC_SUB_STEP_DONE :
		{
			ITECH_ePwrSetVolt(true, 0) ;
			ITECH_TurnOnDev( false, ITECH_DEV_EPOWER);
			Ioa_BmuMainRelayOn(ATE_TEST_BOARD_IDX) ;
			Ioa_BmuMainRelayOff(ATE_STD_BOARD_IDX) ;

		}
		break;

		case ATF_CALI_PC_SUB_STEP_END :
		{
			ITECH_ePwrSetVolt(true, 0) ;
			ITECH_TurnOnDev( false, ITECH_DEV_EPOWER);
		}
		break;

		default:
		break;
	}


	if( cali_pc_substep == ATF_CALI_PC_SUB_STEP_DONE )
	{
		ITECH_ePwrSetVolt(true, 0) ;
		ITECH_TurnOnDev( false, ITECH_DEV_EPOWER);
		Ioa_BmuMainRelayOn(ATE_TEST_BOARD_IDX) ;
		Ioa_BmuMainRelayOff(ATE_STD_BOARD_IDX) ;


		*errCode = 0 ;
		return 1 ;
	}
	else if( cali_pc_substep == ATF_CALI_PC_SUB_STEP_END )
	{
		ITECH_ePwrSetVolt(true, 0) ;
		ITECH_TurnOnDev( false, ITECH_DEV_EPOWER);
		Ioa_BmuMainRelayOn(ATE_TEST_BOARD_IDX) ;
		Ioa_BmuMainRelayOff(ATE_STD_BOARD_IDX) ;

		*errCode = 11 ;
		return -1 ;
	}
	else
	{
		*errCode = 22 ;
		return 0 ;
	}

}

int Atf_Check_OC( bool act, int *errCode )
{
	static int check_oc_substep = ATF_OC_SUB_STEP_INIT_PL455 ;
	static int check_oc_cnt = 0 ;
	static int check_oc_checkSt = 0xFF;

	if( act == 0 ) // reset the self check status
	{
		check_oc_substep = ATF_OC_SUB_STEP_INIT_PL455 ;
		check_oc_cnt = 0 ;
		check_oc_checkSt = 0xFF;
		*errCode = 0 ;
		return 0 ;
	}

	switch( check_oc_substep )
	{
		case ATF_OC_SUB_STEP_INIT_PL455:
		{
			int st = Atf_InitPL455( ATE_TEST_BOARD_IDX, &check_oc_cnt, 10, &check_oc_checkSt ) ;
			if( st == 1 )
			{
				Ioa_BmuMainRelayOn(ATE_TEST_BOARD_IDX) ;
				Ioa_BmuMainRelayOn(ATE_STD_BOARD_IDX) ;
				check_oc_substep = ATF_OC_SUB_STEP_INIT_ELOAD ;
				check_oc_cnt = 0 ;
				LOG_I("<ATF> - [OC Check] -- Standard board PL455 Init - OK.");
			}
			else if( st == 0 )
			{
				if( check_oc_cnt == 2)
				{
					LOG_I("<ATF> - [OC Check] -- Standard board PL455 Init - Doing.");
				}
			}
			else if( st == -1 )
			{
				check_oc_substep = ATF_OC_SUB_STEP_END ;
				check_oc_cnt = 0 ;
				LOG_I("<ATF> - [OC Check] -- Standard board PL455 Init - FAILD.");
			}
		}
		break ;

		case ATF_OC_SUB_STEP_INIT_ELOAD :
		{
			int ret_m = ITECH_SetRemoteCtrlMode(ITECH_DEV_ELOAD);
			if( ret_m >= 0)
			{
				LOG_I("<ePower> - set eload work to remote mode ok.");
			}
			else
			{
				LOG_I("<ePower> - set eload work to remote mode faild.");
			}

			ITECH_eLoadSetMaxCurr();
			int ret_c = ITECH_eLoadSetPwrMode(ELOAD_IN_CC) ; //set eload in cc mode
			if( ret_c >= 0)
			{
				LOG_I("<ePower> - set eLoad work in constance current mode.");
			}

			if( (ret_m >= 0) && (ret_c >= 0) )
			{
				check_oc_substep = ATF_OC_SUB_STEP_ELOAD_S1 ;
				check_oc_cnt = 0 ;
			}

			if( check_oc_cnt >= 10 )
			{
				check_oc_substep = ATF_OC_SUB_STEP_END ;
				check_oc_cnt = 0 ;
			}
			else
			{
				check_oc_cnt++ ;
			}
		}
		break ;

		case ATF_OC_SUB_STEP_ELOAD_S1 :
		{
			int ret = ITECH_eLoadSetPwrPar(ELOAD_IN_CC, 6000) ; // 1500
			if( ret >= 0 )
			{
				ITECH_TurnOnDev( true, ITECH_DEV_ELOAD);
				//ITECH_ePwrSetVolt(false, 0xFF ) ;
				check_oc_substep = ATF_OC_SUB_STEP_SAMPLE_S1 ;
				check_oc_cnt = 0 ;
				LOG_I("<ATF> - [OC Check] -- Set Current to 6A.");
			}
			else
			{
				LOG_I("<ATF>  - [OC Check] -- Set Current to 6A. doing ");
			}
		}
		break ;

		case ATF_OC_SUB_STEP_SAMPLE_S1 :
		{
			static int last_curr_zero, curr_sample_cnt = 0 ;


			if( check_oc_cnt <100 )
			{
				check_oc_cnt++ ;
			}
			else
			{
				//time out
				check_oc_substep = ATF_OC_SUB_STEP_END ;
				check_oc_cnt = 0 ;
				LOG_I("<ATF> - [OC Check] -- [S1(6A)] --- Time Out");
			}

			if( check_oc_cnt == 2 )
			{
				LOG_I("<ATF> - [OC Check] -- [S1(6A)] --- Doing ");
			}

			PL455_Task(ATE_TEST_BOARD_IDX);
			int curr = PL455_GetPacketCurrent(ATE_TEST_BOARD_IDX) ;
			int tmp = curr - last_curr_zero ;
			last_curr_zero = curr ;
			if(  (tmp > 15) || (tmp < -15) || (curr < 4000) )// <15mA
			{
				curr_sample_cnt = 0 ;
			}
			else
			{
				curr_sample_cnt++ ;

				if( curr_sample_cnt > 10 )
				{
					curr_sample_cnt = 0 ;

					int diff = curr - 6000 ;
					if( (diff < 40) && (diff > -40) )
					{
						check_oc_substep = ATF_OC_SUB_STEP_CHECK_S1 ;
					}
					else
					{
						check_oc_substep = ATF_OC_SUB_STEP_END ;
					}

					check_oc_cnt = 0 ;
					LOG_I("<ATF> - [OC Check] -- [S1(6A)] --- Sample OK. curr=%d.diff=%d.",curr, diff);
				}
			}

			LOG_I("<ATF> - [OC check] -- [check S1(6A))]- - Relay - .%d .%d.",Ioa_BmuRelayStFeedback(ATE_STD_BOARD_IDX), Ioa_BmuRelayStFeedback(ATE_TEST_BOARD_IDX));
		}
		break ;

		case ATF_OC_SUB_STEP_CHECK_S1 :
		{
			static int relay_on_cnt = 0 ;

			check_oc_cnt++ ;
			if( check_oc_cnt >= 500)
			{
				//check_oc_substep = ATF_OC_SUB_STEP_END ;
				LOG_I("<ATF> - [OC check] -- [check S1(6A))]- - Relay - FAILD.%d .",Ioa_BmuRelayStFeedback(ATE_STD_BOARD_IDX));
			}

			if( Ioa_BmuRelayStFeedback(ATE_TEST_BOARD_IDX) == IOA_RELAY_TURN_ON )
			{
				relay_on_cnt++;
				if( relay_on_cnt >= 8)
				{
					check_oc_substep = ATF_OC_SUB_STEP_DONE ;
					relay_on_cnt = 0 ;
					LOG_I("<ATF> - [OC check] -- [check S1(6A))]- - Relay - OK.%d .",Ioa_BmuRelayStFeedback(ATE_STD_BOARD_IDX));
				}
			}
			else
			{
				relay_on_cnt = 0 ;
			}

			LOG_I("<ATF> - [OC check] -- [check S1(6A))] -- - std=%d.test=%d..",Ioa_BmuRelayStFeedback(ATE_STD_BOARD_IDX), Ioa_BmuRelayStFeedback(ATE_TEST_BOARD_IDX));

		}
		break ;

		case ATF_OC_SUB_STEP_CHANGE_DIR :
		{
			//int ret = ITECH_eLoadSetPwrPar(ELOAD_IN_CC, 1500) ; // 1500
			Ioa_BmuMainRelayOff(ATE_TEST_BOARD_IDX) ;
			Ioa_BmuMainRelayOff(ATE_STD_BOARD_IDX) ;

			if( (Ioa_BmuRelayStFeedback(ATE_TEST_BOARD_IDX) == IOA_RELAY_TURN_OFF ) &&
				(Ioa_BmuRelayStFeedback(ATE_STD_BOARD_IDX)  == IOA_RELAY_TURN_OFF) )
			{
				check_oc_substep = ATF_OC_SUB_STEP_ELOAD_S1 ;
				check_oc_cnt = 0 ;
			}

			check_oc_cnt++ ;

			if( check_oc_cnt > 50 )
			{
				check_oc_substep = ATF_CALI_PC_SUB_STEP_END ;
				check_oc_cnt = 0 ;
			}

		}
		break ;

		case ATF_OC_SUB_STEP_ELOAD_S2 :
		{
			int ret = ITECH_eLoadSetPwrPar(ELOAD_IN_CC, 6000) ; // 1500
			if( ret >= 0 )
			{
				ITECH_TurnOnDev( true, ITECH_DEV_ELOAD);
				//ITECH_ePwrSetVolt(false, 0xFF ) ;
				//PL455_ResetCorrectPar(ATE_TEST_BOARD_IDX);

				check_oc_substep = ATF_CALI_PC_SUB_STEP_SAMPLE_S1 ;
				check_oc_cnt = 0 ;
				LOG_I("<ATF> - [OC check] -- Set Current to -6A.");
			}
			else
			{
				LOG_I("<ATF> - [OC check] -- Set Current to -6A. doing ");
			}
		}
		break ;

		case ATF_OC_SUB_STEP_SAMPLE_S2 :
		{
			static int last_curr_zero, curr_sample_cnt = 0 ;
			PL455_Task(ATE_TEST_BOARD_IDX);

			if( check_oc_cnt < 100 )
			{
				check_oc_cnt++ ;
			}
			else
			{
				//time out
				check_oc_substep = ATF_CALI_PC_SUB_STEP_END ;
				check_oc_cnt = 0 ;
				LOG_I("<ATF> -  [OC check] -- [S2(-6A)] --- Time Out");
			}

			if( check_oc_cnt == 2 )
			{
				LOG_I("<ATF> -  [OC check] -- [S2(-6A)] --- Doing ");
			}

			int curr = PL455_GetPacketCurrent(ATE_TEST_BOARD_IDX) ;

			int tmp = curr - last_curr_zero ;
			last_curr_zero = curr ;
			if(  (tmp > 15) || (tmp < -15) ) // <15mA
			{
				curr_sample_cnt = 0 ;
			}
			else
			{
				curr_sample_cnt++ ;

				if( curr_sample_cnt > 10 )
				{
					curr_sample_cnt = 0 ;

					int diff = curr + 6000 ;
					if( (diff < 40) && (diff > -40) )
					{
						check_oc_substep = ATF_OC_SUB_STEP_CHECK_S2 ;
					}
					else
					{
						check_oc_substep = ATF_OC_SUB_STEP_END ;
					}

					check_oc_cnt = 0 ;
					LOG_I("<ATF> - [PC Calibrate] -- [S1(0 mA)] --- Sample OK. ");
				}
			}
		}
		break ;

		case ATF_OC_SUB_STEP_CHECK_S2 :
		{
			static int relay_on_cnt = 0 ;

			check_oc_cnt++ ;
			if( check_oc_cnt >= 50)
			{
				check_oc_substep = ATF_OC_SUB_STEP_END ;
				LOG_I("<ATF> - [UV-OV-OC check] -- [check NV1(130V)]- - Relay - FAILD.%d .",Ioa_BmuRelayStFeedback(ATE_STD_BOARD_IDX));
			}

			if( Ioa_BmuRelayStFeedback(ATE_TEST_BOARD_IDX) == IOA_RELAY_TURN_ON )
			{
				relay_on_cnt++;
				if( relay_on_cnt >= 8)
				{
					check_oc_substep = ATF_OC_SUB_STEP_DONE ;
					relay_on_cnt = 0 ;
					LOG_I("<ATF> - [UV-OV-OC check] -- [check NV1(130V)]- - Relay - OK.%d .",Ioa_BmuRelayStFeedback(ATE_STD_BOARD_IDX));
				}
			}
			else
			{
				relay_on_cnt = 0 ;
			}
			LOG_I("<ATF> - [UV-OV-OC check] -- - std=%d.test=%d..",Ioa_BmuRelayStFeedback(ATE_STD_BOARD_IDX), Ioa_BmuRelayStFeedback(ATE_TEST_BOARD_IDX));

		}
		break ;


		case ATF_OC_SUB_STEP_DONE :
		{
			ITECH_ePwrSetVolt(true, 0) ;
			ITECH_TurnOnDev( false, ITECH_DEV_EPOWER);
			Ioa_BmuMainRelayOn(ATE_TEST_BOARD_IDX) ;
			Ioa_BmuMainRelayOff(ATE_STD_BOARD_IDX) ;

		}
		break;

		case ATF_OC_SUB_STEP_END :
		{
			ITECH_ePwrSetVolt(true, 0) ;
			ITECH_TurnOnDev( false, ITECH_DEV_EPOWER);
		}
		break;

		default:
		break;
	}


	if( check_oc_substep == ATF_OC_SUB_STEP_DONE )
	{
		ITECH_ePwrSetVolt(true, 0) ;
		ITECH_TurnOnDev( false, ITECH_DEV_EPOWER);
		Ioa_BmuMainRelayOn(ATE_TEST_BOARD_IDX) ;
		Ioa_BmuMainRelayOff(ATE_STD_BOARD_IDX) ;


		*errCode = 0 ;
		return 1 ;
	}
	else if( check_oc_substep == ATF_OC_SUB_STEP_END )
	{
		ITECH_ePwrSetVolt(true, 0) ;
		ITECH_TurnOnDev( false, ITECH_DEV_EPOWER);
		Ioa_BmuMainRelayOn(ATE_TEST_BOARD_IDX) ;
		Ioa_BmuMainRelayOff(ATE_STD_BOARD_IDX) ;

		*errCode = 11 ;
		return -1 ;
	}
	else
	{
		*errCode = 22 ;
		return 0 ;
	}

}


static int Atf_InitPL455( int boardNo, int *time_cnt, int time_out, int *check_st )
{
	PL455_Init(boardNo);
	int st = PL455_GetInitSt(boardNo);

	if( st == PL455_INIT_ST_OK ) // init successded.
	{
		*check_st = ATF_COMM_ST_PL455_INIT_OK ;
		*time_cnt = 0 ;
		return 1 ;
	}
	else
	{
		*check_st = ATF_COMM_ST_PL455_INIT_DOING ;
		*time_cnt++ ; //try again ;
	}

	if( *time_cnt >= time_out )
	{
		*check_st = ATF_COMM_ST_PL455_INIT_FAILD ;
		*time_cnt = 0 ;
		return -1 ;
	}
	else
	{
		return 0 ;
	}
}


static int Atf_Check_CellVolt( bool act, int *errCode )
{
	static int check_cv_substep = ATF_CHECK_CV_SUB_STEP_INIT ;
	static int check_cv_cnt = 0 ;
	static int check_cv_checkSt = 0xFF;


	if( act == 0 ) // reset the self check status
	{
		check_cv_substep = ATF_CHECK_CV_SUB_STEP_INIT ;
		check_cv_cnt = 0 ;
		check_cv_checkSt = 0xFF;
		*errCode = 0 ;
		return 0 ;
	}

	int time_out = 10 ;
	switch( check_cv_substep )
	{
		case ATF_CHECK_CV_SUB_STEP_INIT:
		{
			int test_bmu_st = Atf_InitPL455( ATE_TEST_BOARD_IDX, &check_cv_cnt, 20, &check_cv_checkSt ) ;
			int std_bmu_st  = Atf_InitPL455( ATE_STD_BOARD_IDX,  &check_cv_cnt, 20, &check_cv_checkSt ) ;
			if( (test_bmu_st == 1 ) && (std_bmu_st == 1 ) )
			{
				check_cv_substep = ATF_CHECK_CV_STEP_SAMPLE_CV; // sample cell voltage 
				check_cv_cnt = 0 ;
				LOG_I("<ATF> - [CV Sample] -- [Test BMU Board] -- PL455 Init - OK.");
			}
			else if( (test_bmu_st == 0 ) || (std_bmu_st == 0 ) )
			{
				if( check_cv_cnt == 2)
				{
					LOG_I("<ATF> - [CV Sample] -- [Test BMU Board] -- PL455 Init - Doing.");
				}
			}
			else if( (test_bmu_st == -1 ) || ( std_bmu_st == -1 ) )
			{
				check_cv_substep = ATF_CHECK_CV_STEP_FAILD ;
				check_cv_cnt = 0 ;
				LOG_I("<ATF> - [CV Sample] -- [Test BMU Board]- - PL455 Init - FAILD.");
			}
		}
		break ;

		case ATF_CHECK_CV_STEP_SAMPLE_CV :
		{
			static int cv_sample_cnt = 0 ;
			
			PL455_Task(ATE_TEST_BOARD_IDX);
			PL455_Task(ATE_STD_BOARD_IDX);

			if( check_cv_cnt < 50 )
			{
				check_cv_cnt++ ;
			}
			else
			{
				//time out
				check_cv_substep = ATF_CHECK_CV_STEP_FAILD ;
				check_cv_cnt = 0 ;
				LOG_I("<ATF> - [CV Sample] -- [Test BMU Board] -- Sample - FAILD.");
			}

			int st = PL455_CheckCellVolt_normal_st() ;

			if( st != 1 ) // < 0.2V
			{
				cv_sample_cnt = 0 ;
			}
			else
			{
				cv_sample_cnt++ ;

				if( cv_sample_cnt >= 5 )
				{
					cv_sample_cnt = 0 ;

					check_cv_substep = ATF_CHECK_CV_STEP_SAMPLE_CT ;
					check_cv_cnt = 0 ;
					LOG_I("<ATF> - [CV Sample] -- Sample - OK.");
				}
			}
		}
		break ;

		case ATF_CHECK_CV_STEP_SAMPLE_CT :
		{
			static int ct_sample_cnt = 0 ;
			
			PL455_Task(ATE_TEST_BOARD_IDX);
			PL455_Task(ATE_STD_BOARD_IDX);

			if( check_cv_cnt < 50 )
			{
				check_cv_cnt++ ;
			}
			else
			{
				//time out
				check_cv_substep = ATF_CHECK_CV_STEP_FAILD ;
				check_cv_cnt = 0 ;
				LOG_I("<ATF> - [CT Sample] -- [Test BMU Board] -- Sample - FAILD.");
			}

			int st = PL455_CheckCellTemp_normal_st() ;

			if( st != 1 ) // < 0.2V
			{
				ct_sample_cnt = 0 ;
			}
			else
			{
				ct_sample_cnt++ ;

				if( ct_sample_cnt >= 20 )
				{
					ct_sample_cnt = 0 ;

					check_cv_substep = ATF_CHECK_CV_STEP_DONE ;
					check_cv_cnt = 0 ;
					LOG_I("<ATF> - [CT Sample] -- Sample -- OK.");
				}
			}
		}
		break ;

		case ATF_CHECK_CV_STEP_DONE :
		{

		}
		break;

		case ATF_CHECK_CV_STEP_FAILD :
		{

		}
		break;

		default:
		break;
	}


	if( check_cv_substep == ATF_CHECK_CV_STEP_DONE )
	{
		*errCode = 0 ;
		return 1 ;
	}
	else if( check_cv_substep == ATF_CHECK_CV_STEP_FAILD )
	{
		*errCode = 11 ;
		return -1 ;
	}
	else
	{
		*errCode = 22 ;
		return 0 ;
	}


}



static int Atf_Check_Balance( bool act, int *errCode )
{
	// init test PL455 ;
	// sample the cell volt 
	// balance the odd cell channels ;
	// wait for 30s ;
	// sample the cell volt and compare ;
	// balance the even cell channels ;
	// wait for 30s ;
	// sample the cell volt and compare ;
	// quit


	static int check_bal_substep = ATF_CHECK_BAL_SUB_STEP_INIT ;
	static int check_bal_cnt = 0 ;
	static int check_bal_checkSt = 0xFF;


	if( act == 0 ) // reset the self check status
	{
		check_bal_substep = ATF_CHECK_BAL_SUB_STEP_INIT ;
		check_bal_cnt = 0 ;
		check_bal_checkSt = 0xFF;
		*errCode = 0 ;
		return 0 ;
	}

	switch( check_bal_substep )
	{
		case ATF_CHECK_BAL_SUB_STEP_INIT:
		{
			int test_bmu_st = Atf_InitPL455( ATE_TEST_BOARD_IDX, &check_bal_cnt, 20, &check_bal_checkSt ) ;
			
			if( test_bmu_st == 1 )
			{
				check_bal_substep = ATF_CHECK_BAL_SUB_STEP_SAMPLE_IDLE; // sample cell voltage 
				check_bal_cnt = 0 ;
				LOG_I("<ATF> - [Balance] -- [Test BMU Board] -- PL455 Init - OK.");
			}
			else if( test_bmu_st == 0 ) 
			{
				if( check_bal_cnt == 2)
				{
					LOG_I("<ATF> - [Balance] -- [Test BMU Board] -- PL455 Init - Doing.");
				}
			}
			else if( test_bmu_st == -1 )
			{
				check_bal_substep = ATF_CHECK_CV_STEP_FAILD ;
				check_bal_cnt = 0 ;
				LOG_I("<ATF> - [Balance] -- [Test BMU Board]- - PL455 Init - FAILD.");
			}
		}
		break ;

		case ATF_CHECK_BAL_SUB_STEP_SAMPLE_IDLE :
		{
			#define BAL_IDLE_SAMPLE_MAX_CNT 		30
			#define BAL_IDLE_SAMPLE_CFM_CNT 		15

			static int bal_idle_normal_cnt = 0 ;

			PL455_Task(ATE_TEST_BOARD_IDX);

			int st = PL455_CheckBal(ATE_TEST_BOARD_IDX, BAL_IDLE) ; //cell volt is in 3200 -4150 ;
			if( st == 1 )
			{
				bal_idle_normal_cnt++ ;

				if (bal_idle_normal_cnt >= BAL_IDLE_SAMPLE_CFM_CNT)
				{
					check_bal_substep = ATF_CHECK_BAL_SUB_STEP_BAL_ODD ;
					bal_idle_normal_cnt = 0 ;
					check_bal_cnt = 0 ;
				}
			}
			else
			{
				bal_idle_normal_cnt = 0 ;
			}

			check_bal_cnt++ ;

			if( check_bal_cnt >= BAL_IDLE_SAMPLE_MAX_CNT )
			{
				check_bal_cnt = 0 ;
				bal_idle_normal_cnt = 0 ;
				check_bal_substep = ATF_CHECK_BAL_SUB_STEP_FAILD ;
				LOG_I("<ATF> - [balance] -- idle sample - NOK.");
			}

		}
		break ;

		case ATF_CHECK_BAL_SUB_STEP_BAL_ODD :
		{
			unsigned char nDev_ID ;
			int chs ;

			chs = 0x5555 ;

			nDev_ID = 0 ;
			PL455_BalanceSet( chs, PL455_BALANCE_TIME_USED, nDev_ID) ;
			nDev_ID = 1 ;
			PL455_BalanceSet( chs, PL455_BALANCE_TIME_USED, nDev_ID) ;

			check_bal_cnt = 0 ;
			check_bal_substep = ATF_CHECK_BAL_SUB_STEP_ODD_WAIT ;
			LOG_I("<ATF> - [balance] -- balance odd channel - cmd.");

		}
		break ;

		case ATF_CHECK_BAL_SUB_STEP_ODD_WAIT :
		{
			#define BAL_ODD_CH_DUR		200 //20s
			
			PL455_Task(ATE_TEST_BOARD_IDX); // keep Pl455 awake 

			check_bal_cnt++ ;

			if(check_bal_cnt >= BAL_ODD_CH_DUR)
			{
				check_bal_cnt = 0 ;
				check_bal_substep = ATF_CHECK_BAL_SUB_STEP_ODD_COMPARE ;				
			}
		}
		break ;

		case ATF_CHECK_BAL_SUB_STEP_ODD_COMPARE :
		{
			#define BAL_ODD_SAMPLE_MAX_CNT 		30
			#define BAL_ODD_SAMPLE_CFM_CNT 		5

			static int bal_odd_sample_cnt = 0 ;

			PL455_Task(ATE_TEST_BOARD_IDX);

			int st = PL455_CheckBal(ATE_TEST_BOARD_IDX, BAL_ODD) ; //cell volt is in 3200 -4150 ;
			if( st == 1 )
			{
				bal_odd_sample_cnt++ ;

				if (bal_odd_sample_cnt >= BAL_ODD_SAMPLE_CFM_CNT)
				{
					check_bal_substep = ATF_CHECK_BAL_SUB_STEP_BAL_EVEN ;
					LOG_I("<ATF> - [balance] -- Odd balance - NOK.");
					bal_odd_sample_cnt = 0 ;
					check_bal_cnt = 0 ;
				}
			}
			else
			{
				bal_odd_sample_cnt = 0 ;
			}

			check_bal_cnt++ ;

			if( check_bal_cnt >= BAL_ODD_SAMPLE_MAX_CNT )
			{
				check_bal_cnt = 0 ;
				bal_odd_sample_cnt = 0 ;
				check_bal_substep = ATF_CHECK_BAL_SUB_STEP_FAILD ;
				LOG_I("<ATF> - [balance] -- Odd balance - NOK.");
			}

		}
		break ;

		case ATF_CHECK_BAL_SUB_STEP_BAL_EVEN :
		{
			unsigned char nDev_ID ;
			int chs ;

			chs = 0xAAAA ;

			nDev_ID = 0 ;
			PL455_BalanceSet( chs, PL455_BALANCE_TIME_USED, nDev_ID) ;
			nDev_ID = 1 ;
			PL455_BalanceSet( chs, PL455_BALANCE_TIME_USED, nDev_ID) ;

			check_bal_cnt = 0 ;
			check_bal_substep = ATF_CHECK_BAL_SUB_STEP_EVEN_WAIT ;
			LOG_I("<ATF> - [balance] -- balance odd channel - cmd.");

		}
		break ;

		case ATF_CHECK_BAL_SUB_STEP_EVEN_WAIT :
		{
			#define BAL_EVEN_CH_DUR		200 //20s
			
			PL455_Task(ATE_TEST_BOARD_IDX); // keep Pl455 awake 

			check_bal_cnt++ ;

			if(check_bal_cnt >= BAL_EVEN_CH_DUR)
			{
				check_bal_cnt = 0 ;
				check_bal_substep = ATF_CHECK_BAL_SUB_STEP_EVEN_COMPARE ;				
			}
		}
		break ;

		case ATF_CHECK_BAL_SUB_STEP_EVEN_COMPARE :
		{
			#define BAL_EVEN_SAMPLE_MAX_CNT 		30
			#define BAL_EVEN_SAMPLE_CFM_CNT 		5

			static int bal_even_sample_cnt = 0 ;

			PL455_Task(ATE_TEST_BOARD_IDX);

			int st = PL455_CheckBal(ATE_TEST_BOARD_IDX, BAL_EVEN) ; //cell volt is in 3200 -4150 ;
			if( st == 1 )
			{
				bal_even_sample_cnt++ ;

				if (bal_even_sample_cnt >= BAL_ODD_SAMPLE_CFM_CNT)
				{
					check_bal_substep = ATF_CHECK_BAL_SUB_STEP_BAL_ODD ;					
					bal_even_sample_cnt = 0 ;
					check_bal_cnt = 0 ;
					LOG_I("<ATF> - [balance] -- Odd balance - NOK.");
				}
			}
			else
			{
				bal_even_sample_cnt = 0 ;
			}

			check_bal_cnt++ ;

			if( check_bal_cnt >= BAL_EVEN_SAMPLE_MAX_CNT )
			{
				check_bal_cnt = 0 ;
				bal_even_sample_cnt = 0 ;
				check_bal_substep = ATF_CHECK_BAL_SUB_STEP_FAILD ;
				LOG_I("<ATF> - [balance] -- Odd balance - NOK.");
			}

		}
		break ;

		case ATF_CHECK_BAL_SUB_STEP_FAILD :
		{
			unsigned char nDev_ID ;
			nDev_ID = 0 ;
			PL455_DisableBal(nDev_ID) ;
			nDev_ID = 1 ;
			PL455_DisableBal(nDev_ID) ; 

			check_bal_cnt = 0 ;
		}
		break ;

		case ATF_CHECK_BAL_SUB_STEP_DONE :
		{
			unsigned char nDev_ID ;
			nDev_ID = 0 ;
			PL455_DisableBal(nDev_ID) ;
			nDev_ID = 1 ;
			PL455_DisableBal(nDev_ID) ; 

			check_bal_cnt = 0 ;
		}
		break ;

		default:
		break;
	}


	if( check_bal_substep == ATF_CHECK_BAL_SUB_STEP_DONE )
	{
		*errCode = 0 ;
		return 1 ;
	}
	else if( check_bal_substep == ATF_CHECK_BAL_SUB_STEP_FAILD )
	{
		*errCode = 11 ;
		return -1 ;
	}
	else
	{
		*errCode = 22 ;
		return 0 ;
	}

}



static int Atf_Check_PL455_ref( bool act, int *errCode )
{


}

static int Atf_Check_PC_Cali_Batt( bool act, int *errCode )
{

}
