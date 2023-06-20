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

ISR (TIMER1_OVF_vect){
	sys_tick++;
	if (sys_tick == 16){
		sys_clk++;
		sys_tick = 0;
		pipline_triger = 1;
	}

}

uint16_t encoderCount = 0;
uint32_t cmd_angle = 0;
uint16_t voltage_cmd = 128;

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
	
	
	//for open loop test
	/*
	int16_t diff_zero =0;
	LLFeedBack_str.CountIn_p = (uint16_t*)&cmd_angle;;
	LLFeedBack_str.DiffCountIn_p = &diff_zero;
	IFeedBack_str.DiffCountIn_p  = &diff_zero;
	*/

	encoderCount = read_deg();
	OVERFLOWER_LAY();
	Overflower_step(&Overflower_str);
	RWFirFilter_LAY();
	MoCmdBuff_lay(&MoCmdBuff_str,encoderCount , MoCmd_NextTask_p);
	SinTable_LAY();
	LLFeedBack_LAY();
	IFeedBack_LAY();
	Compensator_LAY();
	IntPoPwm_LAY();
	sei();
	
	Compensator_step(&Compensator_str);
	cmd_angle = (Compensator_str.Count + 8192)%16384;

	MoCmdBuff_str.PosiCmd = 0;
	MoCmdBuff_str.PosiCmd =cmd_angle;
	MOTO_ON();
	while (1)
	{
		if (pipline_triger ==1)
		{
			encoderCount = read_deg();
			for (uint8_t i=0;i<4;i++)
			{
				encoderCount = read_deg();
				Overflower_step(&Overflower_str);
				RWFirFilter_step(&RWFirFilter_str);
			}
			get_ADC(0);
			
			Compensator_step(&Compensator_str);
			MoCmdBuffExec_step(&MoCmdBuff_str);
			IFeedBack_step(&IFeedBack_str);
			LLFeedBack_step(&LLFeedBack_str);
			IntPoPwm_step(&IntPoPwm_str);
			three_phase_out(IntPoPwm_PwmOut_p);
			pipline_triger = 0;
			task_finish = 1;

		}
		else if(task_finish == 1){
			printf("%5d,%5d,%5d\n",MoCmdBuff_str.PosiGoad , Compensator_str.Count  ,MoCmdBuff_str.PosiCmd);
			
			
			task_finish = 0;
		}
		
		if (0)
		{
			break;
		}
	}
	cli();
	MOTO_OFF();
}


	