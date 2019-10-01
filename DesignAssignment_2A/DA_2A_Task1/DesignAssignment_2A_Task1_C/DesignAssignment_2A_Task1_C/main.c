/*
 * DesignAssignment_2A_Task1_Cprogram.c
 * C 
 * Created: 9/30/2019 5:33:34 PM
 * Author : BRYSEN KOKUBUN 
 */ 

#include <avr/io.h>
#define F_CPU 16000000UL
#include<util/delay.h>


int main(void)
{
    DDRB |= (1<<3);				//MAKING PORTB.3 OUTPUT
	PORTB &= ~(1<<3);			//MAKING PORTB.3 OUTPUT "OFF"

    while (1) 
    {
		PORTB |=(1<<3);			//OUTPUT "LED ON"
		_delay_ms(250);			//DELAY FOR 250ms @16MHz
		PORTB &=~(1<<3);		//OUTPUT "LED OFF"
		_delay_ms(375);			//DELAY FOR 375ms @16MHz
    }
	return 1;
}



