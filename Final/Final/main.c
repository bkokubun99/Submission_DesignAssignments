/*
 * Final.c
 *
 * Created: 12/9/2019 4:56:10 PM
 * Author : Brysen Kokubun 
 */

/******************************************************************************
*							  Definitions					  				  *
******************************************************************************/
//Frequency Definitions 
#ifndef F_CPU
#define F_CPU 16000000UL
#endif

//UART Definitions 
#define BAUD 9600
#define FOSC 16000000
#define UBRR FOSC/8/BAUD-1

//Sensor Write & Read Definitions 
#define APDS9960_WRITE 0x72
#define APDS9960_READ  0x73

/******************************************************************************
*							  Libraries					     				  *
******************************************************************************/
#include <avr/io.h>
#include <stdlib.h>
#include <stdio.h>
#include <util/delay.h>
#include <math.h>
#include "uart.h"
#include "APDS9960_def.h"						//Header Files From Dr. Venki 
#include "i2c_master.h"							//Header Files From Dr. Venki

/******************************************************************************
*						  Function Prototypes								  *
******************************************************************************/
void USART_init();
void init_APDS9960(void);
void USART_tx_string(volatile unsigned char *data);
void ReadSensor(void);

/******************************************************************************
*					  Global Variable Declarations 							  *
******************************************************************************/
volatile unsigned char RED[10];		//Array to store Red String Value
volatile unsigned char GREEN[10];   //Array to store Green String Value
volatile unsigned char BLUE[10];	//Array to store Blue String Value
uint16_t RedData;					//Int Variable For Color Red 
uint16_t GreenData;					//Int Variable For Color Green
uint16_t BlueData;					//Int Variable For Color Blue

/******************************************************************************
*					  AT Commands Declarations 								  *
******************************************************************************/
//Test Connection 
volatile unsigned char AT[] = "AT\r\n"; 
//Set Wi-Fi Connection Mode
volatile unsigned char CWMODE[] = "AT+CWMODE=3\r\n"; 
//Retrieve Wi-Fi Information for Connection 
volatile unsigned char CWJAP[] = "AT+CWJAP=\"Wifi\",\"Password\"\r\n"; 
//Enable Connection 
volatile unsigned char CIPMUX[] = "AT+CIPMUX=0\r\n";
//Connect With ThingSpeak Through IP Address
volatile unsigned char CIPSTART[] = "AT+CIPSTART=\"TCP\",\"184.106.153.149\",80\r\n"; 
//Set Send Function Size To 125
volatile unsigned char CIPSEND[] = "AT+CIPSEND=125\r\n";
//Retrieve API Key To Write To ThingSpeak 
volatile unsigned char API[] = "GET /update?key=BTGT7XH3A9N1Y80Z&field1=";
//Field 2 String
volatile unsigned char F2[] = "&field2=";
//Field 3 String 
volatile unsigned char F3[]	= "&field3=";
//Reset The Module 
volatile unsigned char RESET[] = "AT+RST\r\n";
//Close 
volatile unsigned char CLOSE[] = "AT+CIPCLOSE\r\n";
//New Line For Printing 
volatile unsigned char NEWLINE[] = "\r\n"; 

/******************************************************************************
*						      Int Main  									  *
******************************************************************************/

int main(void){
	USART_init();							//Initialize UART
	i2c_init();								//Initialize I2C
	init_APDS9960();						//Initialize APDS9960 Sensor

	_delay_ms(5000);
	USART_tx_string(AT);					//Test Connection 
	_delay_ms(500);
	USART_tx_string(RESET);					//Reset The Module 
	_delay_ms(500);
	USART_tx_string(AT);					//Confirm Connection 
	_delay_ms(500);
	USART_tx_string(CWMODE);				//Set Wi-Fi Connection Mode
	_delay_ms(500);
	USART_tx_string(CWJAP);					//Retrieve Wi-Fi Information  
	_delay_ms(500);
	
	while(1){

		_delay_ms(1000);
		USART_tx_string(CIPMUX);			//Enable Connection Point 
		_delay_ms(1000);
		USART_tx_string(CIPSTART);			//Connect To ThingSpeak 
		_delay_ms(10000);
		USART_tx_string(CIPSEND);			//Send Size Set To 125 
		_delay_ms(1000);
		ReadSensor();						//Retrieve Sensor Values 
		USART_tx_string(API);				//Connect To API Key 
		USART_tx_string(RED);				//Send ADC Sensor Value To Field 1 
		USART_tx_string(F2);				
		USART_tx_string(GREEN);				//Send ADC Sensor Value To Field 2
		USART_tx_string(F3);
		USART_tx_string(BLUE);				//Send ADC Sensor Value To Field 3
		_delay_ms(1000);

	}
	return 0;
}//End Main 

