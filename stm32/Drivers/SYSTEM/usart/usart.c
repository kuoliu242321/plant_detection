#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/usart/usart.h"
#include "FreeRTOS.h"
#include "queue.h"

//extern QueueHandle_t xQueueIRQcmd;

/* ���ʹ��os,����������ͷ�ļ�����. */
/******************************************************************************************/
/* �������´���, ֧��printf����, ������Ҫѡ��use MicroLIB */

#if 1

#if (__ARMCC_VERSION >= 6010050)            /* ʹ��AC6������ʱ */
__asm(".global __use_no_semihosting\n\t");  /* ������ʹ�ð�����ģʽ */
__asm(".global __ARM_use_no_argv \n\t");    /* AC6����Ҫ����main����Ϊ�޲�����ʽ�����򲿷����̿��ܳ��ְ�����ģʽ */

#else
/* ʹ��AC5������ʱ, Ҫ�����ﶨ��__FILE �� ��ʹ�ð�����ģʽ */
#pragma import(__use_no_semihosting)

struct __FILE
{
    int handle;
    /* Whatever you require here. If the only file you are using is */
    /* standard output using printf() for debugging, no file handling */
    /* is required. */
};

#endif

/* ��ʹ�ð�����ģʽ��������Ҫ�ض���_ttywrch\_sys_exit\_sys_command_string����,��ͬʱ����AC6��AC5ģʽ */
int _ttywrch(int ch)
{
    ch = ch;
    return ch;
}

/* ����_sys_exit()�Ա���ʹ�ð�����ģʽ */
void _sys_exit(int x)
{
    x = x;
}

char *_sys_command_string(char *cmd, int len)
{
    return NULL;
}


/* FILE �� stdio.h���涨��. */
FILE __stdout;

/* MDK����Ҫ�ض���fputc����, printf�������ջ�ͨ������fputc����ַ��������� */
int fputc(int ch, FILE *f)
{
//////////////////////////////////////////////////////////////////////////////////////   
#if USART_EN_RX /*���ʹ���˽���*/
    {
    while ((USART_UX->SR & 0X40) == 0);     /* �ȴ���һ���ַ�������� */

    USART_UX->DR = (uint8_t)ch;             /* ��Ҫ���͵��ַ� ch д�뵽DR�Ĵ��� */
    return ch;
    }
#endif
////////////////////////////////////////////////////////////////////////////////////////
//#if USART2_EN_RX /*���ʹ���˽���*/
//    {
//    while ((USART2_UX->SR & 0X40) == 0);     /* �ȴ���һ���ַ�������� */

//    USART2_UX->DR = (uint8_t)ch;             /* ��Ҫ���͵��ַ� ch д�뵽DR�Ĵ��� */
//    //return ch;
//    }
//#endif
////////////////////////////////////////////////////////////////////////////////////////
//#if USART3_EN_RX /*���ʹ���˽���*/
//    {
//    while ((USART3_UX->SR & 0X40) == 0);     /* �ȴ���һ���ַ�������� */

//    USART3_UX->DR = (uint8_t)ch;             /* ��Ҫ���͵��ַ� ch д�뵽DR�Ĵ��� */
//    return ch;
//    }
//#endif
////////////////////////////////////////////////////////////////////////////////////////
//    
}
#endif
/******************************************************************************************/

char IntermediateBuffer[USART_REC_LEN];//�н��ַ���

/////////////////////////////////////////////////////////////////////////////////////
//#if USART_EN_RX /*���ʹ���˽���*/

/* ���ջ���, ���USART_REC_LEN���ֽ�. */
char g_usart_rx_buf[USART_REC_LEN];

uint16_t g_usart_rx_sta = 0;

uint8_t g_rx_buffer[RXBUFFERSIZE];  /* HAL��ʹ�õĴ��ڽ��ջ��� */

UART_HandleTypeDef g_uart1_handle;  /* UART��� */

/////////////////////////////////////////////////////////////////////////////////////
//#if USART2_EN_RX /*���ʹ���˽���*/
 
/* ���ջ���, ���USART_REC_LEN���ֽ�. */
char g_usart_rx_buf2[USART2_REC_LEN];

uint16_t g_usart_rx_sta2 = 0;

uint8_t g_rx_buffer2[RXBUFFERSIZE_2];  /* HAL��ʹ�õĴ��ڽ��ջ��� */

UART_HandleTypeDef g_uart2_handle;  /* UART��� */
/////////////////////////////////////////////////////////////////////////////////////
//#if USART3_EN_RX /*���ʹ���˽���*/

/* ���ջ���, ���USART_REC_LEN���ֽ�. */
char g_usart_rx_buf3[USART3_REC_LEN];

uint16_t g_usart_rx_sta3 = 0;

uint8_t g_rx_buffer3[RXBUFFERSIZE_3];  /* HAL��ʹ�õĴ��ڽ��ջ��� */

UART_HandleTypeDef g_uart3_handle;  /* UART��� */

/////////////////////////////////////////////////////////////////////////////////////




/**
 * @brief       �������ݽ��ջص�����
                ���ݴ������������
 * @param       huart:���ھ��
 * @retval      ��
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
//usaart1    
    if (huart->Instance == USART_UX)                    /* ����Ǵ���1 */
    {
        if ((g_usart_rx_sta & 0x8000) == 0)             /* ����δ��� */
        {
            if (g_usart_rx_sta & 0x4000)                /* ���յ���0x0d�����س����� */
            {
                if (g_rx_buffer[0] != 0x0a)             /* ���յ��Ĳ���0x0a�������ǻ��м��� */
                {
                    g_usart_rx_sta = 0;                 /* ���մ���,���¿�ʼ */
                }
                else                                    /* ���յ�����0x0a�������м��� */
                {
                    g_usart_rx_sta |= 0x8000;           /* ��������� */
                }
            }
            else                                        /* ��û�յ�0X0d�����س����� */
            {
                if (g_rx_buffer[0] == 0x0d)
                    g_usart_rx_sta |= 0x4000;
                else
                {
                    g_usart_rx_buf[g_usart_rx_sta & 0X3FFF] = g_rx_buffer[0];
                    g_usart_rx_sta++;

                    if (g_usart_rx_sta > (USART_REC_LEN - 1))
                    {
                        g_usart_rx_sta = 0;             /* �������ݴ���,���¿�ʼ���� */
                    }
                }
            }
        }

        HAL_UART_Receive_IT(&g_uart1_handle, (uint8_t *)g_rx_buffer, RXBUFFERSIZE);///////////////////////////////////////////////////////////
    }
    
    
//usart2   
    if (huart->Instance == USART2_UX)                    /* ����Ǵ���2 */
    {

        if ((g_usart_rx_sta2 & 0x8000) == 0)             /* ����δ��� */
        {
            if (g_usart_rx_sta2 & 0x4000)                /* ���յ���0x0d�����س����� */
            {
                if (g_rx_buffer2[0] != 0x0a)             /* ���յ��Ĳ���0x0a�������ǻ��м��� */
                {
                    g_usart_rx_sta2 = 0;                 /* ���մ���,���¿�ʼ */
                }
                else                                    /* ���յ�����0x0a�������м��� */
                {
                    g_usart_rx_sta2 |= 0x8000;           /* ��������� */
                }
            }
            else                                        /* ��û�յ�0X0d�����س����� */
            {
                if (g_rx_buffer2[0] == 0x0d)
                    g_usart_rx_sta2 |= 0x4000;
                else
                {
                    g_usart_rx_buf2[g_usart_rx_sta2 & 0X3FFF] = g_rx_buffer2[0];
                    g_usart_rx_sta2++;

                    if (g_usart_rx_sta2 > (USART2_REC_LEN - 1))
                    {
                        g_usart_rx_sta2 = 0;             /* �������ݴ���,���¿�ʼ���� */
                    }
                }
            }
        }

        HAL_UART_Receive_IT(&g_uart2_handle, (uint8_t *)g_rx_buffer2, RXBUFFERSIZE_2);////////////////////////////////////////////////////

    }
    
    
//usart3
    if (huart->Instance == USART3_UX)                    /* ����Ǵ���2 */
    {
        if ((g_usart_rx_sta3 & 0x8000) == 0)             /* ����δ��� */
        {
            if (g_usart_rx_sta3 & 0x4000)                /* ���յ���0x0d�����س����� */
            {
                if (g_rx_buffer3[0] != 0x0a)             /* ���յ��Ĳ���0x0a�������ǻ��м��� */
                {
                    g_usart_rx_sta3 = 0;                 /* ���մ���,���¿�ʼ */
                }
                else                                    /* ���յ�����0x0a�������м��� */
                {
                    g_usart_rx_sta3 |= 0x8000;           /* ��������� */
                }
            }
            else                                        /* ��û�յ�0X0d�����س����� */
            {
                if (g_rx_buffer3[0] == 0x0d)
                    g_usart_rx_sta3 |= 0x4000;
                else
                {
                    g_usart_rx_buf3[g_usart_rx_sta3 & 0X3FFF] = g_rx_buffer3[0];
                    g_usart_rx_sta3++;

                    if (g_usart_rx_sta3 > (USART3_REC_LEN - 1))
                    {
                        g_usart_rx_sta3 = 0;             /* �������ݴ���,���¿�ʼ���� */
                    }
                }
            }
        }

        HAL_UART_Receive_IT(&g_uart3_handle, (uint8_t *)g_rx_buffer3, RXBUFFERSIZE_3);//////////////////////////////////////////////
    }    
    
}






//*****************************************************************************
void USART_UX_IRQHandler(void)
{

    HAL_UART_IRQHandler(&g_uart1_handle);   /* ����HAL���жϴ����ú��� */
    //printf("%s",g_rx_buffer);

}

/**
 * @brief       ����2�жϷ�����
 * @param       ��
 * @retval      ��
 */
void USART2_UX_IRQHandler(void)
{
//    BaseType_t xHigherPriorityTaskWoken = pdFALSE; // ��ʼֵΪ pdFALSE
    HAL_UART_IRQHandler(&g_uart2_handle);   /* ����HAL���жϴ����ú��� */
//    printf("%s",g_rx_buffer2);
//    xQueueOverwriteFromISR(xQueueIRQcmd, &g_rx_buffer2, &xHigherPriorityTaskWoken);
//    // ���˳��жϷ�����ʱ������Ƿ���Ҫ���Ѹ������ȼ�������
//    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}


/**
 * @brief       ����3�жϷ�����
 * @param       ��
 * @retval      ��
 */
void USART3_UX_IRQHandler(void)
{
    HAL_UART_IRQHandler(&g_uart3_handle);   /* ����HAL���жϴ����ú��� */
    //printf("%s",g_rx_buffer3);
}
//*****************************************************************************

/**
 * @brief       ����X��ʼ������
 * @param       baudrate: ������, �����Լ���Ҫ���ò�����ֵ
 * @note        ע��: ����������ȷ��ʱ��Դ, ���򴮿ڲ����ʾͻ������쳣.
 *              �����USART��ʱ��Դ��sys_stm32_clock_init()�������Ѿ����ù���.
 * @retval      ��
 */
void usart_init(uint32_t baudrate,uint32_t baudrate2,uint32_t baudrate3)
{
    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    /*UART1 ��ʼ������*/
    g_uart1_handle.Instance = USART_UX;                                       /* USART_UX */
    g_uart1_handle.Init.BaudRate = baudrate;                                  /* ������ */
    g_uart1_handle.Init.WordLength = UART_WORDLENGTH_8B;                      /* �ֳ�Ϊ8λ���ݸ�ʽ */
    g_uart1_handle.Init.StopBits = UART_STOPBITS_1;                           /* һ��ֹͣλ */
    g_uart1_handle.Init.Parity = UART_PARITY_NONE;                            /* ����żУ��λ */
    g_uart1_handle.Init.HwFlowCtl = UART_HWCONTROL_NONE;                      /* ��Ӳ������ */
    g_uart1_handle.Init.Mode = UART_MODE_TX_RX;                               /* �շ�ģʽ */
    HAL_UART_Init(&g_uart1_handle);                                           /* HAL_UART_Init()��ʹ��UART1 */

    /* �ú����Ὺ�������жϣ���־λUART_IT_RXNE���������ý��ջ����Լ����ջ��������������� */
    HAL_UART_Receive_IT(&g_uart1_handle, (uint8_t *)g_rx_buffer, RXBUFFERSIZE); 
    
    ///////////////////////////////////////////////////////////////////////////////////////////////////////    
    //usart2
    /*UART ��ʼ������*/
    g_uart2_handle.Instance = USART2_UX;                                       /* USART_UX */
    g_uart2_handle.Init.BaudRate = baudrate2;                                  /* ������ */
    g_uart2_handle.Init.WordLength = UART_WORDLENGTH_8B;                      /* �ֳ�Ϊ8λ���ݸ�ʽ */
    g_uart2_handle.Init.StopBits = UART_STOPBITS_1;                           /* һ��ֹͣλ */
    g_uart2_handle.Init.Parity = UART_PARITY_NONE;                            /* ����żУ��λ */
    g_uart2_handle.Init.HwFlowCtl = UART_HWCONTROL_NONE;                      /* ��Ӳ������ */
    g_uart2_handle.Init.Mode = UART_MODE_TX_RX;                               /* �շ�ģʽ */
    HAL_UART_Init(&g_uart2_handle);                                           /* HAL_UART_Init()��ʹ��UART1 */

    /* �ú����Ὺ�������жϣ���־λUART_IT_RXNE���������ý��ջ����Լ����ջ��������������� */
    HAL_UART_Receive_IT(&g_uart2_handle, (uint8_t *)g_rx_buffer2, RXBUFFERSIZE_2); 
    
    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    //usart3
    /*UART ��ʼ������*/
    g_uart3_handle.Instance = USART3_UX;                                       /* USART_UX */
    g_uart3_handle.Init.BaudRate = baudrate3;                                  /* ������ */
    g_uart3_handle.Init.WordLength = UART_WORDLENGTH_8B;                      /* �ֳ�Ϊ8λ���ݸ�ʽ */
    g_uart3_handle.Init.StopBits = UART_STOPBITS_1;                           /* һ��ֹͣλ */
    g_uart3_handle.Init.Parity = UART_PARITY_NONE;                            /* ����żУ��λ */
    g_uart3_handle.Init.HwFlowCtl = UART_HWCONTROL_NONE;                      /* ��Ӳ������ */
    g_uart3_handle.Init.Mode = UART_MODE_TX_RX;                               /* �շ�ģʽ */
    HAL_UART_Init(&g_uart3_handle);                                           /* HAL_UART_Init()��ʹ��UART1 */

    /* �ú����Ὺ�������жϣ���־λUART_IT_RXNE���������ý��ջ����Լ����ջ��������������� */
    HAL_UART_Receive_IT(&g_uart3_handle, (uint8_t *)g_rx_buffer3, RXBUFFERSIZE_3); 
    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    
}





/**
 * @brief       UART�ײ��ʼ������
 * @param       huart: UART�������ָ��
 * @note        �˺����ᱻHAL_UART_Init()����
 *              ���ʱ��ʹ�ܣ��������ã��ж�����
 * @retval      ��
 */
void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
    GPIO_InitTypeDef gpio_init_struct;
    
    
//usart1/////////////////////////////////////////////////////////
    if (huart->Instance == USART_UX)                            /* ����Ǵ���1�����д���1 MSP��ʼ�� */
    {
        USART_TX_GPIO_CLK_ENABLE();                             /* ʹ�ܴ���TX��ʱ�� */
        USART_RX_GPIO_CLK_ENABLE();                             /* ʹ�ܴ���RX��ʱ�� */
        USART_UX_CLK_ENABLE();                                  /* ʹ�ܴ���ʱ�� */

        gpio_init_struct.Pin = USART_TX_GPIO_PIN;               /* ���ڷ������ź� */
        gpio_init_struct.Mode = GPIO_MODE_AF_PP;                /* ����������� */
        gpio_init_struct.Pull = GPIO_PULLUP;                    /* ���� */
        gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;          /* IO�ٶ�����Ϊ���� */
        HAL_GPIO_Init(USART_TX_GPIO_PORT, &gpio_init_struct);
                
        gpio_init_struct.Pin = USART_RX_GPIO_PIN;               /* ����RX�� ģʽ���� */
        gpio_init_struct.Mode = GPIO_MODE_INPUT;//GPIO_MODE_AF_INPUT;    
        HAL_GPIO_Init(USART_RX_GPIO_PORT, &gpio_init_struct);   /* ����RX�� �������ó�����ģʽ */
        
#if USART_EN_RX
        HAL_NVIC_EnableIRQ(USART_UX_IRQn);                      /* ʹ��USART1�ж�ͨ�� */
        HAL_NVIC_SetPriority(USART_UX_IRQn, 3, 3);              /* ��2��������ȼ�:��ռ���ȼ�3�������ȼ�3 */
#endif
    }
    
    
//usart2/////////////////////////////////////////////////////////        
    if (huart->Instance == USART2_UX)                            /* ����Ǵ���2�����д���2 MSP��ʼ�� */
    {
        USART2_TX_GPIO_CLK_ENABLE();                             /* ʹ�ܴ���TX��ʱ�� */
        USART2_RX_GPIO_CLK_ENABLE();                             /* ʹ�ܴ���RX��ʱ�� */
        USART2_UX_CLK_ENABLE();                                  /* ʹ�ܴ���ʱ�� */

        gpio_init_struct.Pin = USART2_TX_GPIO_PIN;               /* ���ڷ������ź� */
        gpio_init_struct.Mode = GPIO_MODE_AF_PP;                /* ����������� */
        gpio_init_struct.Pull = GPIO_PULLUP;                    /* ���� */
        gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;          /* IO�ٶ�����Ϊ���� */
        HAL_GPIO_Init(USART2_TX_GPIO_PORT, &gpio_init_struct);
                
        gpio_init_struct.Pin = USART2_RX_GPIO_PIN;               /* ����RX�� ģʽ���� */
        gpio_init_struct.Mode = GPIO_MODE_INPUT;//GPIO_MODE_AF_INPUT;    
        HAL_GPIO_Init(USART2_RX_GPIO_PORT, &gpio_init_struct);   /* ����RX�� �������ó�����ģʽ */
        
#if USART2_EN_RX
        HAL_NVIC_EnableIRQ(USART2_UX_IRQn);                      /* ʹ��USART2�ж�ͨ�� */
        HAL_NVIC_SetPriority(USART2_UX_IRQn, 2, 3);              /* ��2��������ȼ�:��ռ���ȼ�3�������ȼ�3 */
#endif
    }
    
    
    
//usart3//////////////////////////////////////////////////////////        
    if (huart->Instance == USART3_UX)                            /* ����Ǵ���2�����д���2 MSP��ʼ�� */
    {
        USART3_TX_GPIO_CLK_ENABLE();                             /* ʹ�ܴ���TX��ʱ�� */
        USART3_RX_GPIO_CLK_ENABLE();                             /* ʹ�ܴ���RX��ʱ�� */
        USART3_UX_CLK_ENABLE();                                  /* ʹ�ܴ���ʱ�� */

        gpio_init_struct.Pin = USART3_TX_GPIO_PIN;               /* ���ڷ������ź� */
        gpio_init_struct.Mode = GPIO_MODE_AF_PP;                /* ����������� */
        gpio_init_struct.Pull = GPIO_PULLUP;                    /* ���� */
        gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;          /* IO�ٶ�����Ϊ���� */
        HAL_GPIO_Init(USART3_TX_GPIO_PORT, &gpio_init_struct);
                
        gpio_init_struct.Pin = USART3_RX_GPIO_PIN;               /* ����RX�� ģʽ���� */
        gpio_init_struct.Mode = GPIO_MODE_INPUT;//GPIO_MODE_AF_INPUT;    
        HAL_GPIO_Init(USART3_RX_GPIO_PORT, &gpio_init_struct);   /* ����RX�� �������ó�����ģʽ */
        
#if USART3_EN_RX
        HAL_NVIC_EnableIRQ(USART3_UX_IRQn);                      /* ʹ��USART2�ж�ͨ�� */
        HAL_NVIC_SetPriority(USART3_UX_IRQn, 2, 2);              /* ��2��������ȼ�:��ռ���ȼ�3�������ȼ�3 */
#endif
    }
    
    
}


void sendDecimalToHex(char *decimalString) {
    int num1, num2, num3;

    // ʹ�� sscanf ���ַ�������Ϊ����
    sscanf(decimalString, "%d %d %d", &num1, &num2, &num3);

    // ʹ�� sprintf ��������ʽ��Ϊʮ�������ַ���
    uint8_t hexString[3]={num1,num2,num3};  // ����ʵ��������������С

    // ʹ�� HAL_UART_Transmit ��������
    HAL_UART_Transmit(&g_uart3_handle, (uint8_t *)hexString, 3, HAL_MAX_DELAY);
    //HAL_UART_Transmit(&g_uart3_handle, (uint8_t *)hexString, 3, HAL_MAX_DELAY);

}



void SendFormattedData(double value) {
    // �� double ��ֳ��������ֺ�С������
    int integralPart = (int)value;
       // ���㸡������ȥ�������ֺ��С������
    float decimalPart = value - integralPart;

    // ��С�����ֱ�����λС��
    decimalPart = (float)((int)(decimalPart * 1000)) / 1000.0f;

    // ת��Ϊ����
    int fractionalPart = (int)(decimalPart * 1000);
    // ������������
    uint8_t dataBufferInt[5] = {0x03, (uint8_t)integralPart ,(uint8_t)(integralPart >> 24),(uint8_t)(integralPart >> 16),(uint8_t)(integralPart >> 8)};
    HAL_UART_Transmit(&g_uart3_handle, dataBufferInt, sizeof(dataBufferInt), HAL_MAX_DELAY);
    //HAL_UART_Transmit(&g_uart3_handle, dataBufferInt, sizeof(dataBufferInt), HAL_MAX_DELAY);
    //delay_ms(100);
    // ����С������
    uint8_t dataBufferFrac[5] = {0x05,(uint8_t)(fractionalPart%256),(uint8_t)(fractionalPart/256), 0,0};
    HAL_UART_Transmit(&g_uart3_handle, dataBufferFrac, sizeof(dataBufferFrac), HAL_MAX_DELAY);
    //HAL_UART_Transmit(&g_uart3_handle, dataBufferFrac, sizeof(dataBufferFrac), HAL_MAX_DELAY);
}
