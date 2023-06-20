#pragma once
/*
MoCmdBuff.c

*/
#ifndef MoCmdBuff_H_INCLUDED
#define MoCmdBuff_H_INCLUDED
#include <stdint.h>
#include "MoCmdBuff_cfg_Brushless.h"

#define MoCmd_NEXTNUM 10
#define JERK_LIMIT 10
#define JERK_EXPON 10
#define ACCEL_LIMIT  10
#define ACCEL_EXPON  10
#define VELO_EXPON 10
#define POSI_LIMIT 1024

typedef struct MoCmdBuff {
    uint8_t SyncTaskId;
    uint8_t UpDaTaskId;
    uint8_t ExecTaskId;
    uint8_t NextNum;
    uint8_t Mode;       //F8.0 control mode 
    uint16_t JerkLmt;   //Fx.JerkExpon Jerk limitation 
    int16_t Acc2Adj;    //Fx.JerkExpon accelation adj wait for execution
    uint8_t JerkExpon;  //F8.0 Jerk Exponent
    uint16_t AccLmt;    //Fx.AccExpon accelation limitation 
    int16_t AccCmd;     //Fx.AccExpon accelation command
    int16_t Vilo2Adj;   //Fx.AccExpon Vilocity adj wait for execution
    int16_t AccBasic;   //Fx.AccExpon Basic Acceration
    uint8_t AccExpon;   //F8.0 AccExpon Exponent
    uint16_t ViloLmt;   //Fx.ViloExpon Vilocity limitation
    int16_t ViloCmd;    //Fx.ViloExpon  Vilocity command
    int16_t PosiMore;    //F8.0 Relative Position to move    
    int16_t Posi2Adj;   //Fx.ViloExpon position adj wait for execution 
    int16_t ViloBasic;  //Fx.ViloExpon Basic Vilocity
    uint8_t ViloExpon;  //F8.0 ViloExpon Exponent
    uint16_t PosiGoad;  //F32.0 Position Goad to control
    uint16_t PosiLmt;   //F32.0 Position Lmt
    uint16_t PosiCmd;    //F32.0 Position command
	int16_t error_out;
	int16_t *error_out_p;
    volatile int32_t SyncCmd;    //Syncronize Command depend on mode it can be AccCmd,ViloCmd+PosiMore, or PosiCmd
    //output  pointers  
    volatile uint16_t* PosiGoad_p;//F32  address of Goad Position to control
    //input pointers
    volatile uint16_t* PosiSensIn_p;//F32  address of Position Sensed
    uint8_t* NextTask_p; //
}MoCmdBuffStr_t;

void MoCmdBuff_lay(MoCmdBuffStr_t* Str_p,uint16_t init_angle , uint8_t* NextTask_p);

uint8_t MoCmdBuffSync_step(void* void_p);
uint8_t MoCmdBuffExec_step(void* void_p);
uint8_t MoCmdBuffUpda_step(void* void_p);

extern uint8_t MoCmd_NextTask_p[];
extern MoCmdBuffStr_t MoCmdBuff_str;

#define MoCmdBuff_LAY {\
	MoCmdBuff_lay(&MoCmdBuff_str,MoCmd_NextTask_p);\
    SYSPIPELINE_reg(MoCmdBuffSync_step,&MoCmdBuff_str,&MoCmdBuff_str.SyncTaskId,"SyncCmd");\
    SYSPIPELINE_reg(MoCmdBuffUpda_step,&MoCmdBuff_str,&MoCmdBuff_str.UpDaTaskId,"UpdateCmd");\
    SYSPIPELINE_reg(MoCmdBuffExec_step,&MoCmdBuff_str,&MoCmdBuff_str.ExecTaskId,"ExecCmd");\
}
#endif

