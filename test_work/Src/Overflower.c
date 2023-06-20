/*
Overflower.c
    use DynaSinTable to calculate four sinusoid components in misalignment error of magnetic encoder

*/
#define TO_COMPILE_LIB_C
/*HW Interrupt Sharer Timer0*/
#include <stdio.h>
#include <stdint.h>
#include "../Inc/Overflower.h"

OverflowerStr_t Overflower_str = {
     .NextNum = OverFlower_NEXTNUM };

uint8_t OverFlowerNextTask_p[OverFlower_NEXTNUM] = { 0 };

void Overflower_lay(OverflowerStr_t* Str_p, uint8_t* NextTask_p)
{
    Str_p->NextTask_p = NextTask_p;
    Str_p->AccuOut_p = &Str_p->AccuOut;
    Str_p->FullScale = FULL_SCALE;
    Str_p->HalfScale = HALF_SCALE;
    Str_p->AccuOut = 0x80000000 + HALF_SCALE;
    Str_p->Count0 = HALF_SCALE;
}

uint8_t Overflower_step(void* void_p) {
    OverflowerStr_t* Str_p = (OverflowerStr_t*)void_p;
    int16_t DiffCount, Count;
    Count = (*Str_p->CountIn_p);

    DiffCount = Count - (int16_t)Str_p->Count0;

    Str_p->Count0 = Count;

    Str_p->AccuOut +=DiffCount;
    if (DiffCount > Str_p->HalfScale)
        Str_p->AccuOut -= Str_p->FullScale;
    if (DiffCount < -Str_p->HalfScale)
        Str_p->AccuOut += Str_p->FullScale;
    
    return 0;

    /*
        for (uint8_t i = 0; i < Str_p->NextNum; i++)
        TRIG_NEXT_TASK(Str_p->NextTask_p[i]);
    */
}

