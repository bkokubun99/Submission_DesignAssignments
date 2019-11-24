/*
 * DesignAssignment_4A.c
 *
 * Created: 11/05/2019 9:34:45 PM
 * Author : BRYSEN KOKUBUN
 */ 
#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <avr/interrupt.h>

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
	DDRC  |=  (0<<3);	   //Set Portc.3 Input
	PORTC |=  (1<<3);	   //Pull Up Resistor Activated
	PORTC |=  (1<<0);	   //Pull Up Resistor Activated
	DDRB  |=  (1<<DDB1);   //Sets Port B1 to be output
	
	PCICR = (1<<PCIE1);    //Pin change interrupt Control Register
	PCMSK1 = (1<<PCINT11); //Enable Mask register for Portc.3
	sei ();				   //GLOBAL INTERRUPT ENABLE
	
	ICR1 = 0XFFFF;
	TCCR1A |= (1<<COM1A1)|(1<<COM1B1);
	TCCR1A |= (1<<WGM11);
	TCCR1B |= (1<<WGM12)|(1<<WGM13);
	TCCR1B |= (1<<CS10);
	
	while (1 )
	{
		read_adc();		   //Call Read function
		_delay_ms(50);
		//PWM at 95% max
		if ((adc_value >= 62258) && (adc_value < 65535 ))
		{
			OCR1A = 62258;
			_delay_ms(20);
		}
		//Increasing PWM As Resistor Value Increases
		else if ((adc_value < 62257) && (adc_value >= 3277))
		{
			OCR1A = adc_value + 30000;
			_delay_ms(20);
		}
		//PWM is 0% If Resistor Value < 5%
		else
		OCR1A = 0x00;
	}
	return 0;
}//End Main

/******************************************************************************
*					INTERRUPT SUBROUTINE									  *
******************************************************************************/
ISR(PCINT1_vect)
{
	DDRC ^= (1<<0);
}

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

