#include "./SYSTEM/usart/usart.h"
#include "MYI2C.h"
#include "BMP280.h"
#include <string.h>
#include "OLED.h"
BMP280 bmp280_inst;
BMP280* bmp280 = &bmp280_inst;		//���ȫ�ֽṹ����������������оƬ��ROM��������

void Bmp_Init(void)
{
	MyI2C_Init();
	uint8_t Lsb,Msb;
	
	/********************������������������*********************/
	//�¶ȴ������Ľ���ֵ
	Lsb = BMP280_Read_Byte(BMP280_DIG_T1_LSB_REG);
	Msb = BMP280_Read_Byte(BMP280_DIG_T1_MSB_REG);
	bmp280->T1 = (((uint16_t)Msb)<<8) + Lsb;			//��λ�ӵ�λ
	Lsb = BMP280_Read_Byte(BMP280_DIG_T2_LSB_REG);
	Msb = BMP280_Read_Byte(BMP280_DIG_T2_MSB_REG);
	bmp280->T2 = (((uint16_t)Msb)<<8) + Lsb;		
	Lsb = BMP280_Read_Byte(BMP280_DIG_T3_LSB_REG);
	Msb = BMP280_Read_Byte(BMP280_DIG_T3_MSB_REG);
	bmp280->T3 = (((uint16_t)Msb)<<8) + Lsb;		
	
	//����ѹ�������Ľ���ֵ
	Lsb = BMP280_Read_Byte(BMP280_DIG_P1_LSB_REG);
	Msb = BMP280_Read_Byte(BMP280_DIG_P1_MSB_REG);
	bmp280->P1 = (((uint16_t)Msb)<<8) + Lsb;		
	Lsb = BMP280_Read_Byte(BMP280_DIG_P2_LSB_REG);
	Msb = BMP280_Read_Byte(BMP280_DIG_P2_MSB_REG);
	bmp280->P2 = (((uint16_t)Msb)<<8) + Lsb;	
	Lsb = BMP280_Read_Byte(BMP280_DIG_P3_LSB_REG);
	Msb = BMP280_Read_Byte(BMP280_DIG_P3_MSB_REG);
	bmp280->P3 = (((uint16_t)Msb)<<8) + Lsb;	
	Lsb = BMP280_Read_Byte(BMP280_DIG_P4_LSB_REG);
	Msb = BMP280_Read_Byte(BMP280_DIG_P4_MSB_REG);
	bmp280->P4 = (((uint16_t)Msb)<<8) + Lsb;	
	Lsb = BMP280_Read_Byte(BMP280_DIG_P5_LSB_REG);
	Msb = BMP280_Read_Byte(BMP280_DIG_P5_MSB_REG);
	bmp280->P5 = (((uint16_t)Msb)<<8) + Lsb;	
	Lsb = BMP280_Read_Byte(BMP280_DIG_P6_LSB_REG);
	Msb = BMP280_Read_Byte(BMP280_DIG_P6_MSB_REG);
	bmp280->P6 = (((uint16_t)Msb)<<8) + Lsb;	
	Lsb = BMP280_Read_Byte(BMP280_DIG_P7_LSB_REG);
	Msb = BMP280_Read_Byte(BMP280_DIG_P7_MSB_REG);
	bmp280->P7 = (((uint16_t)Msb)<<8) + Lsb;	
	Lsb = BMP280_Read_Byte(BMP280_DIG_P8_LSB_REG);
	Msb = BMP280_Read_Byte(BMP280_DIG_P8_MSB_REG);
	bmp280->P8 = (((uint16_t)Msb)<<8) + Lsb;	
	Lsb = BMP280_Read_Byte(BMP280_DIG_P9_LSB_REG);
	Msb = BMP280_Read_Byte(BMP280_DIG_P9_MSB_REG);
	bmp280->P9 = (((uint16_t)Msb)<<8) + Lsb;	
	/******************************************************/
	BMP280_Write_Byte(BMP280_RESET_REG,BMP280_RESET_VALUE);	//����λ�Ĵ���д�����ֵ
	
	BMP_OVERSAMPLE_MODE			BMP_OVERSAMPLE_MODEStructure;
	BMP_OVERSAMPLE_MODEStructure.P_Osample = BMP280_P_MODE_3;
	BMP_OVERSAMPLE_MODEStructure.T_Osample = BMP280_T_MODE_1;
	BMP_OVERSAMPLE_MODEStructure.WORKMODE  = BMP280_NORMAL_MODE;
	BMP280_Set_TemOversamp(&BMP_OVERSAMPLE_MODEStructure);
	
	BMP_CONFIG					BMP_CONFIGStructure;
	BMP_CONFIGStructure.T_SB = BMP280_T_SB1;
	BMP_CONFIGStructure.FILTER_COEFFICIENT = BMP280_FILTER_MODE_4;
	BMP_CONFIGStructure.SPI_EN = DISABLE;
	
	BMP280_Set_Standby_FILTER(&BMP_CONFIGStructure);
}
//����BMP���������� MODE 
//BMP280_SLEEP_MODE||BMP280_FORCED_MODE||BMP280_NORMAL_MODE
void BMP280_Set_TemOversamp(BMP_OVERSAMPLE_MODE * Oversample_Mode)
{
	uint8_t Regtmp;
	Regtmp = ((Oversample_Mode->T_Osample)<<5)|
			 ((Oversample_Mode->P_Osample)<<2)|
			 ((Oversample_Mode)->WORKMODE);
	
	BMP280_Write_Byte(BMP280_CTRLMEAS_REG,Regtmp);
}


