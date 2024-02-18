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
/*FreeRTOS����*/

/* START_TASK ���� ����
 * ����: ������ �������ȼ� ��ջ��С ��������
 */
#define START_TASK_PRIO 1                   /* �������ȼ� */
#define START_STK_SIZE  128                 /* �����ջ��С */
TaskHandle_t            StartTask_Handler;  /* ������ */
void start_task(void *pvParameters);        /* ������ */

/* TASK1 ���� ����/////////////////////////////////////////////////////////////////////DHT11��ʪ�Ȳɼ�
 * ����: ������ �������ȼ� ��ջ��С ��������
 */
#define TASK1_PRIO      3                   /* �������ȼ� */
#define TASK1_STK_SIZE  128                 /* �����ջ��С */
TaskHandle_t            Task1Task_Handler;  /* ������ */
void task1(void *pvParameters);             /* ������ */

/* TASK2 ���� ����/////////////////////////////////////////////////////////////////////adc���ݲɼ�
 * ����: ������ �������ȼ� ��ջ��С ��������
 */
#define TASK2_PRIO      3                   /* �������ȼ� */
#define TASK2_STK_SIZE  256                 /* �����ջ��С */
TaskHandle_t            Task2Task_Handler;  /* ������ */
void task2(void *pvParameters);             /* ������ */

/* TASK3 ���� ����/////////////////////////////////////////////////////////////////////bmp280���ݲɼ�
 * ����: ������ �������ȼ� ��ջ��С ��������
 */
#define TASK3_PRIO      3                   /* �������ȼ� */
#define TASK3_STK_SIZE  128                 /* �����ջ��С */
TaskHandle_t            Task3Task_Handler;  /* ������ */
void task3(void *pvParameters);             /* ������ */

/* TASK4 ���� ����/////////////////////////////////////////////////////////////////////������Ϣ����+ָ�����
 * ����: ������ �������ȼ� ��ջ��С ��������
 */
#define TASK4_PRIO      5                   /* �������ȼ� */
#define TASK4_STK_SIZE  128                 /* �����ջ��С */
TaskHandle_t            Task4Task_Handler;  /* ������ */
void task4(void *pvParameters);             /* ������ */

/* TASK5 ���� ����/////////////////////////////////////////////////////////////////////CI-03T���ݲɼ�����+ָ�����
 * ����: ������ �������ȼ� ��ջ��С ��������
 */
#define TASK5_PRIO      5                   /* �������ȼ� */
#define TASK5_TK_SIZE  128                 /* �����ջ��С */
TaskHandle_t            Task5Task_Handler;  /* ������ */
void task5(void *pvParameters);             /* ������ */

/* TASK6 ���� ����/////////////////////////////////////////////////////////////////////esp8266���ݽ��շ���+ָ�����
 * ����: ������ �������ȼ� ��ջ��С ��������
 */
#define TASK6_PRIO      2                   /* �������ȼ� */
#define TASK6_STK_SIZE  128                 /* �����ջ��С */
TaskHandle_t            Task6Task_Handler;  /* ������ */
void task6(void *pvParameters);             /* ������ */

/* TASK7 ���� ����/////////////////////////////////////////////////////////////////////oled����ʾ
 * ����: ������ �������ȼ� ��ջ��С ��������
 */
#define TASK7_PRIO      5                   /* �������ȼ� */
#define TASK7_STK_SIZE  128                 /* �����ջ��С */
TaskHandle_t            Task7Task_Handler;  /* ������ */
void task7(void *pvParameters);             /* ������ */ 

/* TASK8 ���� ����/////////////////////////////////////////////////////////////////////�ƿ�
 * ����: ������ �������ȼ� ��ջ��С ��������
 */
#define TASK8_PRIO      5                   /* �������ȼ� */
#define TASK8_STK_SIZE  128                 /* �����ջ��С */
TaskHandle_t            Task8Task_Handler;  /* ������ */
void task8(void *pvParameters);             /* ������ */ 
/******************************************************************************************************/

