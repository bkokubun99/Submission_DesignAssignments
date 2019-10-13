/*
 * DA_2C_TASK3.c
 *
 * Created: 10/12/2019 9:40:22 PM
 * Author : BRYSEN KOKUBUN
 */ 

#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>


volatile uint8_t Overflow;

void set()
{
	TCCR0A = (1<<WGM01);		//CTC MODE OPERATION
	TCCR0B  = 0X05;				//THE PRESCALER SET TO 1024
	TCNT0 = 0X00;				//COUNTER VALUE = 0
	OCR0A = 0X80;				//COMPARE REGISTER VALUE OF 128
	TIMSK0 = (1<<OCIE0A);		//ENABLE TIMER COMPARE INTERUPT
	sei();						//ENABLE GLOBAL INTERUPT
}

ISR(TIMER0_OVF_vect)
{
	Overflow++;					//INCREMENT OVERFLOW
}

void Button()
{
	DDRB = (0<<3);				//SET PORTB.3 "OFF"
	Overflow = 0;				//RESET OVERFLOW
	set();						//CALL SET FCT
	DDRB |= (1<<2);				//SET PORTB.2 "ON"
	while (Overflow < 162);
	DDRB = (0<<2);				//SET PORTB.2 "OFF"
	Overflow = 0;
}

int main(void)
{
	DDRC &=~ (1<<3);			//SET PORTC.3 INPUT
	PORTC |= (1<<3);			//PULL-UP RESISTOR ACTIVATE
	set();						//CALL set FCT
	DDRB = (1<<3);				//SET PORTB.3 "ON"
	while (1)
	{
		//CONTROL ENTERS IF BUTTON PRESSED @ PORTB.3
		if (!(PINC & (1<<3)))
		{
			Button();			//CALL Button FCT.
		}
		//CONTROL ENTERS IF OVERFLOW VALUE BETWEEN 15 & 38
		if ((Overflow >= 30) & (Overflow < 76))
		{
			DDRB = (0<<3);		//SET PORTB.3 "OFF"
		}
		//CONTROL ENTERS IF OVERFLOW VALUE IS GREATER THAN 38
		if ((Overflow > 76))
		{
			DDRB = (1<<3);		//SET PORTB.3 "ON"
			Overflow = 0;		//RESET OVERFLOW TO 0
		}
	}
	return 1;
}
