#include<avr/io.h>
#include<util/delay.h>
#include<avr/interrupt.h>
#include"uartnaval.h"
#include"uartnaval.c"

#define ADC_RAW_DIV  		512.0			/* 512 for 10 bit adc, 128 for 8 bit adc*/
#define ADC_REAL_MUL   		(5/(ADC_RAW_DIV*2))	/* to be directly multiplied by ADC value of current to get realworld current value*/
//#define MV_PER_AMP		0.2			// for ACS712 5A module
#define MV_PER_AMP			0.1		// for ACS712 20A module
//#define MV_PER_AMP		0.066			// for ACS712 30A module
#define VOLTAGE_MAX_READ	24
#define FULLSCALE_VADC_READ	4.8

/* calculated voltage 0-5 range from ADC value is for voltage, it is 
then multiplied by below ratio to get real world voltage value*/
#define ADC_REAL_V_MUL		(VOLTAGE_MAX_READ/FULLSCALE_VADC_READ) 

unsigned char str[20],st[1],str2[20];
unsigned int i=0,adval,current_adc_val=0,voltage_adc_val=0;
float current_adc_val_div=0,vout,cout;

/* initializing ADC on Atmega32 in free running mode with interrupt enabled */
/*  refer Atmega32 data sheet for mode about below registers */
void init_ADCINT()
{	
	ADCSRA =(1<<ADEN)|(1<<ADIE)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);
	ADMUX =(1<<REFS0)|(1<<ADLAR);
	ADCSRA |= (1<<ADSC);
}

int main()
{	
	uart_init();
	_delay_ms(20);
	uart_txstr("Enable Global Interrupt \r");
	sei();
	
	init_ADCINT();
	_delay_ms(20);
	while(1) 
	{	
		/* for 0 A current, voltage output from ACS712 is +2.5V, (ACS712 voltage - 2.5V) to get actual rise in voltage*/	
		current_adc_val_div = current_adc_val - ADC_RAW_DIV;
		
		/* dividing actual rise in voltage by per AMP voltage rise */
		cout = ADC_REAL_MUL * current_adc_val_div / MV_PER_AMP;
		
		/* converting float value to string */
		dtostrf(cout,2,2,str2);
		
		/* creating string with current value to print on terminal as string */
		sprintf(str,"Current is %sA",str2);
		uart_txstr(str);
		
		vout=voltage_adc_val*ADC_REAL_MUL*ADC_REAL_V_MUL;
		
		dtostrf(vout,2,2,str2);
		sprintf(str," Bat. Voltage is %sV",str2);
		uart_txstr(str);
		
		uart_tx('\r');
		_delay_ms(500);
	}
	
	

}

ISR(ADC_vect){
	
	
	st[0]=ADCL;
	st[1]=ADCH;
	adval=(st[1]<<2)|(st[0]>>6);
	
	if(i==0)
	{
		current_adc_val=adval;
	} 
	
	else if(i==1)
	{
		voltage_adc_val=adval;
	}
	
	i++;
	if(i>=2)
	{i=0;}
	
	ADMUX=0b01100000;	// ADLAR=1 i.e. left adjusted result // 	8bit
	ADMUX=ADMUX+i;		// selecting adc pin to read
	ADCSRA |= (1<<ADSC);
	
	
	
}
