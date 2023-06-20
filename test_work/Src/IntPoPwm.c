/*
IntPoPwm.c
	use DynaSinTable to calculate four sinusoid components in misalignment error of magnetic encoder

*/
#define TO_COMPILE_LIB_C
#define FULL_SECTOR (8192)
#define HALF_SECTOR (4096)

#include "../Inc/IntPoPwm.h"
#include <stdio.h>

uint8_t IntPoPwm_step(void* void_p);
uint16_t IntPoPwm_PwmOut_p[IntPoPwm_CHANN] = { 0 };
int16_t IntPoPwm_Table_p[] = IntPoPwm_Table_INI;
uint8_t IntPoPwm_NextTask_p[IntPoPwm_NEXTNUM] = { 0 };
IntPoPwmStr_t IntPoPwm_str = { 0 };

inline int lagrange_weighting(int pos_3, int pos_1 ,int pos_2, int dut , int expon) {
	int molecular = (dut - pos_1) * (dut - pos_2);
	int denominator = (pos_3 - pos_1) * (pos_3 - pos_2) >> expon;
	return (molecular / denominator);
}

inline int lagrange_polynomial(int weight_1, int weight_2, int weight_3, int val_1, int val_2, int val_3 , int expon) {
	return (int)((weight_1 * val_1) + (weight_2 * val_2) + (weight_3 * val_3))>> expon;
}

void IntPoPwm_lay(IntPoPwmStr_t* Str_p, uint16_t* PwmOut_p, int16_t* Table_p, uint8_t* NextTask_p) {
	Str_p->NextTask_p = NextTask_p;
	Str_p->PwmOut_p = PwmOut_p; //net s array
	Str_p->Table_p = Table_p;
}

/*
*1圈步進馬達電子角為 4*2^13
*1圈機械角為 50*2^15
*單位轉換:(50*2^15)/2^14 = 100
*
*1圈外轉子馬達電子角為 6*2^13
*1圈機械角為 7*6*2^13
*單位轉換:(7*6*2^13)/2^14 = 21
*/
//實際為9459 但定點數算爆了
#define TOTAL_TIME 9490

uint8_t IntPoPwm_step(void* void_p) {
	IntPoPwmStr_t* Str_p = (IntPoPwmStr_t*)void_p;
	int32_t Au, Resi;/*F3.13 maximun 8 for 6 step or 4 step per electic cycle */
	int32_t Wtau0, Wtau1, zreo_vector, Temp;
	uint8_t Phase0, Phase1;

	//Au = ((((uint32_t)*Str_p->CountIn_p - Str_p->Alpha) * 21) % (Str_p->Chann << 14));
	Au = (((uint32_t)*Str_p->CountIn_p * (uint32_t)21) % ((uint32_t)Str_p->Chann << 14));
	Phase0 = Au >> 13;//Get the MSB 3bits as its phase
	Phase1 = Phase0 + 1;
	if (Phase1 == Str_p->Chann * 2)
		Phase1 = 0;
	// resi  電子角和第幾象限的差值(小數)，得到的結果是與x軸的夾角
	Resi = Au - ((uint16_t)Phase0 << 13);
	
	Temp = ((Resi*(Resi- FULL_SECTOR))>>16)*(Str_p->MdWt) >> 8;
	//for y-axis or 60deg
	Wtau0 = (((Resi- FULL_SECTOR)*(Resi- HALF_SECTOR))>>12)-Temp;
	//for x-axis or 0deg
	Wtau1 = ((Resi*(Resi- HALF_SECTOR))>>12) - Temp;


	/*
	int w_1 = lagrange_weighting(0, 4096, 8192, Resi, expon);
	int w_2 = lagrange_weighting(4096, 8192, 0, Resi, expon);
	int w_3 = lagrange_weighting(8192, 0, 4096, Resi, expon);
	int real = lagrange_polynomial(0, w_2, w_3,0,5792,8192, expon);
	uint16_t duty[2] = {0};
	*/
	//max period = 8192 * 2 /sqrt(3)，在30度時用相交點相加可以測出來
	int32_t Used_time = Wtau0 + Wtau1;

	zreo_vector = (TOTAL_TIME - (Used_time*(*Str_p->CurrentIn_p)>>10))>>1;
	//>>10 是因為pwm ICR 是 1024
	for (uint8_t ch = 0; ch < Str_p->Chann; ch++) {
		Str_p->PwmOut_p[ch] = ((*Str_p->CurrentIn_p) * 
			((((uint32_t)Str_p->Table_p[Phase0 * 3 + ch] * Wtau0)>>13) + 
			 (((uint32_t)Str_p->Table_p[Phase1 * 3 + ch] * Wtau1)>>13) +
			 (0))>>10) + 16;
	 
	}

	/*
		for (uint8_t i = 0; i < Str_p->NextNum ; i++)
		TRIG_NEXT_TASK(Str_p->NextTask_p[i]);
	*/

	return 0;
}

