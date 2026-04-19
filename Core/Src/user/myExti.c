#include "myExti.h"
#include "key.h"
#include "h7contect.h"
#include "myPrintf.h"
#include "esp01s.h"
#include "mqtt_queue.h"
#include "heartbeat.h"
#include "rgb.h"

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    if(huart->Instance==UART4)
    {

    }
}
/**
 * 定时器周期执行中断
 * @param htim
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance==TIM2) //3s
    {
        keyEvent();
    }
    if (htim->Instance==TIM5) //10ms
    {
        breathingLamp();
    }
    if (htim->Instance==TIM6) //1s
    {
        mqttTime();
    }
    if (htim->Instance==TIM7) //1s
    {
        heartbeatTime();
    }
}

/**
 * 外部中断
 * @param GPIO_Pin ：GPIO管脚
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin==GPIO_PIN_0)
    {
        keyProcess();
    }
}

