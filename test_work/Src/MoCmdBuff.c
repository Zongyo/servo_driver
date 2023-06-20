/*
MoCmdBuff.c
	use

*/
#define TO_COMPILE_LIB_C
/*HW Interrupt Sharer Timer0*/

#include "../Inc/MoCmdBuff.h"
#include <math.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#define sgn(x) (((x) < (0)) ? (-1):(1))
#define min(a,b) ((a)>(b)? (b):(a))


#define HALF_CYCLE 8192
#define FULL_CYCLE 16384

void MoCmdBuff_lay(MoCmdBuffStr_t* Str_p, uint16_t initial_angle ,uint8_t* NextTask_p) {
	Str_p->NextTask_p = NextTask_p;
	Str_p->PosiGoad_p = &Str_p->PosiGoad;
	Str_p->error_out_p = &Str_p->error_out;
	Str_p->PosiGoad = initial_angle;
}

//position control under asinged velocity limit move to the assigned position
//velocity control fallow the velocity command and adjust position when need

#define ACCELERATION_CNT 0
#define VELOCITY_CNT 1
#define POSITION_CNT 2


MoCmdBuffStr_t MoCmdBuff_str = {
	.NextNum = MoCmd_NEXTNUM,
	.JerkLmt = JERK_LIMIT,
	.JerkExpon = JERK_EXPON,
	.AccLmt = ACCEL_LIMIT,
	.AccExpon = ACCEL_EXPON,
	.ViloLmt = VELO_LIMIT,
	.ViloExpon = VELO_EXPON,
	.PosiLmt = POSI_LIMIT,
	.ViloCmd = VELO_LIMIT,
	.Posi2Adj = 0
};
uint8_t MoCmd_NextTask_p[MoCmd_NEXTNUM] = { 0 };


uint8_t MoCmdBuffSync_step(void* void_p) { 
	return 0;
	}

uint8_t MoCmdBuffUpda_step(void* void_p) {
	return 0;
}
//uint8_t MoCmdBuffUpda_step(void* void_p) {
//	MoCmdBuffStr_t* Str_p = (MoCmdBuffStr_t*)void_p;
//	switch (Str_p->Mode) {
//	case ACCELERATION_CNT:
//		/* in acceleration adjust mode, Jerk mode, provide new AccCmd, Jerk=AccCmd-AccBasic*/
//		if (Str_p->AccCmd > 0) {
//			Str_p->Acc2Adj = (min(Str_p->AccCmd, Str_p->AccLmt) - Str_p->AccBasic) << (Str_p->JerkExpon - Str_p->AccExpon);
//		}
//		
//		else {
//			Str_p->Acc2Adj = (max(Str_p->AccCmd, -Str_p->AccLmt) - Str_p->AccBasic) << (Str_p->JerkExpon - Str_p->AccExpon);
//		}
//		break;
//	case VELOCITY_CNT:
//		/* in velocity adjust mode, Acc mode, provide new velocity, Acc=ViloCmd-ViloBasic*/
//		if (Str_p->ViloCmd > 0) {
//			Str_p->Vilo2Adj = (min(Str_p->ViloCmd, Str_p->ViloLmt) - Str_p->ViloBasic) << (Str_p->AccExpon - Str_p->ViloExpon);
//		}
//		else {
//			Str_p->Vilo2Adj = (max(Str_p->ViloCmd, -Str_p->ViloLmt) - Str_p->ViloBasic) << (Str_p->AccExpon - Str_p->ViloExpon);
//		}
//		/* in velocity adjust mode allow adjust phase, more positi)*/
//		Str_p->Posi2Adj = Str_p->Posi2Adj + Str_p->PosiMore << Str_p->ViloExpon;
//		Str_p->PosiMore = 0; //reset PosiMore after adjust Posi
//		break;
//	case POSITION_CNT:
//		/* in position adjust mode only positive Posicmd allow Posi2Adj= differnce between command and sensed position*/
//		Str_p->Posi2Adj = min(Str_p->PosiCmd, Str_p->PosiLmt) - *Str_p->PosiSensIn_p;
//		break;
//	}
//	
//		//for (uint8_t i = 0; i < Str_p->NextNum; i++)
//		//TRIG_NEXT_TASK(Str_p->NextTask_p[i]);
//	
//	return 0;
//}



