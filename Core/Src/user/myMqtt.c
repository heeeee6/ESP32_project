#include <stm32f1xx_hal.h>
#include <usart.h>
#include <stdbool.h>
#include <string.h>
#include "myMqtt.h"
#include "../mqtt/MQTTConnect.h"
#include "../json/cJSON.h"
#include "init.h"
#include "net.h"
#include "myMqttSub.h"
#include "mqtt_config.h"
#include "chip_id.h"
#include "mqtt_queue.h"
#include "heartbeat.h"
#include "myPrintf.h"

volatile bool isOnline=false;

olineStep_e volatile onLineStep=NC;

uint32_t volatile mqtt_count;

uint8_t volatile onlineCount=0;

/**
 * 设备下线
 */
void devOffline(void)
{
    isOnline=false;
    onLineStep=NC;
}

/**
 * mqtt连接函数
 * 形式：json串
 */
void Mqtt_connect(void)
{
    int buflen ,len;
    cJSON* root=NULL;
    uint8_t buf[300];
    char msg[100]={0};
    //配置mqtt心跳报文
    MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
    MQTTString username = MQTTString_initializer;
    MQTTString password = MQTTString_initializer;
    MQTTString topicname = MQTTString_initializer;
    MQTTString topicmsg = MQTTString_initializer;

    buflen = sizeof(buf);
    username.cstring = "andy";      //mqtt登录的用户名
    password.cstring = "123456";    //mqtt登录的密码(在控制台设置)
    data.username = username;
    data.password = password;
    data.clientID.cstring = (char *)chipId;//使用芯片的唯一chipID作为clientID
    data.keepAliveInterval = 60;//心跳维持时间60s，在这个时间内要发心跳包
    data.cleansession = 1;
    topicname.cstring =WILL_TOPIC;
    root = cJSON_CreateObject();
    cJSON_AddStringToObject(root,"uuid",chipId);
    cJSON_AddNumberToObject(root,"code",OFFLINE);
    cJSON_AddStringToObject(root,"msg","device offline");
    sprintf(msg, "%s", cJSON_PrintUnformatted(root));
    topicmsg.cstring =msg;
    if (root!=NULL){
        cJSON_Delete(root);
    }

    data.will.topicName=topicname;
    data.will.message=topicmsg;
    data.will.qos=1;
    data.willFlag=1;
    len = MQTTSerialize_connect(buf, buflen, &data);
    HAL_UART_Transmit(&huart2,buf,len,1000);
}


/**
 * mqtt订阅主题
 */
void Mqtt_subscribe(uint8_t **topicName,int *req_qos,uint8_t count)
{
    if (count>10)
        return;
    uint8_t buf[500];
    uint8_t len,bufLen ;
    MQTTString topicString[10] = MQTTString_initializer;
    bufLen = sizeof(buf);
    for (int i = 0; i < count; ++i) {
        topicString[i].cstring=(char *)topicName[i];
        printf("订阅mqtt主题: %s\r\n",topicName[i]);
    }
    len = MQTTSerialize_subscribe(buf, bufLen, 0, 1, count,topicString,req_qos);
    HAL_UART_Transmit(&huart2,buf,len,1000);
}



/**
 * 循环执行函数
 */
