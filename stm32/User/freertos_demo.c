#include "./SYSTEM/usart/usart.h"
#include "./SYSTEM/delay/delay.h"
#include "freertos_demo.h"
#include <string.h>
#include "OLED.h"
#include "BMP280.h"
#include "math.h"
#include "HC05.h"
#include "light.h"
#include "adc.h"
#include "DHT11.h"
/*FreeRTOS*********************************************************************************************/
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"
/******************************************************************************************************/
/*FreeRTOS配置*/

/* START_TASK 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define START_TASK_PRIO 1                   /* 任务优先级 */
#define START_STK_SIZE  128                 /* 任务堆栈大小 */
TaskHandle_t            StartTask_Handler;  /* 任务句柄 */
void start_task(void *pvParameters);        /* 任务函数 */

/* TASK1 任务 配置/////////////////////////////////////////////////////////////////////DHT11温湿度采集
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define TASK1_PRIO      3                   /* 任务优先级 */
#define TASK1_STK_SIZE  128                 /* 任务堆栈大小 */
TaskHandle_t            Task1Task_Handler;  /* 任务句柄 */
void task1(void *pvParameters);             /* 任务函数 */

/* TASK2 任务 配置/////////////////////////////////////////////////////////////////////adc数据采集
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define TASK2_PRIO      3                   /* 任务优先级 */
#define TASK2_STK_SIZE  256                 /* 任务堆栈大小 */
TaskHandle_t            Task2Task_Handler;  /* 任务句柄 */
void task2(void *pvParameters);             /* 任务函数 */

/* TASK3 任务 配置/////////////////////////////////////////////////////////////////////bmp280数据采集
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define TASK3_PRIO      3                   /* 任务优先级 */
#define TASK3_STK_SIZE  128                 /* 任务堆栈大小 */
TaskHandle_t            Task3Task_Handler;  /* 任务句柄 */
void task3(void *pvParameters);             /* 任务函数 */

/* TASK4 任务 配置/////////////////////////////////////////////////////////////////////蓝牙信息发送+指令接受
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define TASK4_PRIO      5                   /* 任务优先级 */
#define TASK4_STK_SIZE  128                 /* 任务堆栈大小 */
TaskHandle_t            Task4Task_Handler;  /* 任务句柄 */
void task4(void *pvParameters);             /* 任务函数 */

/* TASK5 任务 配置/////////////////////////////////////////////////////////////////////CI-03T数据采集发送+指令接收
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define TASK5_PRIO      5                   /* 任务优先级 */
#define TASK5_TK_SIZE  128                 /* 任务堆栈大小 */
TaskHandle_t            Task5Task_Handler;  /* 任务句柄 */
void task5(void *pvParameters);             /* 任务函数 */

/* TASK6 任务 配置/////////////////////////////////////////////////////////////////////esp8266数据接收发送+指令接收
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define TASK6_PRIO      2                   /* 任务优先级 */
#define TASK6_STK_SIZE  128                 /* 任务堆栈大小 */
TaskHandle_t            Task6Task_Handler;  /* 任务句柄 */
void task6(void *pvParameters);             /* 任务函数 */

/* TASK7 任务 配置/////////////////////////////////////////////////////////////////////oled屏显示
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define TASK7_PRIO      5                   /* 任务优先级 */
#define TASK7_STK_SIZE  128                 /* 任务堆栈大小 */
TaskHandle_t            Task7Task_Handler;  /* 任务句柄 */
void task7(void *pvParameters);             /* 任务函数 */ 

/* TASK8 任务 配置/////////////////////////////////////////////////////////////////////灯控
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define TASK8_PRIO      5                   /* 任务优先级 */
#define TASK8_STK_SIZE  128                 /* 任务堆栈大小 */
TaskHandle_t            Task8Task_Handler;  /* 任务句柄 */
void task8(void *pvParameters);             /* 任务函数 */ 
/******************************************************************************************************/

////***************************中断数据提供
//// 定义队列的长度
//#define QUEUE_LENGTH_IRQcmd    1
//// 定义队列句柄
//QueueHandle_t xQueueIRQcmd;
//***************************中断数据提供

