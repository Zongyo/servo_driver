/*
IFeedBack.c
    use DynaSinTable to calculate four sinusoid components in misalignment error of magnetic encoder

*/
#ifndef IFeedBack_H_INCLUDED
#define IFeedBack_H_INCLUDED
#include <stdint.h>
#include <stdlib.h>
typedef struct IFeedBack {
    uint16_t MaxCurr;   //F16.0 maximun current
    uint16_t MinCurr;   //F16.0 minimun current
    uint16_t CurrKi;     //F0.9 K_i coeficient
    uint8_t CurrKi_Expon; //F8.0 K_i coeficient exponent 
    uint16_t CurrKp;     //F0.9 K_p coeficient
    uint8_t CurrKp_Expon; //F8.0 K_p coeficient exponent 
    uint16_t CurrentOut; //F16  Current output
    //output pointers  
    volatile uint16_t* CurrentOut_p;//F16  address of Current output 
    //input pointers
    int32_t DiffCountAcc; //F16.0 address of one cycle encoder count
	int32_t staturate;
	float lamda;
    volatile int16_t* DiffCountIn_p; //F16.0 address of Cmd Sense Count Difference
}IFeedBackStr_t;

/* Current FeedbackControled */

//�̤j�Ȭ�1024�A��mosfet��X���F�A�Y�j���X�|���q��
// Maximun Current uint16_t F16.0
#define MAX_CURRENT 196UL

// Minimun Current uint16_t F10.6,  MIN_Current=MAX_Current/8
// min = 40
// speed min = 110
#define MIN_CURRENT 40
#define Curr_EXPON 0

// Curr_K_P_EXPON : gain 2^Curr_K_P_EXPON
#define Curr_K_P 512
#define Curr_K_P_EXPON 10

// Curr_K_I_EXPON : gain 2^Curr_K_I_EXPON
#define Curr_K_I 32
#define Curr_K_I_EXPON 10
#define LAMDA (0.96)

void IFeedBack_lay(IFeedBackStr_t* Str_p);
uint8_t IFeedBack_step(void* void_p);

extern IFeedBackStr_t IFeedBack_str;

#define IFeedBack_LAY(){\
	    IFeedBack_lay(&IFeedBack_str);\
}

#endif
