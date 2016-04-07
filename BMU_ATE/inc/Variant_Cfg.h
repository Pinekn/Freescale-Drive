#ifndef _VARIANT_CFG_H_
#define _VARIANT_CFG_H_

// log info debug configure
#define LOG_DEBUG_ENABLE 				 1

//standard board ,test board index
#define ATE_STD_BOARD_IDX   				0
#define ATE_TEST_BOARD_IDX   				1

#define BMU_HW_VERSION 						1U

#define BBM_SW_VERSION 			 "1.0.0 - Beta3"

/* packe/moudle number configure */
#define MAX_PACKET_NUM                   2U
#define MAX_MOUDLE_NUM                   2U

#define MAX_TEMP_NUM_PER_MOUDLE			 4U
#define MAX_CELL_NUM_PER_MOUDLE			16U


#define CELL_MOUDLE_NO1    				0U
#define CELL_MOUDLE_NO2    				1U


#define CELL_ILLHEALTH_STS   			0U
#define CELL_HEALTH_STS      			1U



/* BMU default threshold value */
#define CVM_OV_LV1_TH             4050 //4050mV 3950
#define CVM_OV_LV2_TH             4110 //4100mV
#define CVM_UV_LV1_TH             3350 //3350mV
#define CVM_UV_LV2_TH             3300 //3300mV
#define CVM_UV_SHUTDOWN_THUP      3200
#define CVM_UV_SHUTDOWN_THDW      2960

#define CVM_ALM_OV_NORMAL_C       4800
#define CVM_ALM_UV_NORMAL_C       2000

//#define CVM_UV_LV1_TH             3050 //3350mV
//#define CVM_UV_LV2_TH             3000 //3300mV
//#define CVM_UV_SHUTDOWN_THUP      2900
//#define CVM_UV_SHUTDOWN_THDW      2860


#define PVM_OV_LV1_TH             1300 //130V
#define PVM_OV_LV2_TH             1320 //132V
#define PVM_UV_LV1_TH              920 //92V
#define PVM_UV_LV2_TH              900 //90V

#define PCM_CHRG_OC_LV1_TH        3300 //2000mA
#define PCM_CHRG_OC_LV2_TH        3500 //2300mA
#define PCM_DISCHRG_OC_LV1_TH    -4800
#define PCM_DISCHRG_OC_LV2_TH    -5000

#define CTM_CHRG_OT_LV1_TH        380 //38C
#define CTM_CHRG_OT_LV2_TH        410 //41C
#define CTM_CHRG_UT_LV1_TH         10  //1C
#define CTM_CHRG_UT_LV2_TH        -20 //-2C
#define CTM_DISCHRG_OT_LV1_TH     480 //58C
#define CTM_DISCHRG_OT_LV2_TH     510 //61C
#define CTM_DISCHRG_UT_LV1_TH     -18 //-18C
#define CTM_DISCHRG_UT_LV2_TH     -21 //-21C

#define SOC_LOW_LV1_TH            20 //20%
#define SOC_LOW_LV2_TH            10 //10%

/*Threshold vlaue offset */
#define CVM_OV_LV1_TH_OFFSET              6 //6mV
#define CVM_OV_LV2_TH_UP_OFFSET           6 //6mV
#define CVM_OV_LV2_TH_DOWN_OFFSET         20 //6mV
#define CVM_UV_LV1_TH_OFFSET              6 //6mV
#define CVM_UV_LV2_TH_UP_OFFSET           6 //6mV
#define CVM_UV_LV2_TH_DOWN_OFFSET         6 //6mV
#define CVM_OV_DIFF_THDW            485 //500-15mV
#define CVM_OV_DIFF_THUP            515 //500+15mV

//#define SOC_LOW_LV1_TH_OFFSET             50 //0.5%
//#define SOC_LOW_LV2_TH_OFFSET             50 //0.5%
/////////////////// debug //////////////
#define SOC_LOW_LV1_TH_OFFSET             2 //0.5%
#define SOC_LOW_LV2_TH_OFFSET             2 //0.5%




#define IOA_BMU_CH1                         0
#define IOA_BMU_CH2                         1

//BEC hardware type(only 1 bit )
#define BEC_HW_TYPE_110V					0U
#define BEC_HW_TYPE_220V					1U

#define BEC_HW_MAX_PWR_RATE					500U
#define BEC_HW_MIN_PWR_RATE					300U


// packet voltage differece allow the packet combine
#define PWR_POLICY_PACKET_VOLT_DIFF_TH_DW      	10    //2.0V -> 1.2V -> 1.0V(2015-12-29)
#define PWR_POLICY_PACKET_VOLT_DIFF_TH_UP      	100    //4.0V -> 2.5V -> 10V(2015-12-26)



typedef struct
{
	short CellVoltOffset[MAX_MOUDLE_NUM];      // 320  - AFE offset data
	short CellTempOffset[MAX_MOUDLE_NUM][MAX_TEMP_NUM_PER_MOUDLE];
	short CellTempVref[MAX_MOUDLE_NUM];

	unsigned short  hystereticThreshold;
	unsigned short  cellBalBegThreshold;
	unsigned short  onTime_ms;
	unsigned short  pulseChargeTime;    //  38  - Charge pulse (ms)
	unsigned short  pulseRelaxTime;     //  40  - Relaxation pulse (ms)
	unsigned short  maxChrgCurrent ;
	unsigned short  maxDischrgCurrent ;

}Bmu_Config_C_Types;


typedef struct
{
	unsigned short  BmuPacketPwrCap[MAX_PACKET_NUM] ;
	unsigned short  BmuDeviceSN[MAX_PACKET_NUM][MAX_MOUDLE_NUM] ;
	unsigned  char  BmuChipLot[MAX_PACKET_NUM][MAX_MOUDLE_NUM][8] ;

	unsigned short  maxBecPwrRate ;
	unsigned short  BecSoftwareVersion ;
	unsigned short  BecHardwareType ;  // 110V or 220V
	unsigned short  BecHardwareVersion ;

}Bms_ConfigTypes;

extern const Bmu_Config_C_Types  Bmu_Config_C;
extern Bms_ConfigTypes     Bms_ConfigPar;

#ifdef __cplusplus
extern "C" {
#endif

#define READ_MODULE_F		"rb"


int open_cfg(const char* filename, const char* module);
const char* get_cfg_string(const char* funname, const char* var, char* val, int v_l, const char* defval);
long get_cfg_int(const char* funname, const char* var, long defval);
int write_func(const char* funname);
int write_string(const char* key, const char* value);
int write_int(const char* key, long value);
int write_func_end();
void close_cfg();

#ifdef __cplusplus
};
#endif



#endif