//***************************DHT11数据提供
// 定义队列的长度
#define QUEUE_LENGTH_dht11    1
// 定义队列句柄
QueueHandle_t xQueueDHT11Data;
//***************************DHT11数据提供

//***************************soil数据提供
// 定义队列的长度
#define QUEUE_LENGTH_soil    1
// 定义队列句柄
QueueHandle_t xQueuesoil_Data;
//***************************soil数据提供

//***************************bmp280数据提供
// 定义队列的长度
#define QUEUE_LENGTH_bmp280    1
// 定义队列句柄
QueueHandle_t xQueuebmp280_Data;
//***************************bmp280数据提供

SemaphoreHandle_t xSemaphore = NULL;// 创建一个计数信号量，最大计数值为10，初始计数值为0

EventGroupHandle_t myEventGroup;//事件组
/**
 * @brief       FreeRTOS例程入口函数
 * @param       无
 * @retval      无
 */
void freertos_demo(void)
{

    myEventGroup = xEventGroupCreate();
    
//    xQueueIRQcmd = xQueueCreate(QUEUE_LENGTH_IRQcmd, sizeof(char));
    xQueuebmp280_Data = xQueueCreate(QUEUE_LENGTH_bmp280, sizeof(bmp280_Data));
    xQueueDHT11Data = xQueueCreate(QUEUE_LENGTH_dht11, sizeof(DHT11_Data_TypeDef));
    xQueuesoil_Data = xQueueCreate(QUEUE_LENGTH_soil, sizeof(soil_detect_Data));
    
    if(( xQueueDHT11Data != NULL )&(xQueuesoil_Data != NULL)&(xQueuebmp280_Data != NULL))
	{
    xTaskCreate((TaskFunction_t )start_task,            /* 任务函数 */
                (const char*    )"start_task",          /* 任务名称 */
                (uint16_t       )START_STK_SIZE,        /* 任务堆栈大小 */
                (void*          )NULL,                  /* 传入给任务函数的参数 */
                (UBaseType_t    )START_TASK_PRIO,       /* 任务优先级 */
                (TaskHandle_t*  )&StartTask_Handler);   /* 任务句柄 */
    vTaskStartScheduler();
    }

}

/**
 * @brief       start_task
 * @param       pvParameters : 传入参数(未用到)
 * @retval      无
 */
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();           /* 进入临界区 */
    /* 创建任务1 */
    xTaskCreate((TaskFunction_t )task1,
                (const char*    )"task1",
                (uint16_t       )TASK1_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )TASK1_PRIO,
                (TaskHandle_t*  )&Task1Task_Handler);
    /* 创建任务2 */
    xTaskCreate((TaskFunction_t )task2,
                (const char*    )"task2",
                (uint16_t       )TASK2_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )TASK2_PRIO,
                (TaskHandle_t*  )&Task2Task_Handler);
    /* 创建任务3 */
    xTaskCreate((TaskFunction_t )task3,
                (const char*    )"task3",
                (uint16_t       )TASK3_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )TASK3_PRIO,
                (TaskHandle_t*  )&Task3Task_Handler);
    /* 创建任务4 */
    xTaskCreate((TaskFunction_t )task4,
                (const char*    )"task4",
                (uint16_t       )TASK4_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )TASK4_PRIO,
                (TaskHandle_t*  )&Task4Task_Handler);
    /* 创建任务5 */                
    xTaskCreate((TaskFunction_t )task5,
                (const char*    )"task5",
                (uint16_t       )TASK5_TK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )TASK5_PRIO,
                (TaskHandle_t*  )&Task5Task_Handler);
    /* 创建任务6 */
    xTaskCreate((TaskFunction_t )task6,
                (const char*    )"task6",
                (uint16_t       )TASK6_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )TASK6_PRIO,
                (TaskHandle_t*  )&Task6Task_Handler);
    /* 创建任务7 */
    xTaskCreate((TaskFunction_t )task7,
                (const char*    )"task7",
                (uint16_t       )TASK7_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )TASK7_PRIO,
                (TaskHandle_t*  )&Task7Task_Handler);
    /* 创建任务8 */
    xTaskCreate((TaskFunction_t )task8,
                (const char*    )"task8",
                (uint16_t       )TASK8_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )TASK8_PRIO,
                (TaskHandle_t*  )&Task8Task_Handler);
    vTaskDelete(StartTask_Handler); /* 删除开始任务 */
    taskEXIT_CRITICAL();            /* 退出临界区 */
}