void onLineLoop(void)
{
    //如果设备不在线
    if (!isOnline){
        //准备连接mqtt服务器
        if (onLineStep==NC){
            printf("正在连接mqtt...\r\n");
            //关闭心跳机制
            heartbeatClose();
            Mqtt_connect();
            delay_ms(800);
            onlineCount++;
            if (onlineCount>30)
            {
                onlineCount=0;
                software_reset();
            }
            return;
        }
        //mqtt连接成功，准备订阅主题
        if (onLineStep==MQTT_CONNECT){
            ///订阅主题名
            uint8_t *topicNames[]={
                    "device/chipId",
                    "device/all"
            };
            char newTopic[64]={0};
            sprintf(newTopic,"device/%s",chipId);
            topicNames[0] = newTopic;
            int qos[2]={2,2};
            //订阅主题
            Mqtt_subscribe(topicNames,qos,2);
            delay_ms(500);
            onlineCount++;

            if (onlineCount>3)
            {
                onlineCount=0;
                onLineStep=NC;
            }
            return;
        }
        //订阅主题成功，准备发上线消息
        if (onLineStep==MQTT_SUBSCRIBE){
            sendMqttOnlineMsg();
            delay_ms(500);
            //TODO 这个状态需要在确认到服务器端的mqtt客户端发送过来的消息再改
            onLineStep = MQTT_PUSH;
            isOnline = true;
            //打开心跳机制
            heartbeatOpen();

            onlineCount++;
            if (onlineCount>3)
            {
                onlineCount=0;
                onLineStep=NC;
            }
        }

    }
}

/**
 * 任意读取mqtt消息
 * @param buf
 * @param count
 * @return
 */
int transport_getdata(unsigned char* buf, int count)
{
    int res = 0;
    while(count != 0)
    {
        *buf =netCmd.buf[mqtt_count+res];
        buf++;
        count--;
        res++;
    }
    mqtt_count = mqtt_count+res;
    return res;
}

/**
 * 处理mqtt消息
 * @return
 */
bool mqttMsgProcess(void)
{
    unsigned char buf[2048] = {0};
    int bufLen = sizeof(buf);
    uint32_t mqType,len;
    mqtt_count=0;
    uint8_t pacKetType[20];
    uint8_t dup;
    uint16_t pacKetId;
    //反序列化mqtt协议消息
    mqType = MQTTPacket_read(buf,bufLen,transport_getdata);
    ///串口1打印的字符串
    printf("mqType = %d\r\n",mqType);

    switch (mqType) {
        case CONNACK:
            printf("mqtt连接成功！\r\n");
            onLineStep=MQTT_CONNECT;
            onlineCount=0;
            break;
        case SUBACK:
            printf("订阅主题成功！\r\n");
            onLineStep=MQTT_SUBSCRIBE;
            onlineCount=0;
            break;
        case PUBLISH:
            handle_publish(buf,bufLen);
            break;
            //qos=1 时的应答
        case PUBACK:
            MQTTDeserialize_ack(pacKetType,&dup,&pacKetId,buf,bufLen);
            removeMqttMsg(pacKetId);
            break;
            //qos=2时第1次的ack
        case PUBREC:
            MQTTDeserialize_ack(pacKetType,&dup,&pacKetId,buf,bufLen);
            memset(buf,0,sizeof (buf));
            len = MQTTSerialize_pubrec(buf,bufLen,pacKetId);
            //发送1次确认消息
            HAL_UART_Transmit(&huart2,buf,len,1000);
            break;
            //qos=2时第2次的ack
        case PUBREL:
            MQTTDeserialize_ack(pacKetType,&dup,&pacKetId,buf,bufLen);
            memset(buf,0,sizeof (buf));
            len = MQTTSerialize_pubrel(buf,bufLen,dup,pacKetId);
            //发送2次确认消息
            HAL_UART_Transmit(&huart2,buf,len,1000);
            break;
        case PUBCOMP:
            MQTTDeserialize_ack(pacKetType,&dup,&pacKetId,buf,bufLen);
            memset(buf,0,sizeof (buf));
            len = MQTTSerialize_pubcomp(buf,bufLen,pacKetId);
            //发送3次确认消息
            HAL_UART_Transmit(&huart2,buf,len,1000);
            removeMqttMsg(pacKetId);
            break;

            //发送心跳应答
        case PINGRESP:
            resetHeartbeatCount();
            break;

    }
}


/**
 * 注册循环执行函数
 */
void registerMqttLoop(void)
{
    LoopFunc_t loopFunc;
    loopFunc.LoopCallBack=onLineLoop;
    strcpy(loopFunc.funcName,"onLineLoop");
    registerLoopCallBack(loopFunc);
}


void mqttInit(void)
{
    registerMqttLoop();
}

