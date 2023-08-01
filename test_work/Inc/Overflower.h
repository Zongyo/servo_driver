/*
Overflower.c
    use DynaSinTable to calculate four sinusoid components in misalignment error of magnetic encoder
Usage:
    0. modify this file to match the full_scale of the input data
    1. include this file
    2. Call OVERFLOWER_LAY() to layout overflow TAsk Block
    3. connect Overflower_str.TaskId to its previous-stage
    4. connect Overflower_str.CountIn_p to up-stream
    5. connect Overflower_str.AccuOut_p to down-stream
*/
#ifndef Overflower_H_INCLUDED
#define Overflower_H_INCLUDED

#include <stdint.h>
/* F14.2*/
#define FULL_SCALE (1UL<<14)
#define HALF_SCALE (1UL<<13)

/* Overflower */
#define OverFlower_NEXTNUM 1

typedef struct Overflower {
    uint8_t TaskId;
    uint8_t NextNum;
    uint16_t FullScale; //F16.0 full scale of one turn encoder 
    int16_t HalfScale; //F16.0 half scale of one ture of encoder
    int16_t Count0;  //F16 old count of encoder
    uint32_t AccuOut; //F32 accumulated multiturn count of encoder
    /*Input pointer*/
	int16_t diffcount;
    volatile uint16_t* CountIn_p; //F16 address of newly readed count of encder
    //output pointers  
    volatile uint32_t* AccuOut_p;  //F32 address of accumulated multiturn count of encoder 
    volatile uint8_t* NextTask_p;
}OverflowerStr_t;

void Overflower_lay(OverflowerStr_t* Str_p, uint8_t* NextTask_p);

extern uint8_t Overflower_step(void* void_p);
extern OverflowerStr_t Overflower_str;
extern uint8_t OverFlowerNextTask_p[];


/*
    Overflower_lay()  layout overflower structure
    SYSTICK_reg()     regist this Task block to be excuted in system tick
    Overflower_str.CountIn_p=CountSource_p;   connect the input pin to source pin
*/
#define OVERFLOWER_LAY() {\
    Overflower_lay(&Overflower_str ,OverFlowerNextTask_p);\
    \
}
//SYSPIPELINE_reg(Overflower_step,&Overflower_str,&Overflower_str.TaskId,"Overflower");
#endif

