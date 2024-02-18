#include "HC05.h"
#include "./SYSTEM/sys/sys.h"
#include "string.h"
#include "./SYSTEM/usart/usart.h"
#include "OLED.h"
#include "./SYSTEM/delay/delay.h"
void HC05_Init(void) {
    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Pin = GPIO_PIN_0;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

void HC05_EnterAT(void) {
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET);
}

void HC05_ExitAT(void) {
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET);
}

void HC05_SendString(char *Send) {
    //strcpy(IntermediateBuffer, Send);
    HAL_UART_Transmit(&g_uart2_handle, (uint8_t *)Send, strlen(Send), 1000);
    while(__HAL_UART_GET_FLAG(&g_uart2_handle,UART_FLAG_TC) != SET){};           /* 等待发送结束 */     
    //printf("blue_sendtest");
}


void HC05_show(void)
{
        if (g_usart_rx_sta2 & 0x8000)        /* 蓝牙接收到了数据? */
        {
            while(__HAL_UART_GET_FLAG(&g_uart2_handle,UART_FLAG_TC) != SET);           /* 等待发送结束 */         
            OLED_ShowString(15,2,g_usart_rx_buf2,OLED_6X8);     
            g_usart_rx_sta2 = 0;
        }
        if (g_usart_rx_sta & 0x8000)        /* 蓝牙接收到了数据? */
        {
            while(__HAL_UART_GET_FLAG(&g_uart1_handle,UART_FLAG_TC) != SET);           /* 等待发送结束 */         
            //OLED_ShowString(15,2,g_usart_rx_buf2,OLED_6X8);     
            g_usart_rx_sta = 0;
        }
}

