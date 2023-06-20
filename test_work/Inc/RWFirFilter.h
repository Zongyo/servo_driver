#pragma once
/*
RWFirFilter.h
    use DynaSinTable to calculate four sinusoid components in misalignment error of magnetic encoder
Usage :
    0: modify the RWFIR_TAP 8,RWFIR_DIVISHIFT 3 according to your need
    1: include this file into main()
    2. Call RWFirFilter_LAY()   to connect the input pin to signal source address
    3. connect RWFirFilter_str.TaskId to its previous-stage
    4. connect RWFirFilter_str.MCountIn_p to up-stream
    5. connect RWFirFilter_str.MCountOut_p to down-stream
*/
#ifndef RWFirFilter_H_INCLUDED
#define RWFirFilter_H_INCLUDED

#include <stdio.h>
#include <stdint.h>

#define RWFir_NEXTNUM 1
/* RWFirFilter*/
#define RWFIR_TAP 4
#define RWFIR_DIVISHIFT 2

typedef struct RWFirFilter {
    uint8_t TaskId;
    uint8_t NextNum;
    uint8_t Tap; //F8.0 Tap of FIR filter
    uint8_t DiviShift; //F8.0 shift to replace division 
    uint8_t Index; //F8.0 Index to Signal Buffer 
    uint32_t MCountAcc; //F32.0 multi turn encoder count accumulated value 
    uint16_t MCountOut; //F32.0 multi turn encoder count 
    uint16_t mask;
    //output pointers  
    volatile uint16_t* MCountOut_p;//F32  address of multi turn encoder count output 
    //input pointers
    volatile uint32_t* MCountIn_p; //F32 address of multi turn encoder countn iput 
    //Buffer array pointer
    volatile uint32_t* Buffer_p; //F32 address of signal buffer array
    volatile uint8_t* NextTask_p; //
}RWFirFilterStr_t;

void RWFirFilter_lay(RWFirFilterStr_t* Str_p, uint32_t* Buffer_p, uint8_t* NextTask_p);


extern uint8_t RWFirFilter_step(void* void_p);
extern RWFirFilterStr_t RWFirFilter_str;
extern uint32_t RWFirFilter_Buffer_p[];
extern uint8_t RWFir_NextTask_p[];


#define RWFirFilter_LAY() {													\
	RWFirFilter_lay(&RWFirFilter_str,RWFirFilter_Buffer_p,RWFir_NextTask_p);\
}

// SYSPIPELINE_reg(RWFirFilter_step,&RWFirFilter_str,&RWFirFilter_str.TaskId,"RWFIR");

#endif
