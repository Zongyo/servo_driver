/*
IntPoPwm.c
	use Dyna to calculate four sinusoid components in misalignment error of magnetic encoder
usage :
	0. Modify the configuration macro in this file to match your need
	1. Include this file
	2. Call IntPoPwm_LAY()  to layout IntPoPwm
	3. Connect IntPoPwm_str.TaskId to previous stage Task Block
	4. Connect IntPoPwm_str.CurrentIn_p,  to up-Stream Task Block
	5. Connect IntPoPwm_str.CountIn_p to up-Stream Task Block
	6. Connect IntPoPwm_str.PwmOut_p to down-Stream TaskBlock
*/

#ifndef IntPoPwm_H_INCLUDED
#define IntPoPwm_H_INCLUDED

#include <stdint.h>
#include <math.h>
#include "IntPoPwm_cfg_3c.h"


typedef struct IntPoPwm {
	uint8_t TaskId;
	uint8_t NextNum;
	uint8_t Chann;
	uint8_t OmegaMan; //F8.0 Omega Mentisa
	uint8_t OmegaExp; //F8.0 Omega Exponent 
	uint8_t Alpha; //F4.4 maximun 16
	int16_t MdWt;     //F2.14 middle component 
	/* input pointer*/
	volatile uint16_t* CurrentIn_p;//F1.15
	volatile uint16_t* CountIn_p; //F14.2 address of Single cycle encoder count 
	/* Output array pointer*/
	volatile uint16_t* PwmOut_p; //F10  
	/* Array pointer */
	volatile int16_t* Table_p; //F10  
	uint8_t* NextTask_p; //
}IntPoPwmStr_t;

void IntPoPwm_lay(IntPoPwmStr_t* Str_p, uint16_t* PwmOut_p, int16_t* Table_p, uint8_t* NextTask_p);

extern uint8_t IntPoPwm_step(void* void_p);
extern uint16_t IntPoPwm_PwmOut_p[];
extern int16_t IntPoPwm_Table_p[];
extern uint8_t IntPoPwm_NextTask_p[];
extern IntPoPwmStr_t IntPoPwm_str;

#define IntPoPwm_CFG_INI {						\
	IntPoPwm_str.NextNum=IntPoPwm_NEXTNUM;		\
	IntPoPwm_str.Chann=IntPoPwm_CHANN;			\
	IntPoPwm_str.OmegaMan=IntPoPwm_OMEGAMAN;	\
	IntPoPwm_str.OmegaExp=IntPoPwm_OMEGAEXP;	\
	IntPoPwm_str.Alpha=IntPoPwm_ALPHA;			\
	IntPoPwm_str.MdWt=IntPoPwm_MCOMP;			\
}

#define IntPoPwm_LAY() {								\
	IntPoPwm_CFG_INI;								\
	IntPoPwm_lay(&IntPoPwm_str, IntPoPwm_PwmOut_p,	\
	IntPoPwm_Table_p, IntPoPwm_NextTask_p);			\
}



//SYSPIPELINE_reg(IntPoPwm_step, &IntPoPwm_str, &IntPoPwm_str.TaskId, "IntPoPwm");

#endif 
//IntPoPwm_H_INCLUDED
