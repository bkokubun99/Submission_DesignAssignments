/*
 * DesignAssign_5
 *
 * Created: 12/01/2019 12:24:33 PM
 * Author : Brysen Kokubun
 */

//Clock frequency
#ifndef F_CPU
#define F_CPU 16000000UL
#endif
/******************************************************************************
*							Libraries										  *
******************************************************************************/
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

//UART for printf();
#define UBRR_9600 103

#include "STDIO_UART.h"

//nRF24L01+ library
#include "nrf24l01.h"
#include "nrf24l01-mnemonics.h"
#include "spi.h"

/******************************************************************************
*							Function Prototypes								  *
******************************************************************************/
void adc_init(void);
void USART_init( );
void read_adc(void);
void USART_tx_string(char *data);
void print_config(void);

/******************************************************************************
*							Volatile Declarations							  *
******************************************************************************/
volatile bool message_received = false;				//IRQ ISR
volatile bool status = false;						//IRQ ISR
volatile float adc_value;							//ADC VALUE
char ar[20];

//Pipe Address's 
//uint8_t rx_address[5] = { 0xe7, 0xe7, 0xe7, 0xe7, 0xe7 };//Read pipe address
//uint8_t tx_address[5] = { 0xf7, 0xf7, 0xf7, 0xf7, 0xf7 };//Write pipe address
#define READ_PIPE

/******************************************************************************
*							INT MAIN 										  *
******************************************************************************/
int main(void)
{
	adc_init();							//INITIALIZE ADC 
	USART_init(UBRR_9600);				//INITIALIZE USART
	USART_tx_string("Hello there!\n");  //PRINT TO TERMINAL 
	_delay_ms(500);

	//Message To Send 
	char tx_message[32];				// Define string array
	strcpy(tx_message,"Hello World!");	// Copy string into array
	
	USART_init();						//INITIALIZE USART 
	nrf24_init();						//INITIALIZE nRF24L01
	print_config();						//INITIALIZE PRINT 
	
	//WAIT FOR RECIEVE DATA
	nrf24_start_listening();
	
	while (1)
	{
		//CHECK IF MESSAGE RECEIVED 
		if (message_received)
		{
			message_received = false;
			//PRINT MESSAGE 
			printf("Received Temperature: %s\n",nrf24_read_message());
			_delay_ms(500);
			//Send RESPONSE MESSAGE 
			status = nrf24_send_message(tx_message);
			//CHECK STATUS/PRINT TEMPERATURE 
			if (status == true) printf("Transmitted Temperature");
		}
		
		read_adc();								//READ ADC VALUE
		snprintf(ar, sizeof(ar),"%f\r\n",adc_value);	//PRINT
		USART_tx_string(ar);
		_delay_ms(1000);						//1 SEC DELAY

	}
}
/******************************************************************************
*							FUNCTION DEFINITIONS 							  *
******************************************************************************/

//Interrupt SUBROUTINE FOR IRQ PIN
ISR(INT0_vect)
{
	message_received = true;
}

//FUNCTION TO INITIALIZE ADC
void adc_init(void)
{
	ADMUX =  (0<<REFS1) | //REFERENCE SELECTION BITS
	(1<<REFS0) | //AVcc -External cap at AREF 5V
	(0<<ADLAR) | //ADC LEFT ADJUST RESULT
	(1<<MUX2)  | //ANALOG CHANNEL SELECTION BITS
	(0<<MUX1)  | //ADC0 (PC4) CHANNEL 4
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

//FUNCTION TO INITIALIZE USART (RS-232) 
void USART_init(unsigned int ubrr){
	UBRR0H = (unsigned char)(ubrr >> 8);
	UBRR0L = (unsigned char)ubrr;
	UCSR0B = (1 << TXEN0); 			//ENABLE TRANSMISSION 
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); //5-bit characters
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
		adc_value += ADC;            //PC4 value added & stored into adc_value
	}
	adc_value = adc_value/10;	     //Average of values
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

//FUNCTION TO PRINT DATA/CONFIGURATIONS TO TERMINAL 
void print_config(void)
{
	uint8_t data;
	printf("Startup successful\n\n nRF24L01+ configured as:\n");
	printf("-------------------------------------------\n");
	nrf24_read(CONFIG,&data,1);
	printf("CONFIG		0x%x\n",data);
	nrf24_read(EN_AA,&data,1);
	printf("EN_AA			0x%x\n",data);
	nrf24_read(EN_RXADDR,&data,1);
	printf("EN_RXADDR		0x%x\n",data);
	nrf24_read(SETUP_RETR,&data,1);
	printf("SETUP_RETR		0x%x\n",data);
	nrf24_read(RF_CH,&data,1);
	printf("RF_CH			0x%x\n",data);
	nrf24_read(RF_SETUP,&data,1);
	printf("RF_SETUP		0x%x\n",data);
	nrf24_read(STATUS,&data,1);
	printf("STATUS		0x%x\n",data);
	nrf24_read(FEATURE,&data,1);
	printf("FEATURE		0x%x\n",data);
	printf("-------------------------------------------\n\n");
}




