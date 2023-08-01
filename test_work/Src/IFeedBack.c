/*
IFeedBack.c
	use DynaSinTable to calculate four sinusoid components in misalignment error of magnetic encoder

*/
#define TO_COMPILE_LIB_C
/*HW Interrupt Sharer Timer0*/
#include "../Inc/IFeedBack.h"
#define I_STATURATE ((MAX_CURRENT * Curr_K_I)<<Curr_K_I_EXPON)
IFeedBackStr_t IFeedBack_str = {
	.MaxCurr = MAX_CURRENT ,
	.MinCurr = MIN_CURRENT ,
	.CurrKp = Curr_K_P ,
	.CurrKi = Curr_K_I ,
	.CurrKp_Expon = Curr_K_P_EXPON ,
	.CurrKi_Expon = Curr_K_I_EXPON ,
	.staturate = (MAX_CURRENT<<Curr_K_I_EXPON)/Curr_K_I,
	.lamda = LAMDA
};

void IFeedBack_lay(IFeedBackStr_t* Str_p) {
	Str_p->CurrentOut_p = &Str_p->CurrentOut;
}
//�S���ֿn�~�t���ܼ�
//DIFF ��null
uint8_t IFeedBack_step(void* void_p) {

	IFeedBackStr_t* Str_p = (IFeedBackStr_t*)void_p;
	uint16_t Current;
	Str_p->DiffCountAcc += *Str_p->DiffCountIn_p;

	if (Str_p->DiffCountAcc > Str_p->staturate )
		Str_p->DiffCountAcc = Str_p->staturate;
	else if (Str_p->DiffCountAcc < -Str_p->staturate)
		Str_p->DiffCountAcc = -Str_p->staturate;
	
	Str_p->DiffCountAcc = Str_p->DiffCountAcc*Str_p->lamda;
	Current = abs(((Str_p->CurrKi * Str_p->DiffCountAcc) +
		Str_p->CurrKp * (*Str_p->DiffCountIn_p)) >> Str_p->CurrKi_Expon);
	if (Current > Str_p->MaxCurr)
		Current = Str_p->MaxCurr;
	else if (Current < Str_p->MinCurr)
		Current = Str_p->MinCurr;
	Str_p->CurrentOut = Current;//F16.0=F16.0+F7.9
	return 0;
}

