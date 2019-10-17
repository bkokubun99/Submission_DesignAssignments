/*
 * DesignAssignment_3A
 *
 * Created: 10/14/2019 5:15:35 PM
 * Author : BRYSEN KOKUBUN 
 */ 

#define BAUD 9600
#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <util/setbaud.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <stdio.h>

//"Global variable"
volatile uint8_t Overflow;

//FUNCTION TO INITIALIZE USART
void USART_init(void)
{
	UBRR0H = UBRRH_VALUE;
	UBRR0L = UBRRL_VALUE;
	UCSR0C = _BV(UCSZ01) | _BV(UCSZ00); //8-BIT DATA
	UCSR0B = _BV(RXEN0) | _BV(TXEN0);	//ENABLE RX AND TX
}

//FUNCTION THAT SETS TIMER/INTERRUPT REGISTERS 
void set()
{
	TCCR0A = 0;					//NORMAL MODE OPERATION
	TCCR0B  = 0X05;				//THE PRESCALER SET TO 1024
	TCNT0 = 0X00;				//COUNTER VALUE = 0
	TIMSK0 = (1<<TOIE0);		//ENABLE INTERRUPT			
	sei();						//ENABLE GLOBAL INTERRUPT
}

//INTERUPT SUBROUTINE FOR TIMER 0 
ISR(TIMER0_OVF_vect)
{
	Overflow++;					//INCREMENT OVERFLOW 
}

//FUNCTION THAT READS IN 1 CHAR AT A TIME 
void USART_tx_string(char*data)
{
	//CONTROL ENTERS WHILE DATA REG NOT EMPTY
	while((*data!='\0'))
	{
		//WAIT FOR BUFFER REGISTER TO CLEAR
		while(!(UCSR0A & (1 << UDRE0)));
		UDR0 = *data;			//REGESTER EQUALS DATA
		data++;					//DATA MOVES POSITION
	}
}

int main(void)
{
    USART_init();			//Initialize USART
	set();					//CALL SET FUNCTION
	
	int num = 2303;			//Initialize integer value
	char char_array [16];	//Character array of 16 for integer
	char char_array2[16];	//Character array of 16 for float
	
    while (1) 
    {
		//Initialization of float value 
		volatile float float_value = num * 3.2;
		
	//CONTROL WILL ENTER IF OVERFLOW >= 61 CYCLES ~1sec
	if (Overflow >= 61)		
	{
		//To generate random number 
		num = (((num * 3) % 100) + 2);
		itoa (num, char_array, 10);
		
		snprintf(char_array2,sizeof(char_array2), "%f\r\n", float_value);

		//Prints string Brysen Kokubun!
		USART_tx_string("Brysen Kokubun!"); 
		USART_tx_string(" ");
		
		//Prints integer value
		USART_tx_string(char_array);
		USART_tx_string(" ");
		
		//Prints floating value
		USART_tx_string(char_array2);
		USART_tx_string("\n");
		USART_tx_string("\n");
		
		//Overflow reset to 0
		Overflow = 0;
	}
    }
}


