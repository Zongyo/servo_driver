/*
DynaSinTable.c
Dynamic Sin wave Look up Table
	DynaSinTable_lay() :
		connect DynaSinTable structure, Table initial signal vector S0_p, Signal Table matrix S_p, update parameter vector TwoCos_p and result signal vector
		Search for and initialize Signal Table matrix S_p and Table section vector N[]
	DynaSinTable_step() :
		recursive update Dynamic Signal talbe S_p[] and Table section vector N[]
		interpolation calculate signal vector s_p[] with current S_p[] and N[]
*/
#define TO_COMPILE_LIB_C

#include "../Inc/DynaSinTable.h"
#include <math.h>
extern DynaSinTableStr_t CompSinTable_str;

void DynaSinTable_lay(DynaSinTableStr_t* Str_p, int32_t* S0_p, int32_t* S_p, int32_t* TwoCos_p, int32_t* s_p) {
	uint8_t Channel = Str_p->Channel;
	uint16_t CountPP = Str_p->CountPerPeri;
	uint16_t FullScale = Str_p->FullScale;
	Str_p->S0_p = S0_p;//S0[0],S0[1],S0[2] is the sinusoid values sin(-p),sin(0),sin(+p)
	Str_p->S_p = S_p;
	Str_p->TwoCos_p = TwoCos_p;
	Str_p->s_p = s_p;
	Str_p->n = *Str_p->FullCountIn_p % FullScale;			//calculate initial one cycle count  
	uint16_t PERIOD = FullScale / CountPP;					//Total periods of one cycle
	uint16_t period = (Str_p->n) / CountPP;					//count is in which period of this cycle 
	Str_p->last_val = Str_p->n;
	if (period < (PERIOD >> 1)) {							//left half period 
		for (uint8_t i = 0; i < Channel; i++) {				//for all channels of sinusoid waves
			Str_p->S_p[i * 3 + 0] = Str_p->S0_p[i * 3 + 0];	// initial dynamic  table value S[0] by original table S0[1]
			Str_p->S_p[i * 3 + 1] = Str_p->S0_p[i * 3 + 1];	// initial dynamic table value S[1] by original table S0[2]
			Str_p->S_p[i * 3 + 2] = Str_p->S0_p[i * 3 + 2]; //iteration for S[2]
		}
		for (uint16_t p = 0; p <= period; p++) {				//iteration period times for table values S[0],S[1],S[2] to cover n
			for (uint8_t i = 0; i < Channel; i++) {
				Str_p->S_p[i * 3 + 0] = Str_p->S_p[i * 3 + 1];
				Str_p->S_p[i * 3 + 1] = Str_p->S_p[i * 3 + 2];
				Str_p->S_p[i * 3 + 2] = (((int64_t)Str_p->TwoCos_p[i] * (int64_t)Str_p->S_p[i * 3 + 1]) >> 29) - Str_p->S_p[i * 3 + 0];
			}
		}
		Str_p->N[0] = CountPP * period;				//Table count N[1]
		Str_p->N[1] = Str_p->N[0] + CountPP;		//Table count N[2]
		Str_p->N[2] = Str_p->N[1] + CountPP;		//Table count N[0]
	}

	else { //right half period 
		for (uint8_t i = 0; i < Channel; i++) {
			Str_p->S_p[i * 3 + 0] = Str_p->S0_p[i * 3 + 0]; //Take S0[1] as S[2] sin(0)
			Str_p->S_p[i * 3 + 1] = Str_p->S0_p[i * 3 + 1]; //Take S0[0] as S[1] sin(-p)
			Str_p->S_p[i * 3 + 2] = Str_p->S0_p[i * 3 + 2]; //Caculat S[0] sin(-2p)
		}
		for (uint16_t p = PERIOD -1; p >= period; p--) {//iteration down ward for periods to cover n
			for (uint8_t i = 0; i < Channel; i++) {
				Str_p->S_p[i * 3 + 2] = Str_p->S_p[i * 3 + 1];
				Str_p->S_p[i * 3 + 1] = Str_p->S_p[i * 3 + 0];
				Str_p->S_p[i * 3 + 0] = (((int64_t)Str_p->TwoCos_p[i] * (int64_t)Str_p->S_p[i * 3 + 1]) >> 29) - Str_p->S_p[i * 3 + 2];
			}
		}
		Str_p->N[2] = CountPP * period+ CountPP;
		Str_p->N[1] = Str_p->N[2] - CountPP;
		Str_p->N[0] = Str_p->N[1] - CountPP;
	}
	DynaSinTable_step((void*)Str_p);
}

