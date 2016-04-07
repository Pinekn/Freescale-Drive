#ifndef _PL455A_H_
 #define _PL455A_H_

#include  "../inc/Variant_Cfg.h"



#define     PL455_DEFAULT_BAUDRATE      250000   //The PL455A default baudrate is 250K
#define     PL455_WORKING_BAUDRATE      500000   //The baud rate is set in the COMCONFIG[BAUD] register for 125k-250k-500k-1M baud rates.


// Register Maps
#define     PL455_REG_STATUS            0x51

#define     PL455_REG_FAULT_SUM         0x52
#define     PL455_REG_FAULT_COM         0x5E
#define     PL455_REG_FAULT_SYS         0x60
#define     PL455_REG_FAULT_DEV         0x61

#define     PL455_REG_FAULT_UV 			0x54
#define     PL455_REG_FAULT_OV 			0x56
#define     PL455_REG_FAULT_AUX 		0x58
#define     PL455_REG_FAULT_2UV 		0x5A
#define     PL455_REG_FAULT_2OV 		0x5C
#define     PL455_REG_FO_CTRL 			0x6E

#define     PL455_REG_DEVCONFIG         0x0E
#define     PL455_REG_DEV_CTRL          12
#define     PL455_REG_ADDR              0x0A
#define     PL455_REG_COMCONFIG         16
#define     PL455_REG_CBCONFIG          19  // Balance Configuration
#define     PL455_REG_CBENBL            20  // Balancing Enable
#define     PL455_REG_TSTCONFIG         30  // Test Configuration

#define     PL455_REG_MAGIC1            0x82  //
#define     PL455_REG_MAGIC2            0xFC  //
#define     PL455_REG_CTO               0x28
#define     PL455_REG_SN                0xC6
#define     PL455_REG_LOT               0xBE

#define     PL455_REG_AX0OFFSET         0xD4  // AX0OFFSET
#define     PL455_REG_AX1OFFSET         0xD6  // AX0OFFSET
#define     PL455_REG_AX2OFFSET         0xD8  // AX0OFFSET
#define     PL455_REG_SCRATCH           0xC8

#define     PL455_REG_CALI_PC           PL455_REG_SCRATCH
#define     PL455_REG_CALI_PV           0x2C




// REQ_TYPE in Frame Initialization Byte Fields
#define     FRMWRT_SGL_R	            0x00 // single device write with response
#define     FRMWRT_SGL_NR	            0x10 // single device write without response
#define     FRMWRT_GRP_R	            0x20 // group broadcast with response
#define     FRMWRT_GRP_NR	            0x30 // group broadcast without response
#define     FRMWRT_ALL_R	            0x60 // general broadcast with response
#define     FRMWRT_ALL_NR	            0x70 // general broadcast without response


#define     PL455_DEFAULT_BAUDRATE          250000
#define     PL455_VREF_MV                   2500

/* Balance control */
#define     PL455_BAL_TIME_STOP             0
#define     PL455_BAL_TIME_1S               1
#define     PL455_BAL_TIME_1MIN             2
#define     PL455_BAL_TIME_2MIN             3
#define     PL455_BAL_TIME_5MIN             4
#define     PL455_BAL_TIME_10MIN            5

#define     PL455_BALANCE_TIME_USED        PL455_BAL_TIME_1MIN


/* Cell voltage debonce */
#define	    PL455_CV_ERROR_DOBONCE_TIMES           10
#define	    PL455_CV_NORMAL_DOBONCE_TIMES          5

/* Packet voltage debonce */
#define	    PL455_PV_ERROR_DOBONCE_TIMES           10
#define	    PL455_PV_NORMAL_DOBONCE_TIMES          5

/* Packet Current debonce */
#define	    PL455_PC_ERROR_DOBONCE_TIMES           10
#define	    PL455_PC_NORMAL_DOBONCE_TIMES          5

/* Cell Tempperature debonce */
#define	    PL455_CT_ERROR_DOBONCE_TIMES           10
#define	    PL455_CT_NORMAL_DOBONCE_TIMES          5

/* PL455 Communicate status debonce */
#define	    PL455_COMM_ERROR_DOBONCE_TIMES         10
#define	    PL455_COMM_NORMAL_DOBONCE_TIMES        10

#define     PL455_COMM_ST_NORMAL                   0
#define     PL455_COMM_ST_ERROR                    1

#define     PL455_INIT_ST_NA                       0
#define     PL455_INIT_ST_ERR                      1
#define     PL455_INIT_ST_OK                       2

