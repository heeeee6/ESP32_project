#ifndef DEMO1_MQTT_QUEUE_H
#define DEMO1_MQTT_QUEUE_H
#include <stdint-gcc.h>
#include <stdbool.h>

#define MQTTQ_LEN 10

typedef struct {
    uint8_t msg[350];//消息实体
    uint16_t msgLen;//消息实体的长度
    uint16_t packId;//发送的包id
    uint8_t send_count;//重发次数
    uint8_t timer;//重发时间
    uint8_t qos;//消息质量
    bool isEnable;
}MqttMsg_t;
void insertMqttMsg(uint8_t *msg,uint16_t len,uint8_t qos,uint16_t mqttPackId);
void mqttMsgInit(void);
void mqttTime(void);
void registerMqttMsgLoop(void);
void removeMqttMsg(uint16_t packId);
#endif //DEMO1_MQTT_QUEUE_H