/**
 * @brief       task1***************************DHT11温湿度采集
 * @param       pvParameters : 传入参数(未用到)
 * @retval      无
 */
void task1(void *pvParameters)
{
DHT11_Data_TypeDef DHT11_Data;//温湿度
BaseType_t xStatus; 
    
TickType_t xLastWakeTime;
const TickType_t xFrequency = pdMS_TO_TICKS(1000); // 
// 初始化上次唤醒时间
xLastWakeTime = xTaskGetTickCount();    
    while(1)
    {
         if(Read_DHT11(&DHT11_Data) == SUCCESS)
            {
                xStatus = xQueueOverwrite( xQueueDHT11Data, &DHT11_Data);
                //printf("this okey!%d %d %d %d\r\n",DHT11_Data.humi_deci,DHT11_Data.humi_int,DHT11_Data.temp_deci,DHT11_Data.temp_int);
                vTaskDelayUntil(&xLastWakeTime, xFrequency);
                    if( xStatus != pdPASS  )
                    {
                        printf( "Flase from tem&humi.\r\n" );
                        vTaskDelay(pdMS_TO_TICKS(2000)); // 1秒
                    }
            }
    }
}

/**
 * @brief       task2***************************ADC土壤湿度采集
 * @param       pvParameters : 传入参数(未用到)
 * @retval      无
 */
void task2(void *pvParameters)
{
#define ADC_DMA_BUF_SIZE        100         /* ADC DMA采集 BUF大小 */
extern uint8_t g_adc_dma_sta;               /* DMA传输状态标志, 0,未完成; 1, 已完成 */
uint16_t g_adc_dma_buf[ADC_DMA_BUF_SIZE];   /* ADC DMA BUF */

uint16_t i;
uint16_t adcx;
uint32_t sum;
float temp;   
    
adc_dma_init((uint32_t)&g_adc_dma_buf); /* 初始化ADC DMA采集 */    
adc_dma_enable(ADC_DMA_BUF_SIZE);   /* 启动ADC DMA采集 */  

BaseType_t xStatus;   
soil_detect_Data soil_data;    

TickType_t xLastWakeTime;
const TickType_t xFrequency = pdMS_TO_TICKS(1000); // 1秒
xLastWakeTime = xTaskGetTickCount();// 初始化上次唤醒时间

    while(1)
    {   
       
        if (g_adc_dma_sta == 1)
        {
            /* 计算DMA 采集到的ADC数据的平均值 */
            sum = 0;

            for (i = 0; i < ADC_DMA_BUF_SIZE; i++)   /* 累加 */
            {
                sum += g_adc_dma_buf[i];
            }

            adcx = sum / ADC_DMA_BUF_SIZE;           /* 取平均值 */
            //printf("%d ",adcx);//************************
            soil_data.original_adc = adcx;
            temp = (float)adcx * (3.3 / 4096);                  /* 获取计算后的带小数的实际电压值，比如3.1111 */
            adcx = temp;                                        /* 赋值整数部分给adcx变量，因为adcx为u16整形 */
            //printf("%d ",adcx);//************************
            soil_data.adc_integer =adcx;
            temp -= adcx;                                       /* 把已经显示的整数部分去掉，留下小数部分，比如3.1111-3=0.1111 */
            temp *= 1000;                                       /* 小数部分乘以1000，例如：0.1111就转换为111.1，相当于保留三位小数。 */
            //printf("%f ",temp);//************************
            soil_data.temp_decimal = temp;
            xStatus = xQueueOverwrite( xQueuesoil_Data, &soil_data); /* 只需要写入地址, 无需写入整个struct */
            
            //printf("%d %d %d\r\n",soil_data.adc_integer,soil_data.original_adc,soil_data.temp_decimal);
            
                    if( xStatus != pdPASS )
                    {
                        //printf( "Flase from ADC detection.\r\n" );
                        vTaskDelay(pdMS_TO_TICKS(2000)); // 1秒
                    }
            g_adc_dma_sta = 0;                                  /* 清除DMA采集完成状态标志 */
            adc_dma_enable(ADC_DMA_BUF_SIZE);                   /* 启动下一次ADC DMA采集 */
            vTaskDelayUntil(&xLastWakeTime, xFrequency);
        }
    }
}


