/*
 * hardware_init.c
 *
 * Created: 2023/6/7 上午 02:27:55
 *  Author: ZongYo
 */ 
#include "../Inc/hardware_init.h"

void spi_initial(void){
	REGFPT(&DDRD,0x20,5,0x01);		//set cs out put
	REGFPT(&PORTB,0x20,5,1);		//CS high
	REGFPT(&DDRB,0x0f,0,0x07);		//MOSI、CS、SCK(out) MISO(in)
	REGFPT(&PORTB,0x30,4,3);		//CS1,CS2(high)
	REGFPT(&DDRB,0x30,4,3);			//CS1,CS2(out)
	REGFPT(&SPSR, 0x01, 0,1);		//雙倍頻率 1:en 0:un
	REGFPT(&SPCR, 0x03, 0,1);		//除頻 0:4 1:16 2:64 3:128
	REGFPT(&SPCR, 0x04, 2,1);		//CLK相位
	REGFPT(&SPCR, 0x08, 3,0);		//CLK極性
	REGFPT(&SPCR, 0x10, 4,1);		//0:slave 1:master
	REGFPT(&SPCR, 0x20, 5,0);		//0:MSB first 1:LSB first
	REGFPT(&SPCR, 0x40, 6,1);		//0:SPI en 1:SPI un
	REGFPT(&SPCR, 0x80, 7,0);		//0:SPI INT en 1:SPI INT un
}

void ADC_initial(void){
	DDRF=0x00;
	REGFPT(&ADMUX,0x1F,0,0);		//pin0 input
	REGFPT(&ADMUX,(P5),5,0);		//low byte 8 bit ,high byte 2 bit
	REGFPT(&ADMUX,(P6|P7),6,0x00);	//internal 0.AREF 1.AVCC 3.Vref:2.56v used
	REGFPT(&ADCSRA,0x07,0,5);		//ADC freq div/32
	REGFPT(&ADCSRA,P3,3,0);			//ADC interrupt enable
	REGFPT(&ADCSRA,P5,5,0);			//free running mode
	REGFPT(&ADCSRA,P7,7,1);			//ADC enable
	REGFPT(&ADCSRA,P6,6,1);			//ADC start convert
}
void pwm_1_initial(void){
	REGFPT(&DDRB,0xE0,5,7);			//B5,B6,B7 output
	REGFPT(&TCCR1A,0xc0,6,3);		//A channel output mode
	REGFPT(&TCCR1A,0x30,4,3);		//B channel output mode
	REGFPT(&TCCR1A,0x0c,2,3);		//C channel output mode
	REGFPT(&TCCR1A,0x03,0,0);		//WWM1 0:1
	REGFPT(&TCCR1B,0x18,3,2);		//WWM1 2:3
	REGFPT(&TCCR1B,0x07,0,1);		//CLK div 1
	REGFPT(&TIMSK,0x04,2,1);		//OVF interrupt enable
	
	ICR1=1024;
	OCR1A=32;
	OCR1B=64;
	OCR1C=128;
}

void IO_initial(void){
		PORTD=0;
		DDRD =DDRD | (1 << PIND7) |(1 << PIND6) | (1 << PIND5);
}

void hardware_init(void){
	uart_init();
	spi_initial();
	ADC_initial();
	pwm_1_initial();
	IO_initial();
}

