
/* compensator */
/*OMEGAn, should be degree in uint16_t F9.7 form
 1'st mode   1 mechanical cycle= 360 Degree =1<<14 Count
 => 1Count=(360>>14) Degree
 => Omega1 =(360>>14) Degree/Count   1<<9=512 >360> 256=1<<8
    Omega1 =(360<<(16-9))>>14+16-9=(360<<7)>>21 Degree/Count
 => Omega1Man = 360<<7 (Q9.7)
    Omega1Exp = 14+7=21
 -------------------------------------------------------------
 2'nd mode   2*360 Degree = 1<<14 Count
 => 1Count=(360>>13) Degree
 => Omega2Man = 360<<7 (Q9.7)
    Omega2Exp = 13+7=20
 -------------------------------------------------------------
 3'th mode    3*360 Degree =1<<14count
 => 1Count = (3*360>>14) Degree        1<<11=2048 >3*360=1080> 1024=1<<10
    Omega3=(3*360<<(16-11))>>14+16-11=(3*360<<5)>>19
 => Omega3Man = 3*360<<5 (Q11.5)
    Omega3Exp = 14+5=19
 -------------------------------------------------------------
 4'th mode  4*360 Degree= 1<<14 Count
  => 1Count=(360>>12) Degree
 => Omega4Man = 360<<7 (Q9.7)
    Omega4Exp = 12+7=19
-------------------------------------------------------------
 unify Form of OmegaXMan to (F12.4)
    Omega1Man=360<<4   (Q12.4) OmegaExp=14+4=18
    Omega2Man=2*360<<4 (Q12.4) OmegaExp=14+4=18
    Omega3Man=3*360<<4 (Q12.4) OmegaExp=14+4=18
    Omega4Man=4*360<<4 (Q12.4) OmegaExp=14+4=18
    d(Q16 Degree) =(count(Q16)* Omega(Q12.4)) >> OmegaExp
    d(Q9.7 Degree) =(count(Q16)* Omega(Q12.4)) >> (OmegaExp-7)
*/

#define TO_COMPILE_LIB_C
#include "../Inc/Compenastor_2.h"
#include <stdio.h>
#define HALF_CYCLE 8192
#define FULL_CYCLE 16384


uint16_t Compensator_Amp_p[Comp_CHANN] = { 0 };    // Q7.9
int16_t Compensator_Sin_p[Comp_CHANN] = { 0 };     // Q2.14
uint16_t Compensator_Omega_p[Comp_CHANN] = { 0 };  // Q12.4
uint16_t Compensator_Alpha_p[Comp_CHANN] = { 0 };  // Q16
uint8_t Comp_NextTask_p[Comp_NEXTNUM] = { 0 };
CompensatorStr_t Compensator_str = { 0 };

void Compensator_lay(CompensatorStr_t* Str_p, SinTableStr_t* SinTable_str,
    uint16_t* Amp_p, int16_t* Sin_p, uint16_t* Omega_p,
    uint16_t* Alpha_p) {
    Str_p->CountDiff_p = &Str_p->CountDiff;  // net output pointer
    Str_p->Count_p = &Str_p->Count;          // net output pointer
    Str_p->Amp_p = Amp_p;                    // net internal A array
    Str_p->Sin_p = Sin_p;                    // net s array
    Str_p->Alpha_p = Alpha_p;                // net Alpha array
    Str_p->Omega_p = Omega_p;                // net Omega array
    Str_p->SinTableStr_p = SinTable_str;    // net SinTable_str array
    Str_p->mask = MASK;
    // Str_p->NextTask_p = NextTask_p;
}

uint8_t Compensator_step(void* void_p) {
    CompensatorStr_t* Str_p = (CompensatorStr_t*)void_p;
    uint16_t Deg, FullDeg;  // Q9.7
    FullDeg = 360;          // Q9.7
    int32_t Compen;
    Compen = Str_p->DC_offset;  // Fx.AmpExp, x=16-AmpExp

    for (uint8_t ch = 0; ch < Str_p->Chann; ch++) {
        Deg = ((uint32_t)Str_p->Omega_p[ch]  * (uint32_t)(*Str_p->FullCountIn_p)>>14)
              + Str_p->Alpha_p[ch];
        if (Deg > FullDeg) 
            Deg = Deg % FullDeg;

        Str_p->Sin_p[ch] = SinTable(Str_p->SinTableStr_p, Deg);
        /*Fx.AmpExp=Fx.AmpExp+ (Fx.AmpExp * Q2.14)>>14 */
        Compen += (((int32_t)Str_p->Sin_p[ch] * (int32_t)Str_p->Amp_p[ch]) >> (14));
    }
    Compen = Compen >> Comp_AMPEXP;
    Str_p->Count = *Str_p->FullCountIn_p + Compen ;
    Str_p->Count = Str_p->Count & Str_p->mask;
	
	/*	//change for cmd buffer
	    Str_p->CountDiff = *Str_p->FullCountGoad_p - Str_p->Count;
	    if (Str_p->CountDiff> HALF_CYCLE)
	    Str_p->CountDiff -= FULL_CYCLE;
	    else if (Str_p->CountDiff < -HALF_CYCLE)
	    Str_p->CountDiff += FULL_CYCLE;
	*/
	
    // for (uint8_t i = 0; i < str_p->NextTaskNum, i++)
    // TRIG_NEXT_TASK(Str_p->NextTask_p[i]);
    return 0;
}
