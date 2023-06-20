/*
LLFeedBack.c
	use DynaSinTable to calculate four sinusoid components in misalignment error of magnetic encoder

*/
#define TO_COMPILE_LIB_C
#include "../Inc/LLFeedBack.h"
#include <stdio.h>

LLFeedBackStr_t LLFeedBack_str = {
	.LLLimit = MAX_LEAD_LAG_COUNT,
	.LLLimit_Expon = MAX_LEAD_LAG_EXPON,
	.Lambda = LAMBDA_MANTISA,
	.Lambda_Expon = LAMBDA_EXPON,
	.LLKi = LLK_I_MANTISA,
	.LLKi_Expon = LLK_I_EXPON };

void LLFeedBack_lay(LLFeedBackStr_t* Str_p) {
	Str_p->CountOut_p = &Str_p->CountOut;
	Str_p->DiffCountAcc_p = &Str_p->DiffCountAcc;
	Str_p->DiffCountAcc = 0;
}

//SATURATE = ( (MAX_LEAD_LAG_COUNT) *( LLK_I_divi) / LLK_I_MANTISA)
#define SATURATE 1497 

uint8_t LLFeedBack_step(void* void_p) {
	LLFeedBackStr_t* Str_p = (LLFeedBackStr_t*)void_p;
	int16_t LeadLag;
	Str_p->DiffCountAcc =(((int32_t)Str_p->Lambda * (int32_t)Str_p->DiffCountAcc) >> Str_p->Lambda_Expon ) + *Str_p->DiffCountIn_p;
	
	if (Str_p->DiffCountAcc > SATURATE)
	{
		Str_p->DiffCountAcc = SATURATE;
	}
	else if (Str_p->DiffCountAcc < -SATURATE) {
		Str_p->DiffCountAcc = -SATURATE;
	}

	//為何有負號
	//diff 為int8 , int8 +-2.81度
	//Str_p->DiffCountAcc = 0.95*Str_p->DiffCountAcc + *Str_p->DiffCountIn_p;
	LeadLag = ((int32_t)Str_p->LLKi * Str_p->DiffCountAcc) >> Str_p->LLKi_Expon;//F7.9 <= F0.9*F16.0=(F8.0*F16.0)>>9=F15.9
	
	if (LeadLag > Str_p->LLLimit)
		LeadLag = Str_p->LLLimit;
	else if (LeadLag < -Str_p->LLLimit)
		LeadLag = -Str_p->LLLimit;

	Str_p->CountOut = *Str_p->CountIn_p + LeadLag;//F16.0=F16.0+F7.9
	return 0;
}

