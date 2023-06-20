/*
 * encoder_correct.c
 *
 * Created: 2023/6/13 上午 01:10:54
 *  Author: ZongYo
 */ 
// path:
// C:\Users\ZongYo\Documents\Atmel Studio\7.0\test_work\test_work\Debug
// cmd:
// asaloader prog -p COM6 -f test_work.hex

#include "../Inc/main.h"
#include <util/delay.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>
/*
六部驅動
[1,0,0]
[1,1,0]
[0,1,0]
[0,1,1]
[0,0,1]
[1,0,1]
*/
#define DOS (60)
#define D2R (M_PI / 180)
typedef struct{
	uint16_t period;
	uint16_t va;
	uint16_t vb;
	uint16_t vc;
}svm_t;

int svm(svm_t* svm ,uint16_t magni ,float pahse){
	    uint8_t sector = pahse / DOS;
	    uint16_t deg = pahse - sector*DOS ;
	    uint16_t va,vb,vc;
		
	    if (sector>5){
		    sector=sector%6;
	    }
	    if (magni > svm->period){
		    magni=svm->period;
	    }
	    
	    uint16_t t2 = (float)magni*sin(deg * D2R);
	    uint16_t t1 = (float)magni*sin((60 - deg)*D2R);
	    int32_t t =t1+t2;
	    if (t>svm->period)
	    {
		    if (t1>svm->period)
		    {
			    t1=svm->period;
		    }
		    
		    if (t2>svm->period)
		    {
			    t2=svm->period;
		    }
	    }
	    
	    uint16_t t0 = (svm->period-t);
	    //half t0 for[0 0 0]another for [1 1 1]
	    t0 = t0/2;
		//會變成鞍波，但鞍波的0charge pump會沒電
		uint16_t offset = 64;
	    t0 = offset;   
		
	    switch (sector)
	    {
		    case 0: //T1*[1 0 0] T2*[1 1 0]
		    va = t + t0;
		    vb = t2 + t0;
		    vc = t0;
		    break;
		    case 1: //T1*[1 1 0] T2*[0 1 0]
		    va = t1 + t0;
		    vb = t + t0;
		    vc = t0;
		    break;
		    case 2: //T1*[0 1 0] T2*[0 1 1]
		    va = t0;
		    vb = t + t0;
		    vc = t2 + t0;;
		    break;
		    case 3: //T1*[0 1 1] T2*[0 0 1]
		    va = t0;
		    vb = t1 + t0;
		    vc = t  + t0;
		    break;
		    case 4: //T1*[0 0 1] T2*[1 0 1]
		    va = t2 + t0;
		    vb = t0;
		    vc = t + t0;
		    break;
		    case 5: //T1*[1 0 1] T2*[1 0 0]
		    va = t + t0;
		    vb = t0;
		    vc = t1 + t0;
		    break;
		    default:
		    va = 0;
		    vb = 0;
		    vc = 0;
		    return -1;
		    break;
	    }
	    svm->va = va;
	    svm->vb = vb;
	    svm->vc = vc;
	    return 0;
}


int main(void)
{
	hardware_init();
	uint16_t current = 0;
	uint16_t encoderCount = read_deg();
	svm_t moto;
	moto.period = 1024;
	volatile uint16_t pwm_val[3]={0};
	MOTO_ON();
	printf("\n\n");
	for(int i=0;i<128;i++){
		current++;
		svm(&moto,current,0);
		pwm_val[0] = moto.va;
		pwm_val[1] = moto.vb;
		pwm_val[2] = moto.vc;
		three_phase_out(pwm_val);
		_delay_ms(5);
	}
	printf("find the min phase a \n\n");
	uint16_t pre_data;
	pre_data = read_deg();
	pre_data = read_deg();
	for(int i=0;i<7;i++){
		for (int j=0 ; j<360 ; j += 15)
		{
			svm(&moto,current, j);
			pwm_val[0] = moto.va;
			pwm_val[1] = moto.vb;
			pwm_val[2] = moto.vc;
			three_phase_out(pwm_val);
			_delay_ms(80);
		}
		encoderCount = read_deg();
		encoderCount = read_deg();
		printf("%d %d\n",pre_data,encoderCount);
		if (encoderCount - pre_data> 8192){
			break;
		}
		else{
			pre_data = encoderCount;
		}
		
	}
	
	printf("start \n\n");
	for(int i=0;i<7*4;i++){
		for (int j=0 ; j<360 ; j++)
		{
			svm(&moto,current, j);
			pwm_val[0] = moto.va;
			pwm_val[1] = moto.vb;
			pwm_val[2] = moto.vc;
			three_phase_out(pwm_val);
			if (j % 60 == 0)
			{
				_delay_ms(80);
				encoderCount = read_deg();
				encoderCount = read_deg();
				printf("%5d\n",encoderCount);
			}
			else{
				_delay_ms(2);
			}
		}
	}
	MOTO_OFF();
}