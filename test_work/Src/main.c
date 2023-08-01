 //path:
 //C:\Users\ZongYo\Documents\Atmel Studio\7.0\test_work\test_work\Debug
 //cmd:
 //asaloader prog -p COM6 -f test_work.hex

#include "../Inc/main.h"
#include <util/delay.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>

volatile uint16_t sys_tick=0 ,sys_clk =0 ;
volatile uint8_t pipline_triger = 0 , task_finish = 0;
int16_t current[3] ={0};
uint8_t error_code=0;
uint16_t encoderCount = 0;

uint16_t voltage_cmd = 128;
int16_t diff_zero =0;

ISR (TIMER1_OVF_vect){
	sys_tick++;
	if (sys_tick == 16){
		sys_clk++;
		sys_tick = 0;
		pipline_triger = 1;
	}
}

uint8_t moto_control(){
	PORTD = PORTD | (1<<4);
	pipline_triger = 0;
	encoderCount = read_deg();
	for (uint8_t i=0;i<4;i++){
		encoderCount = read_deg();
		Overflower_step(&Overflower_str);
		RWFirFilter_step(&RWFirFilter_str);
	}
	/* currtent protect */
	current[0] = ((int16_t)get_ADC(0)-512);
	current[1] = ((int16_t)get_ADC(1)-512);
	current[2] = -current[0]-current[1];
	if (abs(current[0])>510 || abs(current[1])>510 || abs(current[2])>510){
		error_code = 1;
		return 1;
	}//over current

	Compensator_step(&Compensator_str);
	MoCmdBuffExec_step_2(&MoCmdBuff_str);
	IFeedBack_step(&IFeedBack_str);
	LLFeedBack_step(&LLFeedBack_str);
	IntPoPwm_step(&IntPoPwm_str);
	three_phase_out(IntPoPwm_PwmOut_p);
	PORTD = PORTD & (~(1<<4));
	task_finish = 1;
	return 0;
}

int main(void)
{
	hardware_init();
    Overflower_str.CountIn_p = &encoderCount;   
	RWFirFilter_str.MCountIn_p = &Overflower_str.AccuOut;
	Compensator_str.FullCountIn_p = &RWFirFilter_str.MCountOut;
	MoCmdBuff_str.PosiSensIn_p = &Compensator_str.Count;
	LLFeedBack_str.CountIn_p = &Compensator_str.Count;
	LLFeedBack_str.DiffCountIn_p = &MoCmdBuff_str.error_out;
	IFeedBack_str.DiffCountIn_p = &MoCmdBuff_str.error_out;
	IntPoPwm_str.CurrentIn_p = &IFeedBack_str.CurrentOut;
	IntPoPwm_str.CountIn_p = &LLFeedBack_str.CountOut;

	encoderCount = read_deg();
	encoderCount = read_deg();
	OVERFLOWER_LAY();
	Overflower_step(&Overflower_str);
	RWFirFilter_LAY();
	SinTable_LAY();
	LLFeedBack_LAY();
	IFeedBack_LAY();
	Compensator_LAY();
	IntPoPwm_LAY();

	//cmd_buffer initial
	Compensator_step(&Compensator_str);
	MoCmdBuff_lay(&MoCmdBuff_str,Compensator_str.Count , MoCmd_NextTask_p);

	/* sensing initial voltage */
	get_ADC(2);
	float suply_voltage = 0;
	for(int i=0 ; i<16 ; i++){
		suply_voltage += get_ADC(2);
	}
	
	/*for open loop test*/
	//LLFeedBack_str.CountIn_p = &MoCmdBuff_str.PosiGoad;
	//LLFeedBack_str.DiffCountIn_p = &diff_zero;
	//IFeedBack_str.DiffCountIn_p  = &diff_zero;
	
	suply_voltage = (suply_voltage*5*2)/(16*1024);
	printf("----------  voltage=%.4f  ----------  \n\n",suply_voltage);
	_delay_ms(40);
	sei();
	MOTO_ON();
	for (int i=0;i<128;i++)
	{
		MoCmdBuff_str.PosiCmd = 0;
		moto_control();
	}

	MoCmdBuff_str.ViloCmd = 195;
	sys_tick = 0;
	float posi_cmd = 0;
	
	int16_t speed=0;
	int16_t acc = MoCmdBuff_str.ViloCmd;
	int16_t pre_deg = Compensator_str.Count;
	while (1)
	{
		if (pipline_triger ==1)
		{
			/* speed control */
			//MoCmdBuff_str.PosiMore = -acc;
			//acc-=1;
			//if (abs(acc) < 3)
			//	acc=0;
			speed = (int16_t)Compensator_str.Count - pre_deg;
			if (speed>8192)
			{
				speed-=16384;
			}
			else if(speed<-8192){
				speed+=16384;
			}
			pre_deg = Compensator_str.Count;
			if (sys_clk > 3000)
				break;
			else
			moto_control();
			/* position control */
			//if ((sys_clk & 63)==0)
			//{
			//	posi_cmd += 16384.0/42.0;
			//	MoCmdBuff_str.PosiCmd = posi_cmd;
			//}
			//
			//if(moto_control() == 1)
			//	break;
			//
			//if (posi_cmd >= 16384)
			//{
			//	break;
			//}
			task_finish = 1;
		}
		else if(task_finish == 1){
			if ((sys_clk & 63) > 30)
			{
				
			}
			//printf("%5d,%5d,%3d,",MoCmdBuff_str.PosiGoad,*MoCmdBuff_str.PosiSensIn_p,MoCmdBuff_str.error_out);
			//printf("%5d,%5d\n",IFeedBack_str.CurrentOut,LLFeedBack_str.CountOut-Compensator_str.Count);
			printf("%3d,%3d,",MoCmdBuff_str.ViloCmd,speed);
			printf("%3d,%3d,%3d,",current[0],current[1],current[2]);
			printf("%3d,%3d,%3d\n",IntPoPwm_PwmOut_p[0],IntPoPwm_PwmOut_p[1],IntPoPwm_PwmOut_p[2]);
			task_finish = 0;
		}
		if (0)
		{
			break;
		}
	}
	cli();
	MOTO_OFF();
	switch (error_code){
	case 0:
		printf("\nfinish\n");
		break;
	case 1:
		printf("\nover current\n");
		break;
	}
}


	