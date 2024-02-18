#include "gtim.h"
#include "./SYSTEM/sys/sys.h"

    TIM_HandleTypeDef g_tim_timx_pwm_chy_handle;
void gtim_timx_pwm_chy_init(uint16_t arr,uint16_t psc)
{   


    g_tim_timx_pwm_chy_handle.Instance = TIM2;
    g_tim_timx_pwm_chy_handle.Init.Prescaler = psc;
    g_tim_timx_pwm_chy_handle.Init.Period = arr;
    g_tim_timx_pwm_chy_handle.Init.CounterMode = TIM_COUNTERMODE_UP;
    //g_tim_timx_pwm_chy_handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV4 ;
    HAL_TIM_PWM_Init(&g_tim_timx_pwm_chy_handle);

    
    TIM_OC_InitTypeDef timx_oc_pwn_chy;    
    timx_oc_pwn_chy.OCMode = TIM_OCMODE_PWM1;
    timx_oc_pwn_chy.Pulse = arr/2;
    timx_oc_pwn_chy.OCPolarity = TIM_OCPOLARITY_LOW;

    HAL_TIM_PWM_ConfigChannel(&g_tim_timx_pwm_chy_handle,&timx_oc_pwn_chy,TIM_CHANNEL_1);
    HAL_TIM_PWM_ConfigChannel(&g_tim_timx_pwm_chy_handle,&timx_oc_pwn_chy,TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&g_tim_timx_pwm_chy_handle,TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&g_tim_timx_pwm_chy_handle,TIM_CHANNEL_2);
}

void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim2)
{
    if(htim2->Instance == TIM2)
    {
        __HAL_RCC_GPIOA_CLK_ENABLE();
        __HAL_RCC_TIM2_CLK_ENABLE();

        GPIO_InitTypeDef gpio_init_struct;        
        gpio_init_struct.Pin = GPIO_PIN_0;
        gpio_init_struct.Mode = GPIO_MODE_AF_PP;
        gpio_init_struct.Pull = GPIO_PULLUP;
        gpio_init_struct.Speed = GPIO_SPEED_FREQ_LOW;
        HAL_GPIO_Init(GPIOA,&gpio_init_struct);

//        gpio_init_struct.Pin = GPIO_PIN_1;
//        HAL_GPIO_Init(GPIOA,&gpio_init_struct);


    }
}