/**
 * @brief       task3***************************BMP280温湿度采集
 * @param       pvParameters : 传入参数(未用到)
 * @retval      无
 */
void task3(void *pvParameters)
{
BaseType_t xStatus_bmp280;
bmp280_Data soil_data;
uint8_t ID;
Bmp_Init();
ID=BMP280_ReadID();          //获得ID号
TickType_t xLastWakeTime;
const TickType_t xFrequency = pdMS_TO_TICKS(1000); // 

// 初始化上次唤醒时间
xLastWakeTime = xTaskGetTickCount();
    
    while(1)
    {
        soil_data.pressure = BMP280_Get_Pressure()/1000;
        soil_data.tempture = BMP280_Get_Temperature();
        soil_data.id = ID;  
        xStatus_bmp280 = xQueueOverwrite( xQueuebmp280_Data, &soil_data);
        if( xStatus_bmp280 == pdPASS )
            {
            //printf("%d %f %f\r\n",soil_data.id,soil_data.pressure,soil_data.tempture);
             vTaskDelayUntil(&xLastWakeTime, xFrequency);
            }
        else{
             //printf( "Flase from bmp280.\r\n" );
             vTaskDelay(pdMS_TO_TICKS(2000)); // 2秒
            }
    }
}

/**
 * @brief       task4***************************蓝牙数据发送指令接收
 * @param       pvParameters : 传入参数(未用到)
 * @retval      无
 */
void task4(void *pvParameters)
{
    DHT11_Data_TypeDef received_data; /* 读取队列时, 用这个变量来存放数据 */
    bmp280_Data receive_bmp280;//大气压
    
    const TickType_t xTicksToWait = pdMS_TO_TICKS( 1000 );	
    TickType_t xLastWakeTime;
    xLastWakeTime = xTaskGetTickCount();    // 初始化上次唤醒时间
    

BaseType_t xStatus_bmp280;
BaseType_t xStatus_dht11;
    
    char tem[] = "Temp:";
    char hum[] = "Humity:";
    char presure[] = "pre:";
    char back[] = " ";
    char point[] = ".";    
    while(1)
    {
       

        /* 读队列*/
        xStatus_dht11 = xQueuePeek( xQueueDHT11Data, &received_data, xTicksToWait); /* 得到buffer地址，只是4字节 */
        xStatus_bmp280 = xQueuePeek( xQueuebmp280_Data, &receive_bmp280, xTicksToWait);
        if(( xStatus_dht11 ==  pdTRUE )&(xStatus_bmp280 == pdTRUE))
                {
                EventBits_t bits = xEventGroupSetBits(myEventGroup, 0x01);
                
                char temptuare_int[3];  // 这里假设uint8_t的范围是0-255，所以最多占用3个字符
                sprintf(temptuare_int, "%u", received_data.temp_int);
               
                char temptuare_dec[2];  // 这里假设uint8_t的范围是0-255，所以最多占用3个字符
                sprintf(temptuare_dec, "%u", received_data.temp_deci);        
              
                char humity_int[3];  // 这里假设uint8_t的范围是0-255，所以最多占用3个字符
                sprintf(humity_int, "%u", received_data.humi_int);        
                
                char humity_dec[3];  // 这里假设uint8_t的范围是0-255，所以最多占用3个字符
                sprintf(humity_dec, "%u", received_data.humi_deci);  
                    
                char int_pre[4]; // 存储整数部分的字符串，包括终止符 '\0'
                char dec_pre[4]; // 存储小数部分的字符串，包括终止符 '\0'
                // 将整数部分转换为字符串
                sprintf(int_pre, "%03d", (int)receive_bmp280.pressure);
                // 将小数部分转换为字符串，保留三位小数
                sprintf(dec_pre, "%03d", (int)((receive_bmp280.pressure - (int)receive_bmp280.pressure) * 1000));
                    
                //****************
                //字符串拼接
                size_t total = strlen(tem) + strlen(temptuare_int)+ strlen(point)+ strlen(temptuare_dec)+ strlen(back)+ strlen(hum)+ strlen(humity_int)+ strlen(point) + strlen(humity_dec) + strlen(back)+ strlen(presure)+ strlen(int_pre)+ strlen(point)+ strlen(dec_pre)+ 1; // 加1是为了容纳字符串末尾的'\0'
                char result[total];
                sprintf(result, "%s%s%s%s%s%s%s%s%s%s%s%s%s%s", tem, temptuare_int, point, temptuare_dec,back, hum, humity_int, point, humity_dec,back,presure,int_pre,point,dec_pre);
                    
                HC05_SendString(result);

                vTaskDelayUntil(&xLastWakeTime, xTicksToWait);
                    
                }
        else
        {
            //printf( "Flase(from bluetooth).\r\n" );
            vTaskDelay(pdMS_TO_TICKS(2000)); // 2秒
        }
    }
}

