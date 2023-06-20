/*
Compensator.c
    use DynaSinTable to calculate four sinusoid components in misalignment error
of magnetic encoder usage: 0. modify the configulation macros in this file
    1. include this file
    2. Call Compensator_LAY() to layout Associate structure and array and regist
to system pipeline
    3. Connect Compensator_str.TaskId                   to previouse stage
TaskBlock
    4. Connect Input Compensator_str.FullCountIn_p      to up-Stream Task Block
    5. Connect Input Compensator_str.FullCountGould_p   to up-Stream Task Block
    6. Connect Outpue Compensator_str.Count_p           to Down-Stream Task
Block
    7. Connect Output Compensator_str.CountDiff_p       to Down-Stream Task
Block
*/

#ifndef Compensator_H_INCLUDED
#define Compensator_H_INCLUDED
#include <stdint.h>
#define Comp_NEXTNUM 1
#define Comp_CHANN 5

/*AMPn should be count in uint16_t Fx.Exp form*/
/*
#define Comp_AMPEXP 6
//DC = -362.857-415.357
#define DC_OFFSET (-161.176*(1<<Comp_AMPEXP))
#define Comp_AMP1 (int16_t)(7.10574*(1<<Comp_AMPEXP))
#define Comp_AMP2 (int16_t)(1.59083*(1<<Comp_AMPEXP))
#define Comp_AMP3 (int16_t)(0.71144*(1<<Comp_AMPEXP))
#define Comp_AMP4 (int16_t)(2.03023*(1<<Comp_AMPEXP))
#define Comp_AMP5 (int16_t)(0.918476*(1<<Comp_AMPEXP))
*/

#define Comp_AMPEXP 5
#define DC_OFFSET (int16_t)((-362.857-415.357)*(1UL<<Comp_AMPEXP))
#define Comp_AMP1 (int16_t)((6.79138*2)*(1UL<<Comp_AMPEXP))
#define Comp_AMP2 (int16_t)((2.56725*2)*(1UL<<Comp_AMPEXP))
#define Comp_AMP3 (int16_t)((0.74386*2)*(1UL<<Comp_AMPEXP))
#define Comp_AMP4 (int16_t)((2.22795*2)*(1UL<<Comp_AMPEXP))
#define Comp_AMP5 (int16_t)((1.06402*2)*(1UL<<Comp_AMPEXP))

#define Comp_OMEGAEXP 18
#define Comp_OMEGA1 (1*360)
#define Comp_OMEGA2 (2*360)
#define Comp_OMEGA3 (3*360)
#define Comp_OMEGA4 (4*360)
#define Comp_OMEGA5 (5*360)

/* ALPHAn should be count in Q16 form */
#define Comp_ALPHA1 (-139+90+360)
#define Comp_ALPHA2 (65+90)
#define Comp_ALPHA3 (60+90)
#define Comp_ALPHA4 (9+90)
#define Comp_ALPHA5 (-152+90+360)


#define MASK 16383
#include <stdint.h>
#include "SinTable.h"

/* compensator */
/*    use DynaSinTable to calculate four sinusoid components in misalignment
error of magnetic encoder usage: 0. modify the configulation macros in this file
    1. include this file
    2. Call Compensator_LAY() to layout Associate structure and array and regist
to system pipeline
    3. Connect Compensator_str.TaskId                   to previouse stage
TaskBlock
    4. Connect Input Compensator_str.FullCountIn_p      to up-Stream Task Block
    5. Connect Input Compensator_str.FullCountGould_p   to up-Stream Task Block
    6. Connect Outpue Compensator_str.Count_p           to Down-Stream Task
Block
    7. Connect Output Compensator_str.CountDiff_p       to Down-Stream Task
Block
*/

