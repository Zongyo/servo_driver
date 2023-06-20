#include <math.h>
/*
IntPoPwm_cfg_2c.h
	use DynaSinTable to calculate four sinusoid components in misalignment error of magnetic encoder
usage :
	0. Modify the configuration macro in this file to match your need
	1. Include this file
	2. Call IntPoPwm_LAY()  to layout IntPoPwm
	3. Connect IntPoPwm_str.TaskId to previous stage Task Block
	4. Connect IntPoPwm_str.CurrentIn_p,  to up-Stream Task Block
	5. Connect IntPoPwm_str.FullCountIn_p to up-Stream Task Block
	6. Connect IntPoPwm_str.PwmOut_p to down-Stream TaskBlock
*/
#define IntPoPwm_NEXTNUM 1
#define IntPoPwm_CHANN 3
/* Take Brushless motor with (8x6)48 Step as an example.
For 14bits encoder has 1<<14 counts
2^14 counts= 48 Step  => 1 count = 48/2^14 step
take 1/2 step as a uint => 1 step = 2 uint
1 count = 48/2^14 step => 1 count = 96/2^14 uint

omega= 96/2^14 uint/Count

unit = count * omega uint/count
unit(F16) =(count(F16) * OmegaMan) >> OmegaExp
unit(F3.13) =(count(F16) * OmegaMan) >> (OmegaExp-13)
unit(F3.13)%(2*Chann<<13) =((count(F16) * OmegaMan) >> (OmegaExp-13)) % (2*Chann<<13)
*/
#define OFFSET 0
#define IntPoPwm_OMEGAMAN  84
#define IntPoPwm_OMEGAEXP  14
/* IntPoPwm_ALPHA  is the Phase angle in Algular uint
 The 1'st A phase QCount to this positive side of the zero count of encoder
 for example 1'st A phase count is 2 then IntPoPwm_ALPHA is 2
*/
#define IntPoPwm_ALPHA  2

//theata = 30時沒有投影到T2
#define IntPoPwm_MCOMP (int16_t)(8192/sqrt(3))

/* for 10 bits 3 channel pwm 6 step
A:(1,0,0) -C:(1,1,0) B:(0,1,0) -A:(0,1,1) C:(0,0,1), -B(1,0,1)
 1:(1<<10)-1, 0: 0*/
#define PWM_PERIOD (886)

#define IntPoPwm_Table_INI {\
		PWM_PERIOD ,0         ,0         ,\
		PWM_PERIOD ,PWM_PERIOD,0         ,\
		0          ,PWM_PERIOD,0         ,\
		0          ,PWM_PERIOD,PWM_PERIOD,\
		0          ,0         ,PWM_PERIOD,\
		PWM_PERIOD ,0         ,PWM_PERIOD}

#include "IntPoPwm.h"
