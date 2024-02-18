#ifndef __ADC_H
#define __ADC_H

#include "./SYSTEM/sys/sys.h"
#include "FreeRTOS.h"
#include "queue.h"
extern 
/******************************************************************************************/
/* ADC������ ���� */

#define ADC_ADCX_CHY_GPIO_PORT              GPIOA
#define ADC_ADCX_CHY_GPIO_PIN               GPIO_PIN_1 
#define ADC_ADCX_CHY_GPIO_CLK_ENABLE()      do{ __HAL_RCC_GPIOA_CLK_ENABLE(); }while(0)  /* PA��ʱ��ʹ�� */

#define ADC_ADCX                            ADC1 
#define ADC_ADCX_CHY                        ADC_CHANNEL_1                                /* ͨ��Y,  0 <= Y <= 17 */ 
#define ADC_ADCX_CHY_CLK_ENABLE()           do{ __HAL_RCC_ADC1_CLK_ENABLE(); }while(0)   /* ADC1 ʱ��ʹ�� */


/* ADC��ͨ��/��ͨ�� DMA�ɼ� DMA��ͨ�� ����
 * ע��: ADC1��DMAͨ��ֻ����: DMA1_Channel1, ���ֻҪ��ADC1, �����ǲ��ܸĶ���
 *       ADC2��֧��DMA�ɼ�
 *       ADC3��DMAͨ��ֻ����: DMA2_Channel5, ������ʹ�� ADC3 ����Ҫ�޸�
 */
#define ADC_ADCX_DMACx                      DMA1_Channel1
#define ADC_ADCX_DMACx_IRQn                 DMA1_Channel1_IRQn
#define ADC_ADCX_DMACx_IRQHandler           DMA1_Channel1_IRQHandler

#define ADC_ADCX_DMACx_IS_TC()              ( DMA1->ISR & (1 << 1) )    /* �ж� DMA1_Channel1 ������ɱ�־, ����һ���ٺ�����ʽ,
                                                                         * ���ܵ�����ʹ��, ֻ������if��������� 
                                                                         */
#define ADC_ADCX_DMACx_CLR_TC()             do{ DMA1->IFCR |= 1 << 1; }while(0) /* ��� DMA1_Channel1 ������ɱ�־ */

/******************************************************************************************/

void adc_init(void);                                                /* ADC��ʼ�� */
void adc_channel_set(ADC_HandleTypeDef *adc_handle, uint32_t ch,uint32_t rank, uint32_t stime); /* ADCͨ������ */
uint32_t adc_get_result(uint32_t ch);                               /* ���ĳ��ͨ��ֵ  */
uint32_t adc_get_result_average(uint32_t ch, uint8_t times);        /* �õ�ĳ��ͨ����������������ƽ��ֵ */

void adc_dma_init(uint32_t mar);                                    /* ADC DMA�ɼ���ʼ�� */
void adc_dma_enable( uint16_t cndtr);                               /* ʹ��һ��ADC DMA�ɼ����� */
typedef struct
{
uint16_t  original_adc;		//ADC��ԭʼ����
uint8_t  adc_integer;	 	//ADC�Ļ�����������
uint16_t  temp_decimal;	 	//ADC�Ļ���С������
}soil_detect_Data;
#endif 















