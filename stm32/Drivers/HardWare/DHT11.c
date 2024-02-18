#include "./SYSTEM/delay/delay.h"
#include "DHT11.h"


void DHT11_GPIO_Config(void)
{		
	GPIO_InitTypeDef GPIO_InitStructure;
  __HAL_RCC_GPIOA_CLK_ENABLE();
  	GPIO_InitStructure.Pin = DHT11_Out_Pin;	
 	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;   
  	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_MEDIUM; 
  	HAL_GPIO_Init(DHT11, &GPIO_InitStructure);		  
}

static void DHT11_Mode_IPU(void)
{
 	  GPIO_InitTypeDef GPIO_InitStructure;
	  GPIO_InitStructure.Pin = DHT11_Out_Pin;
	  GPIO_InitStructure.Mode = GPIO_MODE_INPUT ; 
	  HAL_GPIO_Init(DHT11, &GPIO_InitStructure);	 
}

static void DHT11_Mode_Out_PP(void)
{
 	GPIO_InitTypeDef GPIO_InitStructure;

  	GPIO_InitStructure.Pin = DHT11_Out_Pin;	

  	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;   

  	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_MEDIUM;

  	HAL_GPIO_Init(DHT11, &GPIO_InitStructure);	 	 
}

static uint8_t Read_Byte(void)
{
	uint8_t i, temp=0;

	for (i=0; i<8; i++)    
	{	 
		while (DHT11_DATA_IN() == RESET);

		delay_us(40);    	  

		if (DHT11_DATA_IN() == SET)
		{
			while(DHT11_DATA_IN() == SET);

			temp |= (uint8_t)(0x01 << (7 - i));
		}
		else
		{			   
			temp &= (uint8_t) ~ (0x01<<(7-i)); 
		}
	}
	return temp;
}

uint8_t Read_DHT11(DHT11_Data_TypeDef *DHT11_Data)
{  
	DHT11_Mode_Out_PP();
	DHT11_DATA_OUT(LOW);
	delay_ms(18);

	DHT11_DATA_OUT(HIGH); 

	delay_us(30);  

	DHT11_Mode_IPU();

	if(DHT11_DATA_IN() == RESET)     
	{
		while(DHT11_DATA_IN() == RESET);

		while(DHT11_DATA_IN() == SET);

		DHT11_Data -> humi_int = Read_Byte();

		DHT11_Data -> humi_deci = Read_Byte();

		DHT11_Data -> temp_int = Read_Byte();

		DHT11_Data -> temp_deci = Read_Byte();

		DHT11_Data -> check_sum= Read_Byte();


		DHT11_Mode_Out_PP();
		DHT11_DATA_OUT(HIGH);

		if (DHT11_Data -> check_sum == DHT11_Data -> humi_int + DHT11_Data -> humi_deci + DHT11_Data -> temp_int + DHT11_Data -> temp_deci)
			return SUCCESS;
		else 
			return ERROR;
	}
	else
	{		
		return ERROR;
	}   
}