/**
 * @brief       task5***************************CI-03T数据发送指令采集
 * @param       pvParameters : 传入参数(未用到)
 * @retval      无
 */
void task5(void *pvParameters)
{
DHT11_Data_TypeDef received_data;//温湿度
bmp280_Data receive_bmp280;//大气压
const TickType_t xTicksToWait = pdMS_TO_TICKS( 1000 );	
TickType_t xLastWakeTime;
xLastWakeTime = xTaskGetTickCount();// 初始化上次唤醒时间
char back[] = " ";
char message_1[]="01";
char message_2[]="06";   
BaseType_t xStatus_bmp280;
BaseType_t xStatus_dht11;
    while(1)
    {
        EventBits_t bits = xEventGroupWaitBits(myEventGroup, 0x01, pdTRUE, pdTRUE, 0);
        if ((bits & 0x01) != 0) 
            {
                xStatus_bmp280 = xQueuePeek( xQueuebmp280_Data, &receive_bmp280, xTicksToWait);
                xStatus_dht11 = xQueuePeek( xQueueDHT11Data, &received_data, xTicksToWait);
                if(( xStatus_bmp280 ==  pdTRUE )&(xStatus_dht11 == pdTRUE))
                    //if( xStatus_bmp280 ==  pdTRUE )
                      {
                        //********************************************************************************************************        
                                char temptuare_int[3];  // 这里假设uint8_t的范围是0-255，所以最多占用3个字符
                                sprintf(temptuare_int, "%u", received_data.temp_int);
                               
                                char temptuare_dec[2];  // 这里假设uint8_t的范围是0-255，所以最多占用3个字符
                                sprintf(temptuare_dec, "%u", received_data.temp_deci);        
                              
                                char humity_int[3];  // 这里假设uint8_t的范围是0-255，所以最多占用3个字符
                                sprintf(humity_int, "%u", received_data.humi_int);        
                                
                                char humity_dec[3];  // 这里假设uint8_t的范围是0-255，所以最多占用3个字符
                                sprintf(humity_dec, "%u", received_data.humi_deci);        
                                size_t uart1_len=strlen(message_1)+strlen(back)+strlen(temptuare_int)+strlen(back)+strlen(temptuare_dec)+1;
                                char datatem_hexsend[uart1_len];
                                sprintf(datatem_hexsend, "%s%s%s%s%s", message_1, back, temptuare_int, back,temptuare_dec);
                                char datahumi_hexsend[uart1_len];
                                sprintf(datahumi_hexsend, "%s%s%s%s%s", message_2, back, humity_int, back,humity_dec);   
                        //********************************************************************************************************        
                                sendDecimalToHex(datatem_hexsend);//温度
                                SendFormattedData(receive_bmp280.pressure);//大气压
                                sendDecimalToHex(datahumi_hexsend);  //湿度
//                                printf("%f,%d\r\n",receive_bmp280.pressure,receive_bmp280.id);
                                vTaskDelayUntil(&xLastWakeTime, xTicksToWait);

                        }        
            EventBits_t bits = xEventGroupSetBits(myEventGroup, 0x02);
            }
            else
            {
                /* 没读到数据 */
                //printf( "Flase(ferom ci-03t).\r\n" );
            vTaskDelay(pdMS_TO_TICKS(2000)); // 2秒
            }
    }
}

