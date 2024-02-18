
#ifndef __USART_H
#define __USART_H

#include "stdio.h"
#include "./SYSTEM/sys/sys.h"


/******************************************************************************************/
/* ���� �� ���� ���� 
 * Ĭ�������USART1��.
 * ע��: ͨ���޸��⼸���궨��,����֧��USART1~UART5����һ������.
 */
 /******************************************************************************************/
 //usart1
#define USART_TX_GPIO_PORT                  GPIOA
#define USART_TX_GPIO_PIN                   GPIO_PIN_9
#define USART_TX_GPIO_CLK_ENABLE()          do{ __HAL_RCC_GPIOA_CLK_ENABLE(); }while(0)   /* PA��ʱ��ʹ�� */
#define USART_RX_GPIO_PORT                  GPIOA
#define USART_RX_GPIO_PIN                   GPIO_PIN_10
#define USART_RX_GPIO_CLK_ENABLE()          do{ __HAL_RCC_GPIOA_CLK_ENABLE(); }while(0)   /* PA��ʱ��ʹ�� */
#define USART_UX                            USART1
#define USART_UX_IRQn                       USART1_IRQn
#define USART_UX_IRQHandler                 USART1_IRQHandler 
#define USART_UX_CLK_ENABLE()               do{ __HAL_RCC_USART1_CLK_ENABLE(); }while(0)  /* USART1 ʱ��ʹ�� */
/******************************************************************************************/
//usart2
#define USART2_TX_GPIO_PORT                  GPIOA
#define USART2_TX_GPIO_PIN                   GPIO_PIN_2
#define USART2_TX_GPIO_CLK_ENABLE()          do{ __HAL_RCC_GPIOA_CLK_ENABLE(); }while(0)   /* PA��ʱ��ʹ�� */
#define USART2_RX_GPIO_PORT                  GPIOA
#define USART2_RX_GPIO_PIN                   GPIO_PIN_3
#define USART2_RX_GPIO_CLK_ENABLE()          do{ __HAL_RCC_GPIOA_CLK_ENABLE(); }while(0)   /* PA��ʱ��ʹ�� */
#define USART2_UX                            USART2
#define USART2_UX_IRQn                       USART2_IRQn
#define USART2_UX_IRQHandler                 USART2_IRQHandler
#define USART2_UX_CLK_ENABLE()               do{ __HAL_RCC_USART2_CLK_ENABLE(); }while(0)  /* USART2 ʱ��ʹ�� */
/******************************************************************************************/
 //usart3
#define USART3_TX_GPIO_PORT                  GPIOB
#define USART3_TX_GPIO_PIN                   GPIO_PIN_10
#define USART3_TX_GPIO_CLK_ENABLE()          do{ __HAL_RCC_GPIOB_CLK_ENABLE(); }while(0)   /* PB��ʱ��ʹ�� */
#define USART3_RX_GPIO_PORT                  GPIOB
#define USART3_RX_GPIO_PIN                   GPIO_PIN_11
#define USART3_RX_GPIO_CLK_ENABLE()          do{ __HAL_RCC_GPIOB_CLK_ENABLE(); }while(0)   /* PB��ʱ��ʹ�� */
#define USART3_UX                            USART3
#define USART3_UX_IRQn                       USART3_IRQn
#define USART3_UX_IRQHandler                 USART3_IRQHandler 
#define USART3_UX_CLK_ENABLE()               do{ __HAL_RCC_USART3_CLK_ENABLE(); }while(0)  /* USART3 ʱ��ʹ�� */
/******************************************************************************************/
//USART1
#define USART_REC_LEN               200         /* �����������ֽ��� 17 */
#define USART_EN_RX                 1           /* ʹ�ܣ�1��/��ֹ��0������1���� */
#define RXBUFFERSIZE   1                        /* �����С */
//

//USART2
#define USART2_REC_LEN               200         /* �����������ֽ��� 200 */
#define USART2_EN_RX                 1           /* ʹ�ܣ�1��/��ֹ��0������1���� */
#define RXBUFFERSIZE_2   1                        /* �����С */
//

//USART2
#define USART3_REC_LEN               200         /* �����������ֽ��� 200 */
#define USART3_EN_RX                 1           /* ʹ�ܣ�1��/��ֹ��0������1���� */
#define RXBUFFERSIZE_3   1                        /* �����С */
//
/******************************************************************************************/

//USART1/////////////////////////////////////////////////////////////////////////////////
extern UART_HandleTypeDef g_uart1_handle;       /* HAL UART��� */
extern char  g_usart_rx_buf[USART_REC_LEN];  /* ���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� */
extern uint16_t g_usart_rx_sta;                 /* ����״̬��� */
extern uint8_t g_rx_buffer[RXBUFFERSIZE];       /* HAL��USART����Buffer */
/////////////////////////////////////////////////////////////////////////////////////////


//usart2/////////////////////////////////////////////////////////////////////////////////
extern UART_HandleTypeDef g_uart2_handle;       /* HAL UART��� */
extern char  g_usart_rx_buf2[USART2_REC_LEN];  /* ���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� */
extern uint16_t g_usart_rx_sta2;                 /* ����״̬��� */
extern uint8_t g_rx_buffer2[RXBUFFERSIZE_2];       /* HAL��USART����Buffer */
/////////////////////////////////////////////////////////////////////////////////////////


//usart3/////////////////////////////////////////////////////////////////////////////////
extern UART_HandleTypeDef g_uart3_handle;       /* HAL UART��� */
extern char  g_usart_rx_buf3[USART3_REC_LEN];  /* ���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� */
extern uint16_t g_usart_rx_sta3;                 /* ����״̬��� */
extern uint8_t g_rx_buffer3[RXBUFFERSIZE_3];       /* HAL��USART����Buffer */
/////////////////////////////////////////////////////////////////////////////////////////


extern char IntermediateBuffer[USART_REC_LEN]; //�н��ַ���

void usart_init(uint32_t bound,uint32_t bound2,uint32_t bound3);                /* ���ڳ�ʼ������ */
void sendDecimalToHex(char *decimalString);
void SendFormattedData(double value);
//
#endif