////***************************�ж������ṩ
//// ������еĳ���
//#define QUEUE_LENGTH_IRQcmd    1
//// ������о��
//QueueHandle_t xQueueIRQcmd;
//***************************�ж������ṩ

//***************************DHT11�����ṩ
// ������еĳ���
#define QUEUE_LENGTH_dht11    1
// ������о��
QueueHandle_t xQueueDHT11Data;
//***************************DHT11�����ṩ

//***************************soil�����ṩ
// ������еĳ���
#define QUEUE_LENGTH_soil    1
// ������о��
QueueHandle_t xQueuesoil_Data;
//***************************soil�����ṩ

//***************************bmp280�����ṩ
// ������еĳ���
#define QUEUE_LENGTH_bmp280    1
// ������о��
QueueHandle_t xQueuebmp280_Data;
//***************************bmp280�����ṩ

SemaphoreHandle_t xSemaphore = NULL;// ����һ�������ź�����������ֵΪ10����ʼ����ֵΪ0

EventGroupHandle_t myEventGroup;//�¼���
/**
 * @brief       FreeRTOS������ں���
 * @param       ��
 * @retval      ��
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
    xTaskCreate((TaskFunction_t )start_task,            /* ������ */
                (const char*    )"start_task",          /* �������� */
                (uint16_t       )START_STK_SIZE,        /* �����ջ��С */
                (void*          )NULL,                  /* ������������Ĳ��� */
                (UBaseType_t    )START_TASK_PRIO,       /* �������ȼ� */
                (TaskHandle_t*  )&StartTask_Handler);   /* ������ */
    vTaskStartScheduler();
    }

}

/**
 * @brief       start_task
 * @param       pvParameters : �������(δ�õ�)
 * @retval      ��
 */
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();           /* �����ٽ��� */
    /* ��������1 */
    xTaskCreate((TaskFunction_t )task1,
                (const char*    )"task1",
                (uint16_t       )TASK1_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )TASK1_PRIO,
                (TaskHandle_t*  )&Task1Task_Handler);
    /* ��������2 */
    xTaskCreate((TaskFunction_t )task2,
                (const char*    )"task2",
                (uint16_t       )TASK2_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )TASK2_PRIO,
                (TaskHandle_t*  )&Task2Task_Handler);
    /* ��������3 */
    xTaskCreate((TaskFunction_t )task3,
                (const char*    )"task3",
                (uint16_t       )TASK3_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )TASK3_PRIO,
                (TaskHandle_t*  )&Task3Task_Handler);
    /* ��������4 */
    xTaskCreate((TaskFunction_t )task4,
                (const char*    )"task4",
                (uint16_t       )TASK4_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )TASK4_PRIO,
                (TaskHandle_t*  )&Task4Task_Handler);
    /* ��������5 */                
    xTaskCreate((TaskFunction_t )task5,
                (const char*    )"task5",
                (uint16_t       )TASK5_TK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )TASK5_PRIO,
                (TaskHandle_t*  )&Task5Task_Handler);
    /* ��������6 */
    xTaskCreate((TaskFunction_t )task6,
                (const char*    )"task6",
                (uint16_t       )TASK6_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )TASK6_PRIO,
                (TaskHandle_t*  )&Task6Task_Handler);
    /* ��������7 */
    xTaskCreate((TaskFunction_t )task7,
                (const char*    )"task7",
                (uint16_t       )TASK7_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )TASK7_PRIO,
                (TaskHandle_t*  )&Task7Task_Handler);
    /* ��������8 */
    xTaskCreate((TaskFunction_t )task8,
                (const char*    )"task8",
                (uint16_t       )TASK8_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )TASK8_PRIO,
                (TaskHandle_t*  )&Task8Task_Handler);
    vTaskDelete(StartTask_Handler); /* ɾ����ʼ���� */
    taskEXIT_CRITICAL();            /* �˳��ٽ��� */
}

/**
 * @brief       task1***************************DHT11��ʪ�Ȳɼ�
 * @param       pvParameters : �������(δ�õ�)
 * @retval      ��
 */
