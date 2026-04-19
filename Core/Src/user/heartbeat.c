#include <stm32f1xx_hal.h>
#include <tim.h>
#include <string.h>
#include <usart.h>
#include "heartbeat.h"
#include "myMqtt.h"
#define MQTTHEARTBEATCOUNT 40

volatile uint16_t mqttHeartbeatCount=MQTTHEARTBEATCOUNT;

/**
 * 复位心跳计数
 */
void resetHeartbeatCount(void)
{
    mqttHeartbeatCount=MQTTHEARTBEATCOUNT;
}


/**
 * 发送心跳到emqx
 */
void mqttHeartbeatSend(void)
{
    uint8_t buf[2]={0xc0,0x00};
    //发送消息到emqx
    HAL_UART_Transmit(&huart2, (uint8_t *) buf, sizeof(buf) , 1000);
}

/**
 * 定时器中断回调函数，1s回调一次
 */
void heartbeatTime(void)
{
    static uint8_t count=0;
    mqttHeartbeatCount--;
    if (mqttHeartbeatCount==0)
    {
        mqttHeartbeatCount=MQTTHEARTBEATCOUNT;
        //设备离线了
        devOffline();
        printf("设备下线");
    }

    count++;
    if (count>30)
    {
        count=0;
        //发送心跳给emqx
        mqttHeartbeatSend();
    }
}


/**
 * 打开心跳机制
 */
void heartbeatOpen(void)
{
    __HAL_TIM_CLEAR_FLAG(&htim7, TIM_SR_UIF);
    htim7.Instance->CNT=0;
    HAL_TIM_Base_Start_IT(&htim7);
}

/**
 * 关闭心跳机制
 */
void heartbeatClose(void)
{
    HAL_TIM_Base_Stop_IT(&htim7);
}