/* PL455 baudrate configure */
#define     PL455_BAUDRATE_125K                    0
#define     PL455_BAUDRATE_250K                    1
#define     PL455_BAUDRATE_500K                    2
#define     PL455_BAUDRATE_1000K                   3

#define     PL455_BAUDRATE_CFG                   PL455_BAUDRATE_250K



#define BAL_IDLE   		0
#define BAL_ODD 		1
#define BAL_EVEN 		2

typedef union
{
  unsigned char StatusByte ;

  struct
  {
    unsigned char sys_init            :1 ;
    unsigned char new_data            :1 ;
    unsigned char auto_mon_run        :1 ;
    unsigned char comm_reset          :1 ;

    unsigned char comm_clear          :1 ;
    unsigned char stack_fault_det     :1 ;
    unsigned char stack_fault         :1 ;
    unsigned char fault_condition     :1 ;
  }StatusBits;
}PL455_StatusTypes ;

typedef union
{
  unsigned short FaultSummaryWord ;

  struct
  {
    unsigned char                     :1 ;
    unsigned char                     :1 ;
    unsigned char                     :1 ;
    unsigned char                     :1 ;

    unsigned char                     :1 ;
    unsigned char                     :1 ;
    unsigned char Gpi_Fault_Sum       :1 ;
    unsigned char Chip_Fault_Sum      :1 ;

    unsigned char Sys_Fault_Sum       :1 ;
    unsigned char Comm_Fault_Sum      :1 ;
    unsigned char Cmpov_Fault_Sum     :1 ;
    unsigned char Cmpuv_Fault_Sum     :1 ;

    unsigned char Auxov_Fault_Sum     :1 ;
    unsigned char Auxuv_Fault_Sum     :1 ;
    unsigned char OV_Fault_Sum        :1 ;
    unsigned char UV_Fault_Sum        :1 ;
  }FaultSummaryBits;
}PL455_FaultSummaryTypes ;

struct PL455_AdcCorrectionType
{
  unsigned short PCGainOfChg ;
  unsigned short PCGainOfDischg ;
            char PCOffsetOfChg ;
            char PCOffsetOfDischg ;
  unsigned  short PVGain ;
            char PVOffset ;
};

typedef struct
{
	union
	{
		unsigned short byte ;
		struct bits
		{
			unsigned short SYS_INIT 		:1 ;
			unsigned short NEW_DATA 		:1 ;
			unsigned short AUTO_MON_RUN 	:1 ;
			unsigned short COMM_RESET 		:1 ;

			unsigned short COMM_CLEAR 		:1 ;
			unsigned short STACK_FAULT_DET 	:1 ;
			unsigned short STACK_FAULT 		:1 ;
			unsigned short FAULT_CONDITION 	:1 ;
		};
	}STATUS;

	union
	{
		unsigned short word ;
		struct bits
		{
			unsigned short STK_FAULT_ERR    :1 ;
			unsigned short RSVD0 			:2 ;
			unsigned short STOP_ERR 		:1 ;

			unsigned short RSVD1 			:1 ;
			unsigned short FRAME_ERR        :1 ;
			unsigned short CRC_FAULT_L		:1 ;
			unsigned short CRC_FAULT_H		:1 ;

			unsigned short ABORT_L 			:1 ;
			unsigned short ABORT_H 			:1 ;
			unsigned short EDGE_ERR_L		:1 ;
			unsigned short EDGE_ERR_H		:1 ;

			unsigned short COMP_FLT_L		:1 ;
			unsigned short COMP_FLT_H 		:1 ;
			unsigned short COMP_ERR_L 		:1 ;
			unsigned short COMP_ERR_H 		:1 ;
    	};
	}FAULT_COM;

	union
	{
		unsigned short word ;
		struct bits
		{
			unsigned short FACT_ECC_ERR 	:1 ;
			unsigned short FACT_ECC_COR 	:1 ;
			unsigned short USER_ECC_ERR 	:1 ;
			unsigned short USER_ECC_COR 	:1 ;

			unsigned short ADC_CAL_ERR 		:1 ;
			unsigned short RSVD 			:6 ;
			unsigned short HREF_GND_FAULT 	:1 ;

			unsigned short HREF_FAULT 		:1 ;
			unsigned short ANALOG_FAULT_ERR :1 ;
			unsigned short FACT_CKSUM_ERR 	:1 ;
			unsigned short USER_CKSUM_ERR 	:1 ;
		};
	}FAULT_DEV;

	union
	{
		unsigned char byte ;
		struct bits
		{
			unsigned char VP_CLAMP 			:1 ;
			unsigned char VP_FAULT 			:1 ;
			unsigned char VM_FAULT 			:1 ;
			unsigned char VDIG_FAULT 		:1 ;

			unsigned char INT_TEMP_FAULT 	:1 ;
			unsigned char VDIG_WAKE_FAULT 	:1 ;
			unsigned char COMM_TIMEOUT 		:1 ;
			unsigned char SYS_RESET 		:1 ;
		};
	}FAULT_SYS;

	union
	{
		unsigned short word ;
		struct bits
		{
			unsigned short RSVD 			:6 ;
			unsigned short GPI_FAULT_SUM    :1 ;
			unsigned short CHIP_FAULT_SUM 	:1 ;

			unsigned short SYS_FAULT_SUM 	:1 ;
			unsigned short COMM_FAULT_SUM 	:1 ;
			unsigned short CMPOV_FAULT_SUM 	:1 ;
			unsigned short CMPUV_FAULT_SUM 	:1 ;

			unsigned short AUXOV_FAULT_SUM 	:1 ;
			unsigned short AUXUV_FAULT_SUM 	:1 ;
			unsigned short OV_FAULT_SUM 	:1 ;
			unsigned short UV_FAULT_SUM 	:1 ;
		};
	}FAULT_SUM;

	unsigned char COMP_UV ;
	unsigned char COMP_OV ;
	unsigned short CELL_UV ;
	unsigned short CELL_OV ;
	unsigned short FAULT_AUX ;
	unsigned short FO_CTRL ;
	unsigned char  FAULT_GPI;

}PL455_FaultSenseType;

