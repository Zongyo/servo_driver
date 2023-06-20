/*
Svpwm.c
	use DynaSinTable to calculate four sinusoid components in misalignment error of magnetic encoder

*/
#ifndef Svpwm_H_INCLUDED
#define Svpwm_H_INCLUDED
#include "DynaSinTable.h" 
#include <stdio.h>

typedef struct Svpwm {
	uint8_t Channel;
	double Omega;
	uint16_t* CurrentIn_p;		//in F16.0 Ampliture input i  maximum is 1024 
	uint16_t* Pwm_p;			//F16 i_pwm of all channels maximum is 1024  
	double* Theta_p;			//
	uint32_t* FullCountIn_p;	//F32.0 address of multi cycle encoder count 
	DynaSinTableStr_t* DynaStr_p;
}SvpwmStr_t;


/* Space vector PWM */
//步進馬達電子角要轉25圈才能對位對到0度
#define Svpwm_FULL_SCALE (1<<13)
#define Svpwm_ONE_PERIOD (1<<5)
#define Svpwm_CHANNEL 2
#define Svpwm_DIVISION_SHIFT 10 
#define Svpwm_OMEGA  (2*PI*Svpwm_ONE_PERIOD* 25.0/Svpwm_FULL_SCALE)
/* Following data should be fill after identification*/
/* encoder count at the A phase excitation nearest next to the zero count of encoder*/
#define Svpwm_0A_count 0.0		//(A項偏移)
#define Svpwm_THETA0 (0.0 - 2*PI*Svpwm_0A_count*25/Svpwm_FULL_SCALE)
/* encoder count at the B phase excitation after the A phase*/
#define Svpwm_0B_count 0.0		//(B項偏移)
#define Svpwm_THETA1 (PI/2 - 2*PI*Svpwm_0B_count*25/Svpwm_FULL_SCALE)


void Svpwm_lay(SvpwmStr_t* Str_p, DynaSinTableStr_t* DynaStr_p, uint16_t* Pwm_p, double* Theta_p);
uint8_t Svpwm_step(void* void_p);

extern DynaSinTableStr_t SvpwmSinTable_str ;
extern int32_t SvpwmSinTable_S0_p[Svpwm_CHANNEL * 3]; //F2.30
extern int32_t SvpwmSinTable_S_p[Svpwm_CHANNEL * 3];  //F2.30
extern int32_t SvpwmSinTable_s_p[Svpwm_CHANNEL];    //F2.30
extern uint16_t SvpwmSinTable_pwm_p[Svpwm_CHANNEL];   //F16
extern int32_t SvpwmSinTable_TwoCos_p[Svpwm_CHANNEL]; //F3.29
extern double SvpwmSinTable_Theta_p[Svpwm_CHANNEL] ;
extern SvpwmStr_t Svpwm_str;

#define Svpwm_LAY(){\
		Svpwm_lay(&Svpwm_str,&SvpwmSinTable_str,SvpwmSinTable_Pwm_p,SvpwmSinTable_Theta_p);\
		DynaSinTable_lay(&SvpwmSinTable_str,SvpwmSinTable_S0_p,SvpwmSinTable_S_p,SvpwmSinTable_TwoCos_p, SvpwmSinTable_s_p);\
	}

#endif //Svpwm_H_INCLUDED
