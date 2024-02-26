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
    HAL_Init();                         /* 初始化HAL库 */
    sys_stm32_clock_init(RCC_PLL_MUL9); /* 设置时钟, 72Mhz */
    delay_init(72);                     /* 延时初始化 */
    usart_init(9600,115200,9600);       /* 串口初始化为115200 */
    led_init();                         /* 初始化LED */
    adc_init();
    OLED_Init();
    DHT11_GPIO_Config(); //DHT11初始化   

    freertos_demo();                    /* 运行FreeRTOS例程 */
}
