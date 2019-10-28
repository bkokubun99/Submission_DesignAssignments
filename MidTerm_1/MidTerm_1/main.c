/*
 * DesignAssignment_MidTerm_1
 *
 * Created: 10/18/2019 7:12:16 PM
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

/*******************************************************************************
*							Global Variables     							   *
*******************************************************************************/
//Unsigned Integer for Interrupt Overflow 
volatile uint8_t Overflow;
//Integer for Temperature from LM-35
volatile int adc_temp;
//Character array To hold Temp value
char char_array[256];
//Variables for AT commands
char command1[] = "AT\r\n";
char command2[] = "AT+CWMODE=1\r\n";
char command3[] = "AT+CWJAP=\"Kokubun\",\"8084465741\"\r\n";
char command4[] = "AT+CIPMUX=0\r\n";
char command5[] = "AT+CIPSTART=\"TCP\",\"api.thingspeak.com\",80\r\n";
char command6[] = "AT+CIPSEND=51\r\n";
char command7[] = "AT+CIPCLOSE\r\n";

/*******************************************************************************
*							Function Prototypes								   *
*******************************************************************************/
void USART_init(void);				//FUNCTION TO INITIALIZE USART
void adc_init(void);				//FUNCTION TO INITIALIZE ADC
void set();							//FUNCTION SETS TIMER/INTERRUPT REGISTERS
void USART_tx_string(char*data);	//FUNCTION READS IN 1 CHAR AT A TIME
void read_adc(void);				//FUNCTION READS VALUES FROM LM-35

/*******************************************************************************
*								Int Main									   *
*******************************************************************************/
int main(void)
{
    USART_init();							//INITIALIZE USART
	adc_init();								//INITIALIZE ADC
	set();									//INITIALIZE TIMERS/INTERRUPTS
	
	_delay_ms(1000);
	USART_tx_string(command1);				//AT-COMMAND
	_delay_ms(3000);
	USART_tx_string(command2);				//SET CWMODE = 1
	_delay_ms(3000);
	USART_tx_string(command3);				//CONNECT Wifi
	_delay_ms(3000);
	USART_tx_string(command4);				//SELECT CIP MUX
	_delay_ms(3000);
	
	while (1)
	{
		if(Overflow >= 4)					//OVERFLOW AMOUNT FOR ~15sec DELAY	
		{
			USART_tx_string(command5);		//CONNECT TO ThinkSpeak
			_delay_ms(3000);
			USART_tx_string(command6);		//SPECIFY SIZE OF DATA
			_delay_ms(3000);
				
			read_adc();						//CALL FUNCTION TO READ TEMPERATURE
				
			snprintf(char_array,sizeof(char_array),"GET /update?key=BTGT7XH3A9N1Y8OZ&field1=%3d\r\n", adc_temp);
			USART_tx_string(char_array);	//PRINT LM-35 TEMPERATURE VALUE
			USART_tx_string("\r\n");
			_delay_ms(3000);
				
			USART_tx_string(command7);		//CLOSE AT-COMMAND
			_delay_ms(3000);
				
			//Overflow reset to 0
			Overflow = 0;
		}
	}
}

/*******************************************************************************
*							Functions										   *
*******************************************************************************/
//FUNCTION TO INITIALIZE USART
void USART_init(void)
{
	UBRR0H = UBRRH_VALUE;
	UBRR0L = UBRRL_VALUE;
	UCSR0C = _BV(UCSZ01) | _BV(UCSZ00); //8-BIT DATA
	UCSR0B = _BV(RXEN0) | _BV(TXEN0);	//ENABLE RX AND TX
}

//FUNCTION TO INITIALIZE ADC
void adc_init(void)
{
	ADMUX =  (0<<REFS1) |				//REFERENCE SELECTION BITS
	(1<<REFS0) |						//AVcc -External cap at AREF 5V
	(0<<ADLAR) |						//ADC RIGHT ADJUST RESULT
	(1<<MUX2)  |						//ANALOG CHANNEL SELECTION BITS
	(0<<MUX1)  |						//ADC4 (PC4) CHANNEL 4
	(0<<MUX0);
	
	ADCSRA = (1<<ADEN)  |				//ADC ENABLE
	(0<<ADSC)  |						//ADC START CONVERSION
	(0<<ADATE) |						//ADC auto trigger enable
	(0<<ADIF)  |						//ADC interrupt flag
	(0<<ADIE)  |						//ADC interrupt enable
	(1<<ADPS2) |						//ADC PRESCALAR SELECTION BITS
	(1<<ADPS1) |						//128 AS PRESCALAR SEL. BITS
	(1<<ADPS0);
}

//FUNCTION THAT SETS TIMER/INTERRUPT REGISTERS
void set()
{
	TCCR1A = (0<<WGM10)| (0<<WGM11);	//NORMAL MODE OPERATION
	TCCR1B = (0<<WGM12)| (0<<WGM13);	//NORMAL MODE OPERATION
	TCCR1B =  0X05;						//THE PRESCALER SET TO 1024		
	TCNT1H =  0X00;						//COUNTER HIGH VALUE = 0
	TCNT1L =  0X00;						//COUNTER LOW  VALUE = 0
	TIMSK1 = (1<<TOIE1);				//ENABLE INTERRUPT
	sei();								//ENABLE GLOBAL INTERRUPT
}

//FUNCTION THAT READS IN 1 CHAR AT A TIME
void USART_tx_string(char*data)
{
	//CONTROL ENTERS WHILE DATA REG NOT EMPTY
	while((*data!='\0'))
	{
		//WAIT FOR BUFFER REGISTER TO CLEAR
		while(!(UCSR0A & (1 << UDRE0)));
		UDR0 = *data;					//REGESTER EQUALS DATA
		data++;							//DATA MOVES POSITION
	}
}

//FUNCTION THAT READS TEMPERATURES
void read_adc(void)
{
	unsigned char i = 4;				//VARIABLE FOR SAMPLE AMOUNTS
	adc_temp = 0;						//INITIALIZE adc_temp
	while(i--)
	{
		ADCSRA |= (1<<ADSC);			//ENABLE START CONVERSION
		while(ADCSRA & (1<<ADSC));		//WAIT UNTIL ABLE AND START CONVERSION
		adc_temp += ADC;				//LM35 VALUE ADDED & STORED INTO adc_temp
		_delay_ms(50);					
	}
	adc_temp = adc_temp/4;				//AVERAGE OF LM35 TEMPERATURE VALUE
}

//INTERUPT SUBROUTINE FOR TIMER 1
ISR(TIMER1_OVF_vect)
{
	Overflow++;							//INCREMENT OVERFLOW FOR INTERRUPT 
}