void task1(void *pvParameters)
{
DHT11_Data_TypeDef DHT11_Data;//��ʪ��
BaseType_t xStatus; 
    
TickType_t xLastWakeTime;
const TickType_t xFrequency = pdMS_TO_TICKS(1000); // 
// ��ʼ���ϴλ���ʱ��
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
                        vTaskDelay(pdMS_TO_TICKS(2000)); // 1��
                    }
            }
    }
}

/**
 * @brief       task2***************************ADC����ʪ�Ȳɼ�
 * @param       pvParameters : �������(δ�õ�)
 * @retval      ��
 */
void task2(void *pvParameters)
{
#define ADC_DMA_BUF_SIZE        100         /* ADC DMA�ɼ� BUF��С */
extern uint8_t g_adc_dma_sta;               /* DMA����״̬��־, 0,δ���; 1, ����� */
uint16_t g_adc_dma_buf[ADC_DMA_BUF_SIZE];   /* ADC DMA BUF */

uint16_t i;
uint16_t adcx;
uint32_t sum;
float temp;   
    
adc_dma_init((uint32_t)&g_adc_dma_buf); /* ��ʼ��ADC DMA�ɼ� */    
adc_dma_enable(ADC_DMA_BUF_SIZE);   /* ����ADC DMA�ɼ� */  

BaseType_t xStatus;   
soil_detect_Data soil_data;    

TickType_t xLastWakeTime;
const TickType_t xFrequency = pdMS_TO_TICKS(1000); // 1��
xLastWakeTime = xTaskGetTickCount();// ��ʼ���ϴλ���ʱ��

    while(1)
    {   
       
        if (g_adc_dma_sta == 1)
        {
            /* ����DMA �ɼ�����ADC���ݵ�ƽ��ֵ */
            sum = 0;

            for (i = 0; i < ADC_DMA_BUF_SIZE; i++)   /* �ۼ� */
            {
                sum += g_adc_dma_buf[i];
            }

            adcx = sum / ADC_DMA_BUF_SIZE;           /* ȡƽ��ֵ */
            //printf("%d ",adcx);//************************
            soil_data.original_adc = adcx;
            temp = (float)adcx * (3.3 / 4096);                  /* ��ȡ�����Ĵ�С����ʵ�ʵ�ѹֵ������3.1111 */
            adcx = temp;                                        /* ��ֵ�������ָ�adcx��������ΪadcxΪu16���� */
            //printf("%d ",adcx);//************************
            soil_data.adc_integer =adcx;
            temp -= adcx;                                       /* ���Ѿ���ʾ����������ȥ��������С�����֣�����3.1111-3=0.1111 */
            temp *= 1000;                                       /* С�����ֳ���1000�����磺0.1111��ת��Ϊ111.1���൱�ڱ�����λС���� */
            //printf("%f ",temp);//************************
            soil_data.temp_decimal = temp;
            xStatus = xQueueOverwrite( xQueuesoil_Data, &soil_data); /* ֻ��Ҫд���ַ, ����д������struct */
            
            //printf("%d %d %d\r\n",soil_data.adc_integer,soil_data.original_adc,soil_data.temp_decimal);
            
                    if( xStatus != pdPASS )
                    {
                        //printf( "Flase from ADC detection.\r\n" );
                        vTaskDelay(pdMS_TO_TICKS(2000)); // 1��
                    }
            g_adc_dma_sta = 0;                                  /* ���DMA�ɼ����״̬��־ */
            adc_dma_enable(ADC_DMA_BUF_SIZE);                   /* ������һ��ADC DMA�ɼ� */
            vTaskDelayUntil(&xLastWakeTime, xFrequency);
        }
    }
}


/**
 * @brief       task3***************************BMP280��ʪ�Ȳɼ�
 * @param       pvParameters : �������(δ�õ�)
 * @retval      ��
 */
