/*
 * DesignAssignment_4B.c
 * Part1 Step Motor
 * Created: 11/12/2019 7:54:45 PM
 * Author : BRYSEN KOKUBUN
 */ 
#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>

//"Global Variable"
volatile float adc_value;

/******************************************************************************
*						FUNCTION PROTOTYPES									  *
******************************************************************************/
void adc_init(void);
void read_adc(void);

/******************************************************************************
*							Main											  *
******************************************************************************/
int main(void)
{
	adc_init();			   //Initial ADC
	PORTC |=  (1<<0);	   //Pull Up Resistor Activated PC0
	DDRB = (1<<1);		   //Output PB1
	DDRD = 0x0F;           //Outputs PD3, PD2, PD1, PD0
	
	
	//Configuration of Timer1
	ICR1 = 4999;  //freq = 50Hz, Period = 20ms
	TCCR1A |= (1<<COM1A1)|(1<<COM1B1); 
	TCCR1A |= (1<<WGM11);			 //CTC Mode 
	TCCR1B |= (1<<WGM12)|(1<<WGM13); //CTC Mode 
	TCCR1B |= (1<<CS10) |(1<<CS11);  //Prescaler 64
	
	while (1)
	{
		read_adc();		   //Call Read function
		_delay_ms(50);
		
		//Stepper Motor Configurations
		PORTD = 0x66;
		_delay_ms(50);
		
		PORTD = 0xCC;
		_delay_ms(50);
		
		PORTD = 0x99;
		_delay_ms(50);
		
		PORTD = 0x33;
		_delay_ms(50);
		
		//PWM at 95% to max
		if ((adc_value >= 973) && (adc_value < 1024 ))
		{
			_delay_ms(10);  //Create small delay
		}
		//Decreasing PWM As Resistor Value Decreases 
		else if ((adc_value < 972) && (adc_value >= 768))
		{
			_delay_ms(50);  //Create delay 
		}
		//Decreasing PWM As Resistor Value Decreases 
		else if ((adc_value < 767) && (adc_value >= 51))
		{
			_delay_ms(100); //Create delay 
		}
		//PWM is 0% If Resistor Value < 5%
		else
		{
			PORTD = 0x00;
			_delay_ms(100); //Create delay 
		}
	}
	return 0;
}//End Main

/******************************************************************************
*					FUNCTION DEFINITIONS									  *
******************************************************************************/
//FUNCTION TO INITIALIZE ADC
void adc_init(void)
{
	ADMUX =  (0<<REFS1) | //REFERENCE SELECTION BITS
	(1<<REFS0) | //AVcc -External cap at AREF 5V
	(1<<ADLAR) | //ADC LEFT ADJUST RESULT
	(0<<MUX2)  | //ANALOG CHANNEL SELECTION BITS
	(0<<MUX1)  | //ADC0 (PC0) CHANNEL 0
	(0<<MUX0);
	
	ADCSRA = (1<<ADEN)  | //ADC ENABLE
	(0<<ADSC)  | //ADC START CONVERSION
	(0<<ADATE) | //ADC auto trigger enable
	(0<<ADIF)  | //ADC interrupt flag
	(0<<ADIE)  | //ADC interrupt enable
	(1<<ADPS2) | //ADC PRESCALAR SELECTION BITS
	(0<<ADPS1) | //128 AS PRESCALAR SEL. BITS
	(1<<ADPS0);
}

//FUNCTION THAT READS ADC PINS
void read_adc(void)
{
	unsigned char i = 10;		   	 //Variable for sample amounts
	adc_value = 0;			       	 //Initialize adc_value
	while(i--)
	{
		ADCSRA |= (1<<ADSC);	     //Enable Start Conversion
		while(ADCSRA & (1<<ADSC));   //Wait until enable & start conversion
		adc_value += ADC;            //PC0 value added & stored into adc_value
	}
	adc_value = adc_value/10;	     //Average of values
}



