#include "./SYSTEM/delay/delay.h"
#include "light.h"
#include "string.h"
#include "./SYSTEM/usart/usart.h"
#include "OLED.h"
void led_off(void) {
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
}

void led_on(void) {
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);
}


void led_init(void){
    // 配置 PC13 为输出模式
    __HAL_RCC_GPIOC_CLK_ENABLE();
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = GPIO_PIN_13;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);    
}

//void led_conctrl(uint8_t *buffer, size_t size) {
//    // 遍历数组中的每个元素
//    for (size_t i = 0; i < size; i++) {
//        // 检查当前元素的值，根据情况执行操作
//        if (buffer[i] == 0x31) {
//            // 执行 LED 亮的操作
//            led_on();
//        } else if (buffer[i] == 0x32) {
//            // 执行 LED 灭的操作
//            led_off();
//        }
//    }
//}
//void led_conctrl(char *buffer,char *buffer2) {
//    // 检查字符串是否为“1”或“2”
//    if ((strcmp(buffer, "1") == 0)|(strcmp(buffer2, "10") == 0)) {
//        // 执行 LED 亮的操作
//        led_on();
//    } else if ((strcmp(buffer, "2") == 0)|(strcmp(buffer2, "11") == 0)) {
//        // 执行 LED 灭的操作
//        led_off();
//    }
//}
//void led_conctrl(char *buffer1, char *buffer2) {
//    // 检查第一个字符串是否为“1”或第二个字符串是否为“10”
//    if ((strcmp(buffer1, "1") == 0) || (strcmp(buffer2, "10") == 0)) {
//        // 执行 LED 亮的操作
//        led_on();
//    } else {
//        // 检查第一个字符串是否为“2”或第二个字符串是否为“11”
//        if ((strcmp(buffer1, "2") == 0) || (strcmp(buffer2, "11") == 0)) {
//            // 执行 LED 灭的操作
//            led_off();
//        }
//    }
//}
// 去除字符串中的回车换行符并返回截断后的字符串

void led_conctrl(char *buffer1, char *buffer2) {
    // 去除字符串中的回车换行符
    // 检查两个字符串是否同时为“1”或同时为“0”
    if (((strcmp(buffer1, "1") == 0) && (strcmp(buffer2, "1") == 0)) ||
        ((strcmp(buffer1, "2") == 0) && (strcmp(buffer2, "2") == 0))) {
        // 执行 LED 亮的操作
        led_on();
    } else {
        // 执行 LED 灭的操作
        led_off();
    }
}