void task3(void *pvParameters)
{
BaseType_t xStatus_bmp280;
bmp280_Data soil_data;
uint8_t ID;
Bmp_Init();
ID=BMP280_ReadID();          //���ID��
TickType_t xLastWakeTime;
const TickType_t xFrequency = pdMS_TO_TICKS(1000); // 

// ��ʼ���ϴλ���ʱ��
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
             vTaskDelay(pdMS_TO_TICKS(2000)); // 2��
            }
    }
}

/**
 * @brief       task4***************************�������ݷ���ָ�����
 * @param       pvParameters : �������(δ�õ�)
 * @retval      ��
 */
void task4(void *pvParameters)
{
    DHT11_Data_TypeDef received_data; /* ��ȡ����ʱ, ������������������ */
    bmp280_Data receive_bmp280;//����ѹ
    
    const TickType_t xTicksToWait = pdMS_TO_TICKS( 1000 );	
    TickType_t xLastWakeTime;
    xLastWakeTime = xTaskGetTickCount();    // ��ʼ���ϴλ���ʱ��
    

BaseType_t xStatus_bmp280;
BaseType_t xStatus_dht11;
    
    char tem[] = "Temp:";
    char hum[] = "Humity:";
    char presure[] = "pre:";
    char back[] = " ";
    char point[] = ".";    
    while(1)
    {
       

        /* ������*/
        xStatus_dht11 = xQueuePeek( xQueueDHT11Data, &received_data, xTicksToWait); /* �õ�buffer��ַ��ֻ��4�ֽ� */
        xStatus_bmp280 = xQueuePeek( xQueuebmp280_Data, &receive_bmp280, xTicksToWait);
        if(( xStatus_dht11 ==  pdTRUE )&(xStatus_bmp280 == pdTRUE))
                {
                EventBits_t bits = xEventGroupSetBits(myEventGroup, 0x01);
                
                char temptuare_int[3];  // �������uint8_t�ķ�Χ��0-255���������ռ��3���ַ�
                sprintf(temptuare_int, "%u", received_data.temp_int);
               
                char temptuare_dec[2];  // �������uint8_t�ķ�Χ��0-255���������ռ��3���ַ�
                sprintf(temptuare_dec, "%u", received_data.temp_deci);        
              
                char humity_int[3];  // �������uint8_t�ķ�Χ��0-255���������ռ��3���ַ�
                sprintf(humity_int, "%u", received_data.humi_int);        
                
                char humity_dec[3];  // �������uint8_t�ķ�Χ��0-255���������ռ��3���ַ�
                sprintf(humity_dec, "%u", received_data.humi_deci);  
                    
                char int_pre[4]; // �洢�������ֵ��ַ�����������ֹ�� '\0'
                char dec_pre[4]; // �洢С�����ֵ��ַ�����������ֹ�� '\0'
                // ����������ת��Ϊ�ַ���
                sprintf(int_pre, "%03d", (int)receive_bmp280.pressure);
                // ��С������ת��Ϊ�ַ�����������λС��
                sprintf(dec_pre, "%03d", (int)((receive_bmp280.pressure - (int)receive_bmp280.pressure) * 1000));
                    
                //****************
                //�ַ���ƴ��
                size_t total = strlen(tem) + strlen(temptuare_int)+ strlen(point)+ strlen(temptuare_dec)+ strlen(back)+ strlen(hum)+ strlen(humity_int)+ strlen(point) + strlen(humity_dec) + strlen(back)+ strlen(presure)+ strlen(int_pre)+ strlen(point)+ strlen(dec_pre)+ 1; // ��1��Ϊ�������ַ���ĩβ��'\0'
                char result[total];
                sprintf(result, "%s%s%s%s%s%s%s%s%s%s%s%s%s%s", tem, temptuare_int, point, temptuare_dec,back, hum, humity_int, point, humity_dec,back,presure,int_pre,point,dec_pre);
                    
                HC05_SendString(result);

                vTaskDelayUntil(&xLastWakeTime, xTicksToWait);
                    
                }
        else
        {
            //printf( "Flase(from bluetooth).\r\n" );
            vTaskDelay(pdMS_TO_TICKS(2000)); // 2��
        }
    }
}