/**
 * @brief       task6***************************esp8266数据发送指令接收
 * @param       pvParameters : 传入参数(未用到)
 * @retval      无
 */
void task6(void *pvParameters)
{
//#define TASK_LIST_BUFFER_SIZE 512
//static char taskListBuffer[TASK_LIST_BUFFER_SIZE];   
DHT11_Data_TypeDef received_data;//温湿度
bmp280_Data receive_bmp280;//大气压
soil_detect_Data soil_data;// 土壤湿度

const TickType_t xTicksToWait = pdMS_TO_TICKS( 3500 );	
TickType_t xLastWakeTime;
xLastWakeTime = xTaskGetTickCount();  // 初始化上次唤醒时间  
    while(1)
    {

//vTaskList(taskListBuffer);// 生成任务列表文本并存储到缓冲区中
//printf("Task List:\n%s\n", taskListBuffer);// 输出任务列表文本到控制台或其他输出设备
        
        //EventBits_t bits = xEventGroupWaitBits(myEventGroup, 0x04, pdTRUE, pdTRUE, 0);
        //if ((bits & 0x04) != 0) 
            {        
                if((xQueuePeek( xQueuebmp280_Data, &receive_bmp280, xTicksToWait) == pdTRUE)|(xQueuePeek( xQueueDHT11Data, &received_data, xTicksToWait)== pdTRUE)|(xQueuePeek( xQueuesoil_Data, &soil_data, xTicksToWait)== pdTRUE))
                {
                        float humidity = received_data.humi_int + received_data.humi_deci / 100.0f;
                        float temperature = received_data.temp_int + received_data.temp_deci / 10.0f;
                        float result = (float)soil_data.original_adc / 4096.0f;
                        float soilhumitidy = roundf(100*result * 10.0f) / 10.0f; // 保留一位小数
                        char buffer[50]; // 定义一个缓冲区用于格式化数据
                        // 格式化数据到缓冲区
                        sprintf(buffer, "%.1f %6.f %.2f %.1f\n", soilhumitidy, receive_bmp280.pressure * 1000, humidity, temperature);
                        // 获取缓冲区长度
                        size_t len = strlen(buffer);
                        // 使用HAL库的UART发送函数发送数据
                        HAL_UART_Transmit(&g_uart1_handle, (uint8_t *)buffer, len, HAL_MAX_DELAY);
//                        taskENTER_CRITICAL();
//                        printf("%.1f %6.f %.2f %.1f\n",soilhumitidy,receive_bmp280.pressure*1000,humidity,temperature);
//                        taskEXIT_CRITICAL();
                }
            }
                vTaskDelayUntil(&xLastWakeTime, xTicksToWait);
    }
}

/**
 * @brief       task7***************************OLED屏显示
 * @param       pvParameters : 传入参数(未用到)
 * @retval      无
 */