extern unsigned short PL455_CellVolt[MAX_PACKET_NUM][MAX_MOUDLE_NUM][MAX_CELL_NUM_PER_MOUDLE];
extern          short PL455_CellTemp[MAX_PACKET_NUM][MAX_MOUDLE_NUM][MAX_CELL_NUM_PER_MOUDLE];
extern          short PL455_PacketCurr[MAX_PACKET_NUM] ;
extern unsigned short PL455_PacketVolt[MAX_PACKET_NUM] ;

extern int PL455_DigDieTemp[MAX_PACKET_NUM][MAX_MOUDLE_NUM] ;
extern int PL455_AnalogDieTemp[MAX_PACKET_NUM][MAX_MOUDLE_NUM] ;
extern int PL455_VoltMoudle[MAX_PACKET_NUM][MAX_MOUDLE_NUM] ;

extern unsigned char PL455_CommErrSt[MAX_PACKET_NUM] ;
extern PL455_FaultSummaryTypes PL455_HWFaultSum[MAX_PACKET_NUM] ;
extern unsigned char PL455_InitSt[MAX_PACKET_NUM];

extern void PL455_Init(unsigned int packetNo);
extern int PL455_Task(unsigned int packetNo);
extern void PL455_Sleep(unsigned int packetNo);
extern void PL455_WakeUp(unsigned int packetNo);
extern int PL455_BalanceSet(int chs, int dura, int mNo);
extern int PL455_IsPowerIdle(int target_board );

extern int PL455_IsPowerNormal(int target_board ) ;
extern int PL455_IsOuvNormal(int target_board ) ;
extern int PL455_GetInitSt(unsigned int packetNo) ;
extern void PL455_SaveReg(unsigned int packetNo, char *buff) ;
extern int PL455_LoadCorrectPar(unsigned int packetNo, char *buff );
extern int PL455_GetCorrectPar(unsigned int packetNo);
extern int PL455_ResetCorrectPar(unsigned int packetNo ) ;
extern int PL455_GetPacketVlot(unsigned int packetNo ) ;
extern int PL455_GetPacketCurrent(unsigned int packetNo ) ;
extern int PL455_GetMux2Vlot(unsigned int packetNo ) ;
extern void PL455_TestBoardInit( unsigned int packetNo);
extern int PL455_CheckBal(unsigned int packetNo, int type ) ;
extern int PL455_CheckCellVolt_normal_st( void ) ;
extern int PL455_CheckCellTemp_normal_st( void ) ;
extern void PL455_Set_HWVersion(unsigned int packetNo) ;

#define PL455_DisableBal(mNo)      PL455_BalanceSet(0, 0, mNo)


#endif