/**
 * @brief       task5***************************CI-03T���ݷ���ָ��ɼ�
 * @param       pvParameters : �������(δ�õ�)
 * @retval      ��
 */
void task5(void *pvParameters)
{
DHT11_Data_TypeDef received_data;//��ʪ��
bmp280_Data receive_bmp280;//����ѹ
const TickType_t xTicksToWait = pdMS_TO_TICKS( 1000 );	
TickType_t xLastWakeTime;
xLastWakeTime = xTaskGetTickCount();// ��ʼ���ϴλ���ʱ��
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
                                char temptuare_int[3];  // �������uint8_t�ķ�Χ��0-255���������ռ��3���ַ�
                                sprintf(temptuare_int, "%u", received_data.temp_int);
                               
                                char temptuare_dec[2];  // �������uint8_t�ķ�Χ��0-255���������ռ��3���ַ�
                                sprintf(temptuare_dec, "%u", received_data.temp_deci);        
                              
                                char humity_int[3];  // �������uint8_t�ķ�Χ��0-255���������ռ��3���ַ�
                                sprintf(humity_int, "%u", received_data.humi_int);        
                                
                                char humity_dec[3];  // �������uint8_t�ķ�Χ��0-255���������ռ��3���ַ�
                                sprintf(humity_dec, "%u", received_data.humi_deci);        
                                size_t uart1_len=strlen(message_1)+strlen(back)+strlen(temptuare_int)+strlen(back)+strlen(temptuare_dec)+1;
                                char datatem_hexsend[uart1_len];
                                sprintf(datatem_hexsend, "%s%s%s%s%s", message_1, back, temptuare_int, back,temptuare_dec);
                                char datahumi_hexsend[uart1_len];
                                sprintf(datahumi_hexsend, "%s%s%s%s%s", message_2, back, humity_int, back,humity_dec);   
                        //********************************************************************************************************        
                                sendDecimalToHex(datatem_hexsend);//�¶�
                                SendFormattedData(receive_bmp280.pressure);//����ѹ
                                sendDecimalToHex(datahumi_hexsend);  //ʪ��
//                                printf("%f,%d\r\n",receive_bmp280.pressure,receive_bmp280.id);
                                vTaskDelayUntil(&xLastWakeTime, xTicksToWait);

                        }        
            EventBits_t bits = xEventGroupSetBits(myEventGroup, 0x02);
            }
            else
            {
                /* û�������� */
                //printf( "Flase(ferom ci-03t).\r\n" );
            vTaskDelay(pdMS_TO_TICKS(2000)); // 2��
            }
    }
}

/**
 * @brief       task6***************************esp8266���ݷ���ָ�����
 * @param       pvParameters : �������(δ�õ�)
 * @retval      ��
 */
