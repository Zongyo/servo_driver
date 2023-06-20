/*
 * main.h
 *
 * Created: 2023/6/8 下午 05:14:58
 *  Author: ZongYo
 */ 
/* 基本硬體操作 */

#ifndef MAIN_H_
#define MAIN_H_
#include "hardware_init.h"
#include "Overflower.h"
#include "RWFirFilter.h"
#include "IntPoPwm.h"
#include "SinTable.h"
#include "Compenastor_2.h"
#include "IFeedBack.h"
#include "LLFeedBack.h"
#include "MoCmdBuff.h"
// on B4 high ,input 
void MOTO_ON() {
	PORTB = PORTB | (1<<4);	
	DDRB = DDRB & (~(1<<4));
}

// off B4 low ,output 
void MOTO_OFF() {
	PORTB = PORTB & (~(1<<4));
	DDRB = DDRB | ((1<<4));
	OCR1A=0;
	OCR1B=0;
	OCR1C=0;
}

static inline uint8_t SPI_data_swap(char cData)
{
	/* Start transmission */
	SPDR = cData;
	/* Wait for transmission complete */
	while(!(SPSR & (1<<SPIF)))
	;
	return SPDR;
}

static inline uint16_t read_deg(){
	uint16_t deg;
	PORTD = PORTD & (~(1<<PIND5));
	for (uint16_t i=0;i<2;i++)
	;
	deg=SPI_data_swap(0x3f);		//deg high byte
	deg=deg<<8;
	deg=deg|SPI_data_swap(0xff);	//deg low byte
	deg=deg&0x3FFF;
	for (uint16_t i=0;i<2;i++)
	;
	PORTD = PORTD | (1<<PIND5);
	
	for (uint16_t i=0;i<4;i++)
	;
	return deg;
}

static inline uint16_t get_ADC(uint8_t pin){
	PORTD =PORTD | 1 << PIND6;
	uint8_t ct=0,j=1;
	static uint8_t last_pin = 0;
	if (last_pin != pin){
		j=3;
	}
	for (int i=0;i<j;i++)
	{
		REGFPT(&ADMUX,0x1F,0,pin);		//select input 當改變pin腳地一次轉換會不準
		REGFPT(&ADCSRA,0x40,6,1);		//ADC start convert		1.24
		while(!((ADCSRA&0x10)>>4)){ct++;}
	}
	last_pin=pin;
	PORTD = PORTD ^ (PORTD&(1<<PIND6));
	return ADC;
}

static inline void three_phase_out(volatile uint16_t *pwm_val){
	uint16_t max = ICR1;
	OCR1A=pwm_val[0]%max;
	OCR1B=pwm_val[1]%max;
	OCR1C=pwm_val[2]%max;
}

#endif /* MAIN_H_ */