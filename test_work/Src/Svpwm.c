/*
Svpwm.c
    use DynaSinTable to calculate four sinusoid components in misalignment error of magnetic encoder

*/
#define TO_COMPILE_SVPWM_C
/*HW Interrupt Sharer Timer0*/
#include "../Inc/Svpwm.h"

int32_t SvpwmSinTable_S0_p[Svpwm_CHANNEL * 3] = { 0 };    // F2.1
int32_t SvpwmSinTable_S_p[Svpwm_CHANNEL * 3]  = { 0 };    // F2.1
int32_t SvpwmSinTable_s_p[Svpwm_CHANNEL] = { 0 };         // F2.1
int32_t SvpwmSinTable_TwoCos_p[Svpwm_CHANNEL] = { 0 };    // F3.1
uint16_t SvpwmSinTable_pwm_p[Svpwm_CHANNEL] = { 0 };      // F16
double SvpwmSinTable_Theta_p[Svpwm_CHANNEL] = { Svpwm_THETA0, Svpwm_THETA1 };

DynaSinTableStr_t SvpwmSinTable_str = { .FullScale      = Svpwm_FULL_SCALE ,
                                        .CountPerPeri   = Svpwm_ONE_PERIOD ,
                                        .Channel        = Svpwm_CHANNEL ,
                                        .DiviShift      = Svpwm_DIVISION_SHIFT };

SvpwmStr_t Svpwm_str = { .Channel = Svpwm_CHANNEL,    // channel oumber of sinusoid waves
                         .Omega = Svpwm_OMEGA };

void Svpwm_lay(SvpwmStr_t* Str_p, DynaSinTableStr_t* DynaStr_p, uint16_t* Pwm_p, double* Theta_p) {
    Str_p->DynaStr_p = DynaStr_p;    // net internal structure
    Str_p->Pwm_p = Pwm_p;            // net s array
    Str_p->Theta_p = Theta_p;        // net Theta array
    // net input of Str_p to input of DynaStr_p
    DynaStr_p->FullCountIn_p = Str_p->FullCountIn_p;
    
    /*initialize S0_p[],TwoCos_p[]*/
    for (int c = 0; c < DynaStr_p->Channel; c++) {
        SvpwmSinTable_TwoCos_p[c] = (int32_t)(cos(Str_p->Omega) * (1UL << 30));
        for (int j = 0; j < 3; j++) {
            SvpwmSinTable_S0_p[c * 3 + j] =
            (int32_t)(sin(Str_p->Theta_p[c] + (j - 1) * Str_p->Omega) * (1UL << 30));
        }
    }
}

uint8_t Svpwm_step(void* void_p) {
    SvpwmStr_t* Str_p = (SvpwmStr_t*)void_p;

    DynaSinTable_step((void*)Str_p->DynaStr_p);
    // I: maximun is 1024 uint16_t F16
    // s :-1 <= s <= 1 int16.7 F2.14
    // 1 : 0x01<<14 is 1 in F2.14
    // 2 :  >> 1
    // pwm = i_pwm : maximun is 1024 uint16/t F16
    // I*(1+s)/2 :  F16* (F2.14 +F2.14)>>1 =F17.15
    // pwm[i]=I*(1+s)/2: F16 <= (F16*(F2.14 +F2.14)>>1)>>1=(F16*(F2.14 +F2.14))>>2=F16.16

    int32_t sin_val;
    for (uint8_t i = 0; i < Str_p->Channel; i++) {
        
        
        sin_val = Str_p->DynaStr_p->s_p[i];
        
        if (sin_val > 0x3fffffff)
            sin_val = 0x3fffffff;
        else if (sin_val < -0x3fffffff)
            sin_val = -0x3fffffff;

        //10 for max gain 1024 , 15 for sin val Q2.30
        Str_p->Pwm_p[i] = (((int64_t)*Str_p->CurrentIn_p * (int64_t)sin_val) >> (10 + 15))+ 0x8000;
    }

    return 0;
}
