/*
LLFeedBack.c
    use DynaSinTable to calculate four sinusoid components in misalignment error of magnetic encoder

*/
#ifndef LLFeedBack_H_INCLUDED
#define LLFeedBack_H_INCLUDED
#include <stdint.h>

typedef struct LLFeedBack {
    int16_t LLLimit;       //F7.9 Lead Lag angle count limit
    uint16_t LLLimit_Expon;  //F8.0 Lead Lag angle count limit exponential
    uint16_t Lambda;        //F0.16 accumulator forgetting factor
    uint16_t Lambda_Expon;   //F8.0 accumulator forgetting factor exponential
    int16_t LLKi;           //F0.9 K_i coeficient
    uint16_t LLKi_Expon;     //F8.0 K_i coeficient exponent 
	int16_t LLKp;     //F0.9 K_p coeficient
	uint16_t LLKp_Expon; //F8.0 K_p coeficient exponent
    uint16_t CountOut;      //F16  Count output  
    int16_t DiffCountAcc;   //F16.0 diff count accumulation
	int16_t staturate;
	int16_t LeadLag;
    volatile uint16_t* CountOut_p;   //F16  address of Count output 
    volatile int16_t* DiffCountAcc_p;//F16.0 address of diff count accumulation
    volatile uint16_t* CountIn_p;    //F16.0 address of one cycle encoder count
    volatile int16_t* DiffCountIn_p;  //F8.0 address of Cmd Sense Count Difference
}LLFeedBackStr_t;

/* Lead-Lag Feedback Contrallor */
// machanic and electronic angle ration 
#define M_E_RATIAL 7

// K_P 0~255 F0.9 = F8.0>>9
#define LLK_P_MANTISA 400
#define LLK_P_EXPON 8
#define LLK_P_divi 256		// =2^9

// 90 degree = FULL_SCALE/M_E_RATIAL/4 count = 2^14/7/4=585.14 count, 585.14*2^6=37449 count in Q6
// 因為具有機械誤差因此領先最大為680
#define MAX_LEAD_LAG_COUNT 700UL
#define MAX_LEAD_LAG_EXPON 6

// lambda 0.95 F0.16 0.95*2^16=62259
// lambda小於0.8會被罵 !!!
// lambda等於0.2一定會被罵 !!!
#define LAMBDA_MANTISA 58000        //0.8 forget_fector
#define LAMBDA_EXPON 16 

// K_I 0~255 F0.9=F8.0>>9 
#define LLK_I_MANTISA 55
#define LLK_I_EXPON 9
#define LLK_I_divi 512		// =2^9 
 
void LLFeedBack_lay(LLFeedBackStr_t* Str_p);
uint8_t LLFeedBack_step(void* void_p);
extern LLFeedBackStr_t LLFeedBack_str;

#define LLFeedBack_LAY(){\
	    LLFeedBack_lay(&LLFeedBack_str);\
    }

#endif
