#ifndef __DHT11_H
#define __DHT11_H
#include "./SYSTEM/sys/sys.h"
#define DHT11_Out_Pin		GPIO_PIN_0
#define DHT11_Out_RCC		RCC_APB2Periph_GPIOA
#define DHT11		GPIOA

#define HIGH  1
#define LOW   0

#define DHT11_DATA_OUT(a)	HAL_GPIO_WritePin(DHT11, DHT11_Out_Pin, (a) ? GPIO_PIN_SET : GPIO_PIN_RESET)
					
#define  DHT11_DATA_IN()	   HAL_GPIO_ReadPin(DHT11, DHT11_Out_Pin)

typedef struct
{
	uint8_t  humi_int;		//湿度的整数部分
	uint8_t  humi_deci;	 	//湿度的小数部分
	uint8_t  temp_int;	 	//温度的整数部分
	uint8_t  temp_deci;	 	//温度的小数部分
	uint8_t  check_sum;	 	//校验和
		                 
}DHT11_Data_TypeDef;

void DHT11_GPIO_Config(void);
static void DHT11_Mode_IPU(void);
static void DHT11_Mode_Out_PP(void);
uint8_t Read_DHT11(DHT11_Data_TypeDef *DHT11_Data);
static uint8_t Read_Byte(void);

#endif