/******************************************************************************
*						Function Declarations   							  *
******************************************************************************/
//Function To Initialize USART
void USART_init() 
{
	UBRR0H = ((UBRR) >> 8);
	UBRR0L = UBRR;
	UCSR0A |= (1<< U2X0); 
	UCSR0B |= (1 << TXEN0); //Transmission Enable
	UCSR0C |= (1 << UCSZ01) | (1 << UCSZ00); //8-Bit Data
}

//Function To Initialize APDS9960 Sensor
void init_APDS9960(void)
{
	uint8_t setup;
	
	i2c_readReg(APDS9960_WRITE, APDS9960_ID, &setup,1);
	if(setup != APDS9960_ID_1) while(1);
	setup = 1 << 1 | 1<<0 | 1<<3 | 1<<4;
	
	i2c_writeReg(APDS9960_WRITE, APDS9960_ENABLE, &setup, 1);
	setup = DEFAULT_ATIME;
	
	i2c_writeReg(APDS9960_WRITE, APDS9960_ATIME, &setup, 1);
	setup = DEFAULT_WTIME;
	
	i2c_writeReg(APDS9960_WRITE, APDS9960_WTIME, &setup, 1);
	setup = DEFAULT_PROX_PPULSE;
	
	i2c_writeReg(APDS9960_WRITE, APDS9960_PPULSE, &setup, 1);
	setup = DEFAULT_POFFSET_UR;
	
	i2c_writeReg(APDS9960_WRITE, APDS9960_POFFSET_UR, &setup, 1);
	setup = DEFAULT_POFFSET_DL;
	
	i2c_writeReg(APDS9960_WRITE, APDS9960_POFFSET_DL, &setup, 1);
	setup = DEFAULT_CONFIG1;
	
	i2c_writeReg(APDS9960_WRITE, APDS9960_CONFIG1, &setup, 1);
	setup = DEFAULT_PERS;
	
	i2c_writeReg(APDS9960_WRITE, APDS9960_PERS, &setup, 1);
	setup = DEFAULT_CONFIG2;
	
	i2c_writeReg(APDS9960_WRITE, APDS9960_CONFIG2, &setup, 1);
	setup = DEFAULT_CONFIG3;
	
	i2c_writeReg(APDS9960_WRITE, APDS9960_CONFIG3, &setup, 1);
	
}

//FUNCTION THAT READS IN 1 CHAR AT A TIME
void USART_tx_string(volatile unsigned char *data)
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

//Function That Reads The APDS9960 Sensor And Stores Color Values 
void ReadSensor(void)
{
	//Variables To Store Color's(High/Low) Values
	uint8_t RedHi, RedLo;
	uint8_t GreenHi, GreenLo;
	uint8_t BlueHi, BlueLo;

	//I2C Read Function To Retrieve Red(High/Low) Values
	i2c_readReg(APDS9960_WRITE, APDS9960_RDATAH, &RedHi, 1);
	i2c_readReg(APDS9960_WRITE, APDS9960_RDATAL, &RedLo, 1);

	//I2C Read Function To Retrieve Green(High/Low) Values 
	i2c_readReg(APDS9960_WRITE, APDS9960_GDATAH, &GreenHi, 1);
	i2c_readReg(APDS9960_WRITE, APDS9960_GDATAL, &GreenLo, 1);

	//I2C Read Function To Retrieve Blue(High/Low) Values
	i2c_readReg(APDS9960_WRITE, APDS9960_BDATAH, &BlueHi, 1);
	i2c_readReg(APDS9960_WRITE, APDS9960_BDATAL, &BlueLo, 1);

	//Shift The High Value by 8 and Combine With Low Value
	RedData   = (RedHi   << 8) | RedLo;
	GreenData = (GreenHi << 8) | GreenLo;
	BlueData  = (BlueHi  << 8) | BlueLo;

	//Make Sure The Sensor Values Do Not Exceed 255
	if (RedData > 255)
	{
		RedData = 255;
	}
	if (GreenData > 255)
	{
		GreenData = 255;
	}
	if (BlueData > 255)
	{
		BlueData = 255;
	}

	unsigned char i;
	char temp[10];
	
	//Convert Char To ASCII
	itoa(RedData, temp, 10); 
	for(i = 0 ; i < 10 ; i++)
	{
		RED[i] = temp[i]; 
	}	
	//Convert Char To ASCII
	itoa(GreenData, temp, 10); 
	for(i = 0 ; i < 10 ; i++)
	{
		GREEN[i] = temp[i]; 
	}	
	//Convert Char To ASCII
	itoa(BlueData, temp, 10); 
	for(i = 0 ; i < 10 ; i++)
	{
		BLUE[i] = temp[i]; 
	}	
		
}

