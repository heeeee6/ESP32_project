#ifndef DEMO1_NET_H
#define DEMO1_NET_H

#include <stdint-gcc.h>

#define MQTT_HOST "129.226.189.180"       //部署的服务器的IP

#define DEV_TO_SERVER_TOPIC "device/to/server"  //上线消息的主题

#define WILL_TOPIC "MiniLastWillTopic"

typedef struct netCmd_T{
    uint32_t len;
    uint8_t buf[1000];
}netCmd_t;

extern  netCmd_t netCmd;
void netInit(void);
#endif //DEMO1_NET_H