typedef struct Compensator {
    uint8_t TaskId;
    uint8_t Chann;
    uint8_t NextNum;
    int16_t DC_offset;  // Fx.AmpExp DC offset
    uint8_t AmpExp;  // Q8.0 group exponent of offset bias and sinusoid waves error
    uint8_t OmegaExp;   // Q8.0 group exponent Omega
    uint16_t mask;
    uint16_t Count;     // Q14.2 encoder count after compensation
    int16_t CountDiff;  // Q8.0 difference of goad count and sensed count
    /*Output pointer*/
    uint16_t* Count_p;  // Q14.2 compensated single cycle encoder count
    int16_t * CountDiff_p;  // Q14.2 difference of goad and sensed encoder count
    /* Array address*/
    uint16_t* Amp_p;  // Fx.AmpExp Amplitude of 0 to 4 mode
    volatile int16_t* Sin_p;  // Q2.14 sin values of all channels at the input count
    volatile uint16_t* Omega_p;  //((F12.4)>>OmegaExp) Deg/count
    volatile uint16_t* Alpha_p;  // Q16 phase angle in count
    SinTableStr_t* SinTableStr_p;
    /* Input pointer */
    volatile uint16_t* FullCountIn_p;    // Q32.0 address of multi cycle encoder
                                // count before compensated
    volatile uint16_t* FullCountGoad_p;  // Q32.0 address of position goad of
                                         // multi cycle encoder count
    uint8_t* NextTask_p;
} CompensatorStr_t;

void Compensator_lay(CompensatorStr_t* Str_p, SinTableStr_t* SinStr_p,
    uint16_t* Amp_p, int16_t* Sin_p, uint16_t* Omega_p,uint16_t* Alpha_p);

uint8_t Compensator_step(void* void_p);

extern uint16_t Compensator_Amp_p[];
extern int16_t Compensator_Sin_p[];
extern uint16_t Compensator_Omega_p[];
extern uint16_t Compensator_Alpha_p[];
extern uint8_t Comp_NextTask_p[];
extern CompensatorStr_t Compensator_str;

#define Compensator_CFG_INI                   \
{                                             \
	Compensator_str.OmegaExp = Comp_OMEGAEXP; \
	Compensator_str.Omega_p[0] = Comp_OMEGA1; \
	Compensator_str.Omega_p[1] = Comp_OMEGA2; \
	Compensator_str.Omega_p[2] = Comp_OMEGA3; \
	Compensator_str.Omega_p[3] = Comp_OMEGA4; \
	Compensator_str.Omega_p[4] = Comp_OMEGA5; \
	Compensator_str.Alpha_p[0] = Comp_ALPHA1; \
	Compensator_str.Alpha_p[1] = Comp_ALPHA2; \
	Compensator_str.Alpha_p[2] = Comp_ALPHA3; \
	Compensator_str.Alpha_p[3] = Comp_ALPHA4; \
	Compensator_str.Alpha_p[4] = Comp_ALPHA5; \
	Compensator_str.AmpExp = Comp_AMPEXP;     \
	Compensator_str.DC_offset = DC_OFFSET;    \
	Compensator_str.Amp_p[0] = Comp_AMP1;     \
	Compensator_str.Amp_p[1] = Comp_AMP2;     \
	Compensator_str.Amp_p[2] = Comp_AMP3;     \
	Compensator_str.Amp_p[3] = Comp_AMP4;     \
	Compensator_str.Amp_p[4] = Comp_AMP5;     \
	Compensator_str.NextNum = Comp_NEXTNUM;   \
	Compensator_str.Chann = Comp_CHANN;       \
}
#define Compensator_LAY()                                                   \
    {                                                                       \
        Compensator_lay(&Compensator_str, &SinTable_str, Compensator_Amp_p, \
                        Compensator_Sin_p, Compensator_Omega_p,             \
                        Compensator_Alpha_p);                               \
        Compensator_CFG_INI;                                                \
    }

// SYSPIPELINE_reg(Compensator_step,&Compensator_str,&Compensator_str.TaskId,"Compensator");

#endif  // Compensator_H_INCLUDED