void task6(void *pvParameters)
{
//#define TASK_LIST_BUFFER_SIZE 512
//static char taskListBuffer[TASK_LIST_BUFFER_SIZE];   
DHT11_Data_TypeDef received_data;//��ʪ��
bmp280_Data receive_bmp280;//����ѹ
soil_detect_Data soil_data;// ����ʪ��

const TickType_t xTicksToWait = pdMS_TO_TICKS( 3500 );	
TickType_t xLastWakeTime;
xLastWakeTime = xTaskGetTickCount();  // ��ʼ���ϴλ���ʱ��  
    while(1)
    {

//vTaskList(taskListBuffer);// ���������б��ı����洢����������
//printf("Task List:\n%s\n", taskListBuffer);// ��������б��ı�������̨����������豸
        
        //EventBits_t bits = xEventGroupWaitBits(myEventGroup, 0x04, pdTRUE, pdTRUE, 0);
        //if ((bits & 0x04) != 0) 
            {        
                if((xQueuePeek( xQueuebmp280_Data, &receive_bmp280, xTicksToWait) == pdTRUE)|(xQueuePeek( xQueueDHT11Data, &received_data, xTicksToWait)== pdTRUE)|(xQueuePeek( xQueuesoil_Data, &soil_data, xTicksToWait)== pdTRUE))
                {
                        float humidity = received_data.humi_int + received_data.humi_deci / 100.0f;
                        float temperature = received_data.temp_int + received_data.temp_deci / 10.0f;
                        float result = (float)soil_data.original_adc / 4096.0f;
                        float soilhumitidy = roundf(100*result * 10.0f) / 10.0f; // ����һλС��
                        char buffer[50]; // ����һ�����������ڸ�ʽ������
                        // ��ʽ�����ݵ�������
                        sprintf(buffer, "%.1f %6.f %.2f %.1f\n", soilhumitidy, receive_bmp280.pressure * 1000, humidity, temperature);
                        // ��ȡ����������
                        size_t len = strlen(buffer);
                        // ʹ��HAL���UART���ͺ�����������
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
 * @brief       task7***************************OLED����ʾ
 * @param       pvParameters : �������(δ�õ�)
 * @retval      ��
 */
void task7(void *pvParameters)
{

DHT11_Data_TypeDef received_data;//��ʪ��
bmp280_Data receive_bmp280;//����ѹ
soil_detect_Data soil_data;// ����ʪ��

const TickType_t xTicksToWait = pdMS_TO_TICKS( 100UL );	
TickType_t xLastWakeTime;
xLastWakeTime = xTaskGetTickCount();  // ��ʼ���ϴλ���ʱ��  
    
    while(1)
    {
        EventBits_t bits = xEventGroupWaitBits(myEventGroup, 0x02, pdTRUE, pdTRUE, 0);
        if ((bits & 0x02) != 0) 
            {
                if((xQueuePeek( xQueuebmp280_Data, &receive_bmp280, xTicksToWait) == pdTRUE)&(xQueuePeek( xQueueDHT11Data, &received_data, xTicksToWait)== pdTRUE)&(xQueuePeek( xQueuesoil_Data, &soil_data, xTicksToWait)== pdTRUE))
                {
                    OLED_ShowString(3,20,"ID:",OLED_6X8);   
                    OLED_ShowString(3,30,"P:",OLED_6X8);   //��ô���ѹǿ
                    OLED_ShowString(3,40,"T:",OLED_6X8);   //����¶�      
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
                    
                    OLED_ShowNum(70, 30, soil_data.original_adc, 4,OLED_6X8);      /* ��ʾADCC�������ԭʼֵ */        
                    OLED_ShowNum(70, 20, soil_data.adc_integer,1 ,OLED_6X8);      /* ��ʾ��ѹֵ���������֣�3.1111�Ļ������������ʾ3 */
                    OLED_ShowChar(76,20,'.',OLED_6X8);
                    OLED_ShowNum(82, 20, soil_data.temp_decimal, 3,OLED_6X8);   /* ��ʾС�����֣�ǰ��ת��Ϊ��������ʾ����������ʾ�ľ���111. */
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
 * @brief       task8***************************�ƿ�
 * @param       pvParameters : �������(δ�õ�)
 * @retval      ��
 */
void task8(void *pvParameters)
{
    TickType_t xLastWakeTime;
    const TickType_t xFrequency = pdMS_TO_TICKS(1000); // 1��
    // ��ʼ���ϴλ���ʱ��
    xLastWakeTime = xTaskGetTickCount();  
    while(1)
    {
    HC05_show();
    //led_conctrl(g_rx_buffer, sizeof(g_rx_buffer));//on off ����led����
    //led_conctrl(g_rx_buffer2, sizeof(g_rx_buffer2));//on off ����led����
    led_conctrl((char *)g_rx_buffer,(char *)g_rx_buffer2);
//    printf("%s %s\r\n",g_rx_buffer,g_rx_buffer2);
//    clearString(g_usart_rx_buf3, sizeof(g_usart_rx_buf3)); 
//    clearString(g_usart_rx_buf, sizeof(g_usart_rx_buf));     
//    clearString(g_usart_rx_buf2, sizeof(g_usart_rx_buf2));             
    vTaskDelayUntil(&xLastWakeTime, xFrequency);        
        
    }
    
}
