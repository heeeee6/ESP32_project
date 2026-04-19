#include <string.h>
#include <stm32f1xx_hal.h>
#include <stm32f1xx_it.h>
#include <main.h>
#include <usart.h>
#include "mqtt_queue.h"

MqttMsg_t mqttMsg_q[MQTTQ_LEN];

/**
 * mqtt消息队列初始化
 */
void mqttMsgInit(void)
{
    __HAL_TIM_CLEAR_FLAG(&htim6, TIM_SR_UIF);
    htim6.Instance->CNT=0;
    HAL_TIM_Base_Start_IT(&htim6);
    //注册循环函数
    registerMqttMsgLoop();
}

/**
 * mqtt消息循环执行函数
 */
void loopMqttMsg(void)
{
    //重发mqtt消息
    for (int i = 0; i < MQTTQ_LEN; ++i) {
        if (mqttMsg_q[i].isEnable){
            if (mqttMsg_q[i].timer>3){
                printf("重发mqtt消息 = %d\r\n",mqttMsg_q[i].packId);
                //重发消息
                HAL_UART_Transmit(&huart2, (uint8_t *) mqttMsg_q[i].msg, mqttMsg_q[i].msgLen, 1000);
                mqttMsg_q[i].timer=0;
                mqttMsg_q[i].send_count++;
            }
        }
    }

    //移除重发次数超限的消息
    for (int i = 0; i < MQTTQ_LEN; ++i) {
        if (mqttMsg_q[i].isEnable){
            if (mqttMsg_q[i].send_count>3){
                mqttMsg_q[i].isEnable=false;
            }
        }
    }
}

/**
 * 注册循环执行函数
 */
void registerMqttMsgLoop(void)
{
    LoopFunc_t loopFunc;
    loopFunc.LoopCallBack=loopMqttMsg;
    strcpy(loopFunc.funcName,"loopMqttMsg");
    registerLoopCallBack(loopFunc);
}

/**
 * 定时器中断处理函数
 */
void mqttTime(void)
{
    for (int i = 0; i < MQTTQ_LEN; ++i) {
        if (mqttMsg_q[i].isEnable){
            mqttMsg_q[i].timer++;
        }
    }
}


/**
 * 向消息队列中插入一条消息
 * @param msg
 * @param len
 */
void insertMqttMsg(uint8_t *msg,uint16_t len,uint8_t qos,uint16_t mqttPackId)
{
    for (int i = 0; i < MQTTQ_LEN; ++i) {
        if (!mqttMsg_q[i].isEnable){
            strncpy(mqttMsg_q[i].msg,msg,len);
            mqttMsg_q[i].isEnable=true;
            mqttMsg_q[i].msgLen=len;
            mqttMsg_q[i].packId = mqttPackId;
            mqttMsg_q[i].send_count=0;
            mqttMsg_q[i].timer=0;
            mqttMsg_q[i].qos = qos;
            return;
        }
    }
}

/**
 * 移除消息队列中指定的消息
 * @param packId ： 包id
 */
void removeMqttMsg(uint16_t packId)
{
    for (int i = 0; i < MQTTQ_LEN; ++i) {
        if (packId == mqttMsg_q[i].packId){
            mqttMsg_q[i].isEnable=false;
        }
    }
}




