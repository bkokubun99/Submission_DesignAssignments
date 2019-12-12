/*
 * Assignment6.c
 *
 * Created: 12/10/2019 3:40:20 PM
 * Author : Brysen Kokubun 
 */ 

/******************************************************************************
*							Libraries										  *
******************************************************************************/
#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include "MPU6050_res_define.h"
#include "I2C_Master_H_file.h"
#include "USART_RS232_H_file.h"

/******************************************************************************
*							Function Prototypes							      *
******************************************************************************/
void MPU6050_Init();
void MPU_Start_Loc();
void Read_RawValue();
 
//Variable Declarations For MPU6050
float Acc_x,Acc_y,Acc_z;		//Acceleration Values X-Y-Z
float Temperature;
float Gyro_x,Gyro_y,Gyro_z;		//Gyro Values X-Y-Z
 
/******************************************************************************
*							Int Main										  *
******************************************************************************/
int main()
{
	//Variable Declarations 
	char buffer[20];
	char float_val[10];
	float X_a,Y_a,Z_a,t;			//Acceleration Values X-Y-Z
	float X_g=0,Y_g=0,Z_g=0;		//Gyro Values X-Y-Z
	 
	I2C_Init();						//Initialize I2C
	MPU6050_Init();					//Initialize MPU6050
	USART_Init(9600);				//Initialize UART
	 
	while(1)
	{
		Read_RawValue();			//Call Read_RawValue Function
		
		X_a = Acc_x/16384.0;		//Acceleration-X
		Y_a = Acc_y/16384.0;		//Acceleration-Y
		Z_a = Acc_z/16384.0;		//Acceleration-Z
		 
		X_g = Gyro_x/16.4;			//Gyro-X
		Y_g = Gyro_y/16.4;			//Gyro-Y
		Z_g = Gyro_z/16.4;			//Gyro-Z

		//Read/Print Acceleration-X
		dtostrf( X_a, 3, 2, float_val );			
		sprintf(buffer," Acc-X = %s g\t",float_val);			
		USART_SendString(buffer);						
		
		//Read/Print Acceleration-Y
		dtostrf( Y_a, 3, 2, float_val );					
		sprintf(buffer," Acc-Y = %s g\t",float_val);			
		USART_SendString(buffer);						
		 
		//Read/Print Acceleration-Z
		dtostrf( Z_a, 3, 2, float_val );					
		sprintf(buffer," Acc-Z = %s g\t",float_val);			
		USART_SendString(buffer);						
		 
		//Read/Print Gyro-X
		dtostrf( X_g, 3, 2, float_val );					
		sprintf(buffer," Gyro-X = %s%c/s\t",float_val,0xF8);	
		USART_SendString(buffer);		
		 
		//Read/Print Gyro-Y
		dtostrf( Y_g, 3, 2, float_val );					
		sprintf(buffer," Gyro-Y = %s%c/s\t",float_val,0xF8);	
		USART_SendString(buffer);						
		 
		//Read/Print Gyro-Z
		dtostrf( Z_g, 3, 2, float_val);					
		sprintf(buffer," Gyro-Z = %s%c/s\r\n",float_val,0xF8);	
		USART_SendString(buffer);						
		_delay_ms(1000);								
	 }
 }//End Main
 
/******************************************************************************
*							Function Definition								  *
******************************************************************************/
//Function to Initialize MPU6050 Sensor 
void MPU6050_Init()
{
	//Write Address/Write Register
	_delay_ms(150);
	I2C_Start_Wait(0xD0);
	I2C_Write(SMPLRT_DIV);
	I2C_Write(0x07);									
	I2C_Stop();
	
	//Manage Power Register
	I2C_Start_Wait(0xD0);
	I2C_Write(PWR_MGMT_1);
	I2C_Write(0x01);			
	I2C_Stop();

	//Write Configuration 
	I2C_Start_Wait(0xD0);
	I2C_Write(CONFIG);	
	I2C_Write(0x00);
	I2C_Stop();

	//Write Gyro Configuration
	I2C_Start_Wait(0xD0);
	I2C_Write(GYRO_CONFIG);
	I2C_Write(0x18);
	I2C_Stop();
	
	//Write to Interrupt
	I2C_Start_Wait(0xD0);
	I2C_Write(INT_ENABLE);
	I2C_Write(0x01);
	I2C_Stop();
}

void MPU_Start_Loc()
{
	I2C_Start_Wait(0xD0);								
	I2C_Write(ACCEL_XOUT_H);							
	I2C_Repeated_Start(0xD1);							
}

//Function to Read Values From Sensor 
void Read_RawValue()
{
	MPU_Start_Loc();									
	Acc_x = (((int)I2C_Read_Ack()<<8) | (int)I2C_Read_Ack());
	Acc_y = (((int)I2C_Read_Ack()<<8) | (int)I2C_Read_Ack());
	Acc_z = (((int)I2C_Read_Ack()<<8) | (int)I2C_Read_Ack());
	Gyro_x = (((int)I2C_Read_Ack()<<8) | (int)I2C_Read_Ack());
	Gyro_y = (((int)I2C_Read_Ack()<<8) | (int)I2C_Read_Ack());
	Gyro_z = (((int)I2C_Read_Ack()<<8) | (int)I2C_Read_Nack());
	I2C_Stop();
}