void task7(void *pvParameters)
{

DHT11_Data_TypeDef received_data;//温湿度
bmp280_Data receive_bmp280;//大气压
soil_detect_Data soil_data;// 土壤湿度

const TickType_t xTicksToWait = pdMS_TO_TICKS( 100UL );	
TickType_t xLastWakeTime;
xLastWakeTime = xTaskGetTickCount();  // 初始化上次唤醒时间  
    
    while(1)
    {
        EventBits_t bits = xEventGroupWaitBits(myEventGroup, 0x02, pdTRUE, pdTRUE, 0);
        if ((bits & 0x02) != 0) 
            {
                if((xQueuePeek( xQueuebmp280_Data, &receive_bmp280, xTicksToWait) == pdTRUE)&(xQueuePeek( xQueueDHT11Data, &received_data, xTicksToWait)== pdTRUE)&(xQueuePeek( xQueuesoil_Data, &soil_data, xTicksToWait)== pdTRUE))
                {
                    OLED_ShowString(3,20,"ID:",OLED_6X8);   
                    OLED_ShowString(3,30,"P:",OLED_6X8);   //获得大气压强
                    OLED_ShowString(3,40,"T:",OLED_6X8);   //获得温度      
                    OLED_ShowString(3, 50, "temp:",OLED_6X8);
                    OLED_ShowString(60, 2, "humi:",OLED_6X8);   

                    OLED_ShowNum(90, 2, received_data.humi_int, 2,OLED_6X8);
                    OLED_ShowString(102, 2, ".",OLED_6X8);
                    OLED_ShowNum(108, 2, received_data.humi_deci, 2,OLED_6X8);
                    
                    OLED_ShowNum(33, 50, received_data.temp_int, 2,OLED_6X8);
                    OLED_ShowString(45, 50, ".",OLED_6X8);
                    OLED_ShowNum(51, 50, received_data.temp_deci, 1,OLED_6X8);  
                    
                    OLED_ShowHexNum(20, 20, receive_bmp280.id, 4,OLED_6X8);          
                    OLED_ShowFloatNum(20,30,receive_bmp280.pressure,3,3,OLED_6X8);
                    OLED_ShowNum(20,40,receive_bmp280.tempture,6,OLED_6X8);  
                    
                    OLED_ShowNum(70, 30, soil_data.original_adc, 4,OLED_6X8);      /* 显示ADCC采样后的原始值 */        
                    OLED_ShowNum(70, 20, soil_data.adc_integer,1 ,OLED_6X8);      /* 显示电压值的整数部分，3.1111的话，这里就是显示3 */
                    OLED_ShowChar(76,20,'.',OLED_6X8);
                    OLED_ShowNum(82, 20, soil_data.temp_decimal, 3,OLED_6X8);   /* 显示小数部分（前面转换为了整形显示），这里显示的就是111. */
                    OLED_Update();
                    //********************************************************************************************************        

//                    clearString(g_usart_rx_buf, sizeof(g_usart_rx_buf));     
//                    clearString(g_usart_rx_buf2, sizeof(g_usart_rx_buf2));  
//                    clearString(g_usart_rx_buf3, sizeof(g_usart_rx_buf3)); 
                 }
             EventBits_t bits = xEventGroupSetBits(myEventGroup, 0x04);
             }
            
                vTaskDelayUntil(&xLastWakeTime, xTicksToWait);
    }
}

/**
 * @brief       task8***************************灯控
 * @param       pvParameters : 传入参数(未用到)
 * @retval      无
 */
void task8(void *pvParameters)
{
    TickType_t xLastWakeTime;
    const TickType_t xFrequency = pdMS_TO_TICKS(1000); // 1秒
    // 初始化上次唤醒时间
    xLastWakeTime = xTaskGetTickCount();  
    while(1)
    {
    HC05_show();
    //led_conctrl(g_rx_buffer, sizeof(g_rx_buffer));//on off 控制led亮灭
    //led_conctrl(g_rx_buffer2, sizeof(g_rx_buffer2));//on off 控制led亮灭
    led_conctrl((char *)g_rx_buffer,(char *)g_rx_buffer2);
//    printf("%s %s\r\n",g_rx_buffer,g_rx_buffer2);
//    clearString(g_usart_rx_buf3, sizeof(g_usart_rx_buf3)); 
//    clearString(g_usart_rx_buf, sizeof(g_usart_rx_buf));     
//    clearString(g_usart_rx_buf2, sizeof(g_usart_rx_buf2));             
    vTaskDelayUntil(&xLastWakeTime, xFrequency);        
        
    }
    
}