//uint8_t MoCmdBuffExec_step(void* void_p) {
//	MoCmdBuffStr_t* Str_p = (MoCmdBuffStr_t*)void_p;
//	//用系統極限的(速度)去限制突變的(位置cmd)，被限制的(位置cmd)加上背景(位置)
//	int16_t Vilom=0;
//	Str_p->PosiCmd = Str_p->PosiCmd + Str_p->PosiMore;
//	Str_p->PosiMore = 0;
//	Str_p->ViloCmd = Str_p->PosiCmd - *Str_p->PosiSensIn_p;
//
//	if (Str_p->ViloCmd > HALF_CYCLE)
//		Str_p->ViloCmd -= FULL_CYCLE;
//	else if (Str_p->ViloCmd < -HALF_CYCLE)
//		Str_p->ViloCmd += FULL_CYCLE;
//
//	Vilom = min(abs(Str_p->ViloCmd), Str_p->ViloLmt) * sgn(Str_p->ViloCmd);
//	Str_p->Posi2Adj = Str_p->ViloCmd - Vilom;
//	/*unstable cmd */
//	//Str_p->PosiGoad = (Str_p->PosiGoad + Vilom) % FULL_CYCLE;
//	//Str_p->PosiGoad= Str_p->PosiGoad % FULL_CYCLE;
//
//	/*stable cmd */
//	Str_p->PosiGoad = (Str_p->PosiCmd - Str_p->Posi2Adj) % FULL_CYCLE;
//	Str_p->PosiGoad= Str_p->PosiGoad % FULL_CYCLE;
//
//	Str_p->error_out = Str_p->PosiGoad - *Str_p->PosiSensIn_p;
//	if (Str_p->error_out > HALF_CYCLE)
//		Str_p->error_out -= FULL_CYCLE;
//	else if (Str_p->error_out < -HALF_CYCLE)
//		Str_p->error_out += FULL_CYCLE;
//
//	//
//	//作為伺服馬達控制角度落在0~270 因此要限制，360旋轉的馬達不需要
//	/*
//		if (Str_p->PosiLmt != 0 && Str_p->PosiGoad > Str_p->PosiLmt)
//		Str_p->PosiGoad = Str_p->PosiLmt;
//	*/
//	return 0;
//}


uint8_t MoCmdBuffExec_step(void* void_p) {
	MoCmdBuffStr_t* Str_p= (MoCmdBuffStr_t*) void_p;
	int16_t Vilo,Vilom;
	Str_p->ViloBasic = 0;

	
	
	Str_p->Posi2Adj = (int16_t)Str_p->PosiCmd - *Str_p->PosiSensIn_p;
	
	Vilo=Str_p->ViloBasic + Str_p->Posi2Adj;			//Vilo include the basic Vilo and more for adjust position
	Vilom=min(Str_p->ViloCmd, abs(Vilo)) * sgn(Vilo);	//Vilo can not exceed limitation
	Str_p->Posi2Adj=Str_p->Posi2Adj-Vilom;				//Posi to Adjust subtract the amount executed
	Str_p->PosiGoad= Str_p->PosiGoad+Vilom;				//Accumu vilo to get position
	Str_p->PosiGoad= Str_p->PosiGoad % FULL_CYCLE;
	
	if (abs((int16_t)Str_p->PosiCmd - (int16_t)Str_p->PosiGoad) < VELO_LIMIT)
	{
		Str_p->PosiGoad = Str_p->PosiCmd;
	}


	Str_p->error_out = Str_p->PosiGoad - *Str_p->PosiSensIn_p;
	if (Str_p->error_out > HALF_CYCLE)
		Str_p->error_out -= FULL_CYCLE;
	else if (Str_p->error_out < -HALF_CYCLE)
		Str_p->error_out += FULL_CYCLE;
	return 0;
}