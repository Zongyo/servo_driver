/* SinTable.c
*  Brifing: Sinusoid wave generator with correctional angles.
*           1. partition 1 cycle to 256 section
            2. 0,16,32,48,64,
            (-16,0,16)        (-sin(16u),0,sin(16u))

*           sin(a)=(a-0)(a-16)/(-16-0)(-16-16)(-sin(16u))+0+(a+16)(a-0)/(16+16)(16)(sin(16u))
*                   =-a(a-16)/(32*16) sin(16u)+a(a+16)/32*16 sin(16u)
*                   =(-a^2+16a+a^2+16a)/32*16 sin(16u)
*                   =32a/32*16 sin(16u)= a*sin(16u)>>4
*           sina(F16)= a(F16)*sin16u(F16) >>4
*           sina(F2.14)>>14=a(F8.8) * sin16u(F2.14)>>8>>14 >>4
            sina(F2.14)=a(F8.8) * sin16u(F2.14)>>(8+4)

*           (16,32,48)
            sin(a)=(a-32)(a-48)/(-32)(-16) sin(16u)+(a-16)(a-48)/(32-16)(32-48)sin(32u)+(a-16)(a-32)/(48-16)(48-32)sin(48u)
                = (a-32)(a-48)sin(16u)/(32*16)  +(a-16)(a-48)sin(32u)/(16)(-16)+(a-16)(a-32)sin(48u)/(32)(16)
                = ((a-32)(a-48)sin(16u)-2(a-16)(a-48)sin(32u)+(a-16)(a-32)sin(48u))>>9
            sina(F16)=((a(F16)-32(F16))(a(F16)-48(F16))sin16u(F16)
                    -2(a(F16)-16(F16))(a(F16)-48(F16))sin32u(F16)
                    +(a(F16)-16(F16))(a(F16)-32(F16))sin48u(F16))>>9

            sina(F2.14)>>14=((a(F8.8)-32<<8)>>8 (a(F8.8)-48<<8)>>8 sin16u(F2.14)>>14
                    -2(a(F8.8)-16<<8)>>8(a(F8.8)-48<<8)>>8 sin32u(F2.14)>>14
                    +(a(F8.8)-16<<8)>>8(a(F8.8)-32<<8)>>8 sin48u(F2.14)>>14)>>9
            sina(F2.14)=((a(F8.8)-32<<8)(a(F8.8)-48<<8)>>16 sin16u(F2.14)
                    -2(a(F8.8)-16<<8)(a(F8.8)-48<<8)>>16 sin32u(F2.14)
                    +(a(F8.8)-16<<8)(a(F8.8)-32<<8)>>16 sin48u(F2.14))>>9

            (48,64,80)
*           sin(a)=(a-64)(a-80)/(32*16)*sin(48u) + (a-48)(a-80)/(16*16)*sin(64u) + (a-48)(a-64)/(32*16)*sin(48u)
*                   =2(a-64)(a-64)*sin(48u)/(32*16) + (a-48)(a-80)*sin(64u) /(16*16)
*                   =(a-64)(a-64)*sin(48u)>>8  + (a-48)(a-80)*sin(64u)>>8
*                   =(((a-64)(a-64)*sin(48u)+ (a-48)(a-80))*sin(64u))>>8
* 
*           sina(F16)=(((a(F16)-64)(a(F16)-64)*sin48u(F16)+ (a(F16)-48)(a(F16)-80))*sin64u(F16))>>8
            sina(F2.14)>>14= (((a(F8.8)-64<<8)>>8 (a(F8.8)-64<<8)>>8) *sin48u(F2.14)>>14
                            + (a(F8.8)-48<<8)>>8 (a(F8.8)-80<<8)>>8) *sin64u(F2.14)>>14)>>8
            sina(F2.14)= (((a(F8.8)-64<<8)(a(F8.8)-64<<8)>>16) *sin48u(F2.14)
                            + (a(F8.8)-48<<8)(a(F8.8)-80<<8)>>16) *sin64u(F2.14))>>8
                        = (((a(F8.8)-64<<8)(a(F8.8)-64<<8)>>16) *sin48u(F2.14)
                            + (a(F8.8)-48<<8)(a(F8.8)-80<<8)>>16)*1<<14)>>8
            sina(F2.14) = (((a(F8.8)-64<<8)(a(F8.8)-64<<8)>>16) *sin48u(F2.14)
                            + (a(F8.8)-48<<8)(a(F8.8)-80<<8)>>2)>>8
            */

#define TO_COMPILE_LIB_C

#include "../Inc/SinTable.h"
#include <stdio.h>
#define PI 3.1415926

#define UNIT2RAD 2*PI/256

/* 1Sycle=256u = 360d => 1d=256/360 u
    D2U=256/360  for  1>>8=1/256 > 1/360> 1/512= 1>>9
    =>     1 > (D2U=(1/360)<<8) > 1>>1
    =>     1<<16 > (D2U=(1/360)<<8)<<16 > 1<<15
    =>     1<<16 > (D2UMAN=(1/360)<<24) > 1<<15
    D2UMAN= (1/360)<<24
    D2U= D2UMAN>>D2UEXP => D2UEXP=16
    u(F16) = d(F16) * D2U unit/deg =(d(F16) *D2UMAN)>>D2UEXP
    u(F8.8) = u(F16)<<8 =(d(F9.7)>>7 * D2UMAN)>>D2UEXP <<8
    =(d(F9.7) * D2UMAN)>>(D2UEXP-8+7)
*/
#define D2UMAN (1UL<< 16) / 360 
#define D2UEXP 16

SinTableStr_t SinTable_str = { 0 };

void SinTable_ini(SinTableStr_t* Str_p) {
    Str_p->sin16u = (int16_t)( sin(16 * UNIT2RAD) * 16384); //F2.14
    Str_p->sin32u = (int16_t)( sin(32 * UNIT2RAD) * 16384); //F2.14
    Str_p->sin48u = (int16_t)( sin(48 * UNIT2RAD) * 16384); //F2.14
    return;
}

int16_t SinTable(SinTableStr_t* Str_p, uint16_t Deg) {
    uint8_t zone = 0;
    int16_t S;
    uint16_t Half = 128 << 8, Quat = 64 << 8, au;//F8.8
    /* au(F8.8)=(d(F9.7) * D2UMAN)>>(D2UEXP-8+7)*/
    
    au = (uint32_t)Deg * D2UMAN;
    
    if (au > Half) {
        au = au - Half;   //image angle for mirror at half position
        zone = 1;           //Skew symmetric
    }
    if (au > Quat)
        au = -au + Half;   //image angle for mirror at quater position(-theta +180)

    //角度<= 1/16 圓
    if (au <= 16 << 8) {
        /*sina(F2.14)=a(F8.8) * sin16u(F2.14)>>(8+4)*/
        S = (((int32_t)au) * ((int32_t)Str_p->sin16u)) >> 12; 
    }
    //角度介於 1/16 到 3/16圓
    else if (au > (16 << 8) && au < (48 << 8)) {
        /*sina(F2.14)=((a(F8.8)-32<<8)(a(F8.8)-48<<8)>>16 sin16u(F2.14)
                    -2(a(F8.8)-16<<8)(a(F8.8)-48<<8)>>16 sin32u(F2.14)
                    +(a(F8.8)-16<<8)(a(F8.8)-32<<8)>>16 sin48u(F2.14))>>9*/
        S  = ((((((int32_t)au - (32 << 8)) * ((int32_t)au - (48 << 8))) >> 16) * ((int32_t)Str_p->sin16u)) -
              (((((int32_t)au - (16 << 8)) * ((int32_t)au - (48 << 8))) >> 15) * ((int32_t)Str_p->sin32u)) +
              (((((int32_t)au - (16 << 8)) * ((int32_t)au - (32 << 8))) >> 16) * ((int32_t)Str_p->sin48u)))>> 9 ;
    }

    else {
        /*sina(F2.14) = (((a(F8.8)-64<<8)(a(F8.8)-64<<8)>>16) *sin48u(F2.14)
                + (a(F8.8)-48<<8)(a(F8.8)-80<<8)>>2)>>8 
                  =(((a-64)(a-64)*sin(48u)+ (a-48)(a-80))*sin(64u))>>8 */
        S =  ((((int32_t)au - (64 << 8)) * ((int32_t)au - (64 << 8)) >> 14) * (int32_t)Str_p->sin48u -
               ((int32_t)au - (48 << 8)) * ((int32_t)au - (80 << 8)))  >> 10;//>>0 for sin(64)=2^14 2^14>>15 = >>1
    }

    if (zone == 1)
        S = -S;

    return(S);
}