#include<avr/io.h>
#include<util/delay.h>
//#include<compat/deprecated.h>
#include<avr/interrupt.h>
#include"uartnaval.h"
#include"uartnaval.c"
//#include"lcdgdheerajat8.h"
//#include"lcdgdheerajat8.c"
//#include"adcatmega16.h"
//#include"adcatmega16.c"


#define Nbitdiv  512.0  // 512 for 10 bit adc, 128 for 8 bit adc
#define factor   (5/(Nbitdiv*2))
//#define mVperA	0.2		// for ACS712 5A module
#define mVperA	0.1		// for ACS712 20A module
//#define mVperA	0.066		// for ACS712 30A module
#define factor2		(5/Nbitdiv*2)
#define voltagemeasure		24
#define fullscale			4.8
#define batvscale		(voltagemeasure/fullscale)
#define dcwatthr	(v*btv)


unsigned char str[20],st[1],str2[20];
unsigned int i=0,adval,j=0,k;
float z=0,v,vout,cout;

void init_ADCINT()
{	
	ADCSRA =(1<<ADEN)|(1<<ADIE)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);
	ADMUX =(1<<REFS0)|(1<<ADLAR);
	ADCSRA |= (1<<ADSC);
}

int main()
{	
	DDRB=0xff;
	DDRD=0xff;
	
	uart_init();
	_delay_ms(20);
	uart_txstr("Enable Global Interrupt \r");
	sei();
	
	init_ADCINT();
	_delay_ms(20);
	while(1) 
	{	
	
		/*sprintf(str,"ADCH is %d",j);
		uart_txstr(str);
		_delay_ms(500);
		uart_tx('\r');
		sprintf(str2,"Vraw is %d",k);
		uart_txstr(str2);*/
		
		z=j - Nbitdiv;		// 
		cout=factor * z / mVperA;
		dtostrf(cout,2,2,str2);
		
		sprintf(str,"Current is %sA",str2);
		uart_txstr(str);
		
		vout=k*factor*batvscale;
		
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
		j=adval;
	} 
	
	else if(i==1)
	{
		k=adval;
	}
	
	i++;
	if(i>=2)
	{i=0;}
	
	ADMUX=0b01100000;// ADLAR=1 i.e. left adjusted result // 	8bit
	ADMUX=ADMUX+i;
	ADCSRA |= (1<<ADSC);
	
	
	
}