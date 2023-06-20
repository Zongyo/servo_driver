#define TO_COMPILE_LIB_C
/*HW Interrupt Sharer Timer0*/
#include "../Inc/RWFirFilter.h"
#define MASK 16383
RWFirFilterStr_t RWFirFilter_str = {
    .NextNum = RWFir_NEXTNUM,
    .Tap = RWFIR_TAP,
    .DiviShift = RWFIR_DIVISHIFT
};
uint32_t RWFirFilter_Buffer_p[RWFIR_TAP] = { 0 };
uint8_t RWFir_NextTask_p[RWFir_NEXTNUM] = { 0 };

void RWFirFilter_lay(RWFirFilterStr_t* Str_p, uint32_t* Buffer_p, uint8_t* NextTask_p) {
    Str_p->NextTask_p = NextTask_p;
    Str_p->MCountOut_p = &Str_p->MCountOut;
    Str_p->Buffer_p = Buffer_p;
    Str_p->mask = MASK;
    for (uint8_t i = 0; i < Str_p->Tap; i++)
        RWFirFilter_step((void*)Str_p);
}

uint8_t RWFirFilter_step(void* void_p) {
    RWFirFilterStr_t* Str_p = (RWFirFilterStr_t*)void_p;
    Str_p->MCountAcc = Str_p->MCountAcc + *Str_p->MCountIn_p - Str_p->Buffer_p[Str_p->Index];

    Str_p->Buffer_p[Str_p->Index] = *Str_p->MCountIn_p;
    Str_p->Index++;
    if (Str_p->Index == Str_p->Tap)
        Str_p->Index = 0;
    Str_p->MCountOut = (Str_p->MCountAcc >> Str_p->DiviShift) & Str_p->mask;

    /*
        for (uint8_t i = 0; i < Str_p->NextNum; i++)
        TRIG_NEXT_TASK(Str_p->NextTask_p[i]);
    */
    return 0;
}