//���ñ���ʱ����˲�����Ƶ����
void BMP280_Set_Standby_FILTER(BMP_CONFIG * BMP_Config)
{
	uint8_t Regtmp;
	Regtmp = ((BMP_Config->T_SB)<<5)|
			 ((BMP_Config->FILTER_COEFFICIENT)<<2)|
			 ((BMP_Config->SPI_EN));
	
	BMP280_Write_Byte(BMP280_CONFIG_REG,Regtmp);
}

//��ȡBMP��ǰ״̬ ԭʼֵ
//status_flag = BMP280_MEASURING ||
//			 	BMP280_IM_UPDATE
uint8_t  BMP280_GetStatus(uint8_t status_flag)
{
	uint8_t flag;
	flag = BMP280_Read_Byte(BMP280_STATUS_REG);
	if(flag&status_flag)	return SET;
	else return RESET;
}





static uint8_t BMP280_Read_Byte(uint8_t reg)
{
	uint8_t rec_data;
	MyI2C_Start();
	MyI2C_SendByte(BMP280_ADDRESS<<1|0);
	MyI2C_ReceiveAck();
	MyI2C_SendByte(reg);
	MyI2C_ReceiveAck();
	
	MyI2C_Start();
	MyI2C_SendByte(BMP280_ADDRESS<<1|1);
	MyI2C_ReceiveAck();
	rec_data = MyI2C_ReceiveByte();	//��Ӧ��
	MyI2C_Stop();
	return rec_data;
}

 void BMP280_Write_Byte(uint8_t reg,uint8_t data)
{
	MyI2C_Start();
	MyI2C_SendByte(BMP280_ADDRESS<<1);
	MyI2C_ReceiveAck();
	MyI2C_SendByte(reg);
	MyI2C_ReceiveAck();
	
	MyI2C_SendByte(data);
	MyI2C_ReceiveAck();
	MyI2C_Stop();
}
uint8_t BMP280_ReadID(void)
{
	return BMP280_Read_Byte(BMP280_CHIPID_REG);
}
/*******************��Ҫ����*********************/
/****************��ȡ��������ȷֵ****************/
//����ѹֵ-Pa
/**************************������ֵת����ֵ*************************************/
BMP280_S32_t t_fine;			//���ڼ��㲹��
//���ø��㲹��
#ifdef USE_FIXED_POINT_COMPENSATE
// Returns temperature in DegC, resolution is 0.01 DegC. Output value of ��5123�� equals 51.23 DegC. 
// t_fine carries fine temperature as global value
BMP280_S32_t bmp280_compensate_T_int32(BMP280_S32_t adc_T)
{
	BMP280_S32_t var1, var2, T;
	var1 = ((((adc_T>>3) - ((BMP280_S32_t)dig_T1<<1))) * ((BMP280_S32_t)dig_T2)) >> 11;
	var2 = (((((adc_T>>4) - ((BMP280_S32_t)dig_T1)) * ((adc_T>>4) - ((BMP280_S32_t)dig_T1))) >> 12) * 
	((BMP280_S32_t)dig_T3)) >> 14;
	t_fine = var1 + var2;
	T = (t_fine * 5 + 128) >> 8;
	return T;
}

// Returns pressure in Pa as unsigned 32 bit integer in Q24.8 format (24 integer bits and 8 fractional bits).
// Output value of ��24674867�� represents 24674867/256 = 96386.2 Pa = 963.862 hPa
BMP280_U32_t bmp280_compensate_P_int64(BMP280_S32_t adc_P)
{
	BMP280_S64_t var1, var2, p;
	var1 = ((BMP280_S64_t)t_fine) - 128000;
	var2 = var1 * var1 * (BMP280_S64_t)dig_P6;
	var2 = var2 + ((var1*(BMP280_S64_t)dig_P5)<<17);
	var2 = var2 + (((BMP280_S64_t)dig_P4)<<35);
	var1 = ((var1 * var1 * (BMP280_S64_t)dig_P3)>>8) + ((var1 * (BMP280_S64_t)dig_P2)<<12);
	var1 = (((((BMP280_S64_t)1)<<47)+var1))*((BMP280_S64_t)dig_P1)>>33;
	if (var1 == 0)
	{
	return 0; // avoid exception caused by division by zero
	}
	p = 1048576-adc_P;
	p = (((p<<31)-var2)*3125)/var1;
	var1 = (((BMP280_S64_t)dig_P9) * (p>>13) * (p>>13)) >> 25;
	var2 = (((BMP280_S64_t)dig_P8) * p) >> 19;
	p = ((p + var1 + var2) >> 8) + (((BMP280_S64_t)dig_P7)<<4);
	return (BMP280_U32_t)p;
}


/***********************************CUT*************************************/
#else
/**************************������ֵת����ֵ*************************************/
// Returns temperature in DegC, double precision. Output value of ��51.23�� equals 51.23 DegC.
// t_fine carries fine temperature as global value
double bmp280_compensate_T_double(BMP280_S32_t adc_T)
{
	double var1, var2, T;
	var1 = (((double)adc_T)/16384.0 - ((double)dig_T1)/1024.0) * ((double)dig_T2);
	var2 = ((((double)adc_T)/131072.0 - ((double)dig_T1)/8192.0) *
	(((double)adc_T)/131072.0 - ((double) dig_T1)/8192.0)) * ((double)dig_T3);
	t_fine = (BMP280_S32_t)(var1 + var2);
	T = (var1 + var2) / 5120.0;
	return T;
}

// Returns pressure in Pa as double. Output value of ��96386.2�� equals 96386.2 Pa = 963.862 hPa
double bmp280_compensate_P_double(BMP280_S32_t adc_P)
{
	double var1, var2, p;
	var1 = ((double)t_fine/2.0) - 64000.0;
	var2 = var1 * var1 * ((double)dig_P6) / 32768.0;
	var2 = var2 + var1 * ((double)dig_P5) * 2.0;
	var2 = (var2/4.0)+(((double)dig_P4) * 65536.0);
	var1 = (((double)dig_P3) * var1 * var1 / 524288.0 + ((double)dig_P2) * var1) / 524288.0;
	var1 = (1.0 + var1 / 32768.0)*((double)dig_P1);
	if (var1 == 0.0)
	{
	return 0; // avoid exception caused by division by zero
	}
	p = 1048576.0 - (double)adc_P;
	p = (p - (var2 / 4096.0)) * 6250.0 / var1;
	var1 = ((double)dig_P9) * p * p / 2147483648.0;
	var2 = p * ((double)dig_P8) / 32768.0;
	p = p + (var1 + var2 + ((double)dig_P7)) / 16.0;
	return p;
}
#endif
double BMP280_Get_Pressure(void)
{
	uint8_t XLsb,Lsb, Msb;
	long signed Bit32;
	double pressure;
	XLsb = BMP280_Read_Byte(BMP280_PRESSURE_XLSB_REG);
	Lsb	 = BMP280_Read_Byte(BMP280_PRESSURE_LSB_REG);
	Msb	 = BMP280_Read_Byte(BMP280_PRESSURE_MSB_REG);
	Bit32 = ((long)(Msb << 12))|((long)(Lsb << 4))|(XLsb>>4);	//�Ĵ�����ֵ,���һ��������
	pressure = bmp280_compensate_P_double(Bit32);
	return pressure;
}

//�¶�ֵ-��
double BMP280_Get_Temperature(void)
{
	uint8_t XLsb,Lsb, Msb;
	long signed Bit32;
	double temperature;
	XLsb = BMP280_Read_Byte(BMP280_TEMPERATURE_XLSB_REG);
	Lsb	 = BMP280_Read_Byte(BMP280_TEMPERATURE_LSB_REG);
	Msb	 = BMP280_Read_Byte(BMP280_TEMPERATURE_MSB_REG);
	Bit32 = ((long)(Msb << 12))|((long)(Lsb << 4))|(XLsb>>4);	//�Ĵ�����ֵ,���һ��������
	temperature = bmp280_compensate_T_double(Bit32);
	return temperature;
}
/***************************************END OF LINE*********************************************/
void bmp_send(char *sendstring){
    HAL_UART_Transmit(&g_uart1_handle,(uint8_t *)sendstring,strlen(sendstring), 1000);    /* �������� */
    while(__HAL_UART_GET_FLAG(&g_uart1_handle,UART_FLAG_TC) != SET){};           /* �ȴ����ͽ��� */    
}

//void bmp_show(void){
//        if (g_usart_rx_sta & 0x8000)        /* ���յ�������? */
//        {
//            while(__HAL_UART_GET_FLAG(&g_uart1_handle,UART_FLAG_TC) != SET);           /* �ȴ����ͽ��� */
//            OLED_ShowString(3,50,g_usart_rx_buf,OLED_6X8); 
//            g_usart_rx_sta2 = 0;
//        }
//};