#include <stm32f1xx_hal.h>
#include <usart.h>
#include <string.h>
#include "myMqttSub.h"
#include "../mqtt/MQTTPacket.h"
#include "myPrintf.h"
#include "../json/cJSON.h"
#include "mqtt_config.h"
#include "chip_id.h"
#include "net.h"
#include "mqtt_queue.h"
#include "mqttCmd.h"

volatile uint16_t mqttPackId=1;

/**
 * 发送上线消息
 */
void sendMqttOnlineMsg(void)
{
    cJSON* root=NULL;
    cJSON* msgObj=NULL;
    char msg[300]={0};
    root = cJSON_CreateObject();
    cJSON_AddStringToObject(root,"uuid",chipId);
    cJSON_AddNumberToObject(root,"code",ONLINE);

    msgObj = cJSON_CreateArray();
    cJSON_AddItemToArray(msgObj, cJSON_CreateNumber(MOTOR));
//    cJSON_AddItemToArray(msgObj, cJSON_CreateNumber(RGB));

//    cJSON_AddItemToArray(msgObj, cJSON_CreateNumber(0));        //lamp
//    cJSON_AddItemToArray(msgObj, cJSON_CreateNumber(0));        //RGB
    cJSON_AddItemToObject(root,"msg",msgObj);
    sprintf(msg, "%s", cJSON_PrintUnformatted(root));
    if (root!=NULL){
        cJSON_Delete(root);
    }

    if (msgObj!=NULL){
        cJSON_Delete(msgObj);
    }

    mqttSerializePublish(msg,strlen(msg),0);
}

/**
 * 序列化发送mqtt消息
 * @param msg ： 要发送的消息json串
 * @param len ：消息长度
 * @param qos ： 消息质量
 */
void mqttSerializePublish(uint8_t *msg,uint16_t len,uint8_t qos)
{
    uint8_t buf[350];
    int buflen ;
    MQTTString topic= MQTTString_initializer;
    buflen = sizeof(buf);

    printf("发送消息qos = %d\r\n",qos);
    topic.cstring=(char *)DEV_TO_SERVER_TOPIC;
    printf_uart1("发送消息主题 = %s\r\n",topic.cstring);
    len = MQTTSerialize_publish(buf,buflen,0,
                                qos, 1,mqttPackId,topic,
                                (uint8_t *)msg,len);

    //发送消息到emqx
    HAL_UART_Transmit(&huart2, (uint8_t *) buf, len, 1000);

    //如果qos不为0说明需要应答，这里插入到消息队列
    if (qos!=0){
        insertMqttMsg(buf,len,qos,mqttPackId);
    }

    mqttPackId++;
    if (mqttPackId>65502){
        mqttPackId=1;
    }
}
/**
 * 处理接收到的消息
 * @param buf ： 接收到未序列化的消息体
 * @param bufLen ：接收到数据长度
 */
void handle_publish(unsigned char * buf,int bufLen)
{
    uint8_t * payLoadIn;
    int payLoadLenIn = 0;
    unsigned char dup;
    int qos;
    int size;
    unsigned char retained;
    unsigned short msgId;
    MQTTString receivedTopic;
    char bufRec[40]={0};
    int bufRecLen=sizeof (bufRec);

    if(MQTTDeserialize_publish(&dup, &qos, &retained, &msgId, &receivedTopic,
                               &payLoadIn, &payLoadLenIn, buf, bufLen) == 1) {

        //对服务器推送过来的消息分析"qos"
        switch (qos) {
            case 1:
                size = MQTTSerialize_puback((uint8_t *) bufRec, bufRecLen, msgId);
                HAL_UART_Transmit(&huart2, (uint8_t *) bufRec, size, 1000);
                break;
            case 2:
                size = MQTTSerialize_puback((uint8_t *) bufRec, bufRecLen, msgId);
                HAL_UART_Transmit(&huart2, (uint8_t *) bufRec, size, 1000);
                break;
        }

        printf_uart1("接收到消息 = %s\r\n",payLoadIn);
        mqttCmdProcess(payLoadIn);
    }
}