extern uint8_t DynaSinTable_step(void* void_p) {
	DynaSinTableStr_t* Str_p = (DynaSinTableStr_t*)void_p;
	uint16_t  CountPP = Str_p->CountPerPeri;
	uint8_t  Channel = Str_p->Channel;
	uint16_t FullScale = Str_p->FullScale;
	Str_p->n = *Str_p->FullCountIn_p % FullScale;//calculate onecycle count 
	//uint16_t PERIOD = FullScale / CountPP;	//Total periods of one cycle
	int32_t R[3];//F1.31 maximum is 1 ratio 

	int16_t diff_count = (int16_t)Str_p->n - (int16_t)Str_p->last_val;
	Str_p->last_val = Str_p->n;

	if (diff_count> (FullScale >> 1) || diff_count < - (FullScale >> 1))
	{

		if (Str_p->N[2]   > (FullScale>>1) ){

			for (uint8_t i = 0; i < Channel; i++) {
				Str_p->S_p[i * 3 + 0] = Str_p->S0_p[i * 3 + 1];
				Str_p->S_p[i * 3 + 1] = Str_p->S0_p[i * 3 + 2];
				Str_p->S_p[i * 3 + 2] = (((int64_t)Str_p->TwoCos_p[i] * (int64_t)Str_p->S_p[i * 3 + 1]) >> 29) - Str_p->S_p[i * 3 + 0];
			}
			Str_p->N[0] = 0;
			Str_p->N[1] = Str_p->N[0] + CountPP;
			Str_p->N[2] = Str_p->N[1] + CountPP;
		}
		else if (Str_p->N[0] < (FullScale >> 1)) {
			for (uint8_t i = 0; i < Channel; i++) {
				Str_p->S_p[i * 3 + 2] = Str_p->S0_p[i * 3 + 1];
				Str_p->S_p[i * 3 + 1] = Str_p->S0_p[i * 3 + 0];
				Str_p->S_p[i * 3 + 0] = (((int64_t)Str_p->TwoCos_p[i] * (int64_t)Str_p->S_p[i * 3 + 1]) >> 29) - Str_p->S_p[i * 3 + 2];
			}
			Str_p->N[2] = FullScale;
			Str_p->N[1] = Str_p->N[2] - CountPP;
			Str_p->N[0] = Str_p->N[1] - CountPP;
		}
	}

	if(Str_p->n > Str_p->N[2]){
		for (int i = diff_count ; i > 0 ; i = i - CountPP)
		{
			for (uint8_t i = 0; i < Channel; i++) {
				Str_p->S_p[i * 3 + 0] = Str_p->S_p[i * 3 + 1];
				Str_p->S_p[i * 3 + 1] = Str_p->S_p[i * 3 + 2];
				Str_p->S_p[i * 3 + 2] = (((int64_t)Str_p->TwoCos_p[i] * (int64_t)Str_p->S_p[i * 3 + 1]) >> 29) - Str_p->S_p[i * 3 + 0];
			}
			Str_p->N[0] = Str_p->N[1];
			Str_p->N[1] = Str_p->N[2];
			Str_p->N[2] = Str_p->N[2] + CountPP;
		}
	}
	else if (Str_p->n < Str_p->N[0]) {
		for (int i = diff_count; i < 0; i = i + CountPP)
		{
			for (uint8_t i = 0; i < Channel; i++) {
				Str_p->S_p[i * 3 + 2] = Str_p->S_p[i * 3 + 1];
				Str_p->S_p[i * 3 + 1] = Str_p->S_p[i * 3 + 0];
				Str_p->S_p[i * 3 + 0] = ((int64_t)(Str_p->TwoCos_p[i] * (int64_t)Str_p->S_p[i * 3 + 1]) >> 29) - Str_p->S_p[i * 3 + 2];
			}
			Str_p->N[2] = Str_p->N[1];
			Str_p->N[1] = Str_p->N[0];
			Str_p->N[0] = Str_p->N[0] - CountPP;
		}
	}
	//interpolation F1.31 = (F32.0*F32.0)/(F32.0*F32.0)<<31=
	R[0] = ((Str_p->n - Str_p->N[1]) * (Str_p->n - Str_p->N[2])) >>1;
	R[1] = -((Str_p->n - Str_p->N[0]) * (Str_p->n - Str_p->N[2]));
	R[2] = ((Str_p->n - Str_p->N[0]) * (Str_p->n - Str_p->N[1])) >>1;

	for (uint8_t i = 0; i < Channel; i++) {
		Str_p->s_p[i] = ((int64_t)R[0] * (int64_t)Str_p->S_p[i * 3 + 0] + (int64_t)R[1] * (int64_t)Str_p->S_p[i * 3 + 1] +
			(int64_t)R[2] * (int64_t)Str_p->S_p[i * 3 + 2])>> Str_p->DiviShift;
	}

	return 0;
}