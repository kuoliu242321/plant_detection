#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/usart/usart.h"
#include "./SYSTEM/delay/delay.h"
#include "freertos_demo.h"
#include <string.h>
#include "OLED.h"
#include "BMP280.h"
#include "math.h"
#include "HC05.h"
#include "light.h"
#include "gtim.h"
#include "adc.h"
#include "DHT11.h"

int main(void)
{
    HAL_Init();                         /* ��ʼ��HAL�� */
    sys_stm32_clock_init(RCC_PLL_MUL9); /* ����ʱ��, 72Mhz */
    delay_init(72);                     /* ��ʱ��ʼ�� */
    usart_init(9600,115200,9600);       /* ���ڳ�ʼ��Ϊ115200 */
    led_init();                         /* ��ʼ��LED */
    adc_init();
    OLED_Init();
    DHT11_GPIO_Config(); //DHT11��ʼ��   

    freertos_demo();                    /* ����FreeRTOS���� */
}
