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
    // ���� PC13 Ϊ���ģʽ
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
//    // ���������е�ÿ��Ԫ��
//    for (size_t i = 0; i < size; i++) {
//        // ��鵱ǰԪ�ص�ֵ���������ִ�в���
//        if (buffer[i] == 0x31) {
//            // ִ�� LED ���Ĳ���
//            led_on();
//        } else if (buffer[i] == 0x32) {
//            // ִ�� LED ��Ĳ���
//            led_off();
//        }
//    }
//}
//void led_conctrl(char *buffer,char *buffer2) {
//    // ����ַ����Ƿ�Ϊ��1����2��
//    if ((strcmp(buffer, "1") == 0)|(strcmp(buffer2, "10") == 0)) {
//        // ִ�� LED ���Ĳ���
//        led_on();
//    } else if ((strcmp(buffer, "2") == 0)|(strcmp(buffer2, "11") == 0)) {
//        // ִ�� LED ��Ĳ���
//        led_off();
//    }
//}
//void led_conctrl(char *buffer1, char *buffer2) {
//    // ����һ���ַ����Ƿ�Ϊ��1����ڶ����ַ����Ƿ�Ϊ��10��
//    if ((strcmp(buffer1, "1") == 0) || (strcmp(buffer2, "10") == 0)) {
//        // ִ�� LED ���Ĳ���
//        led_on();
//    } else {
//        // ����һ���ַ����Ƿ�Ϊ��2����ڶ����ַ����Ƿ�Ϊ��11��
//        if ((strcmp(buffer1, "2") == 0) || (strcmp(buffer2, "11") == 0)) {
//            // ִ�� LED ��Ĳ���
//            led_off();
//        }
//    }
//}
// ȥ���ַ����еĻس����з������ؽضϺ���ַ���

void led_conctrl(char *buffer1, char *buffer2) {
    // ȥ���ַ����еĻس����з�
    // ��������ַ����Ƿ�ͬʱΪ��1����ͬʱΪ��0��
    if (((strcmp(buffer1, "1") == 0) && (strcmp(buffer2, "1") == 0)) ||
        ((strcmp(buffer1, "2") == 0) && (strcmp(buffer2, "2") == 0))) {
        // ִ�� LED ���Ĳ���
        led_on();
    } else {
        // ִ�� LED ��Ĳ���
        led_off();
    }
}
