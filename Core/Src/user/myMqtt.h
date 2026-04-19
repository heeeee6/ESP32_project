#ifndef DEMO1_MYMQTT_H
#define DEMO1_MYMQTT_H
#include "stdbool.h"
void Mqtt_connect(void);
void mqttInit(void);
bool mqttMsgProcess(void);

typedef enum olineStep_E{
    NC,
    MQTT_CONNECT,
    MQTT_SUBSCRIBE,
    MQTT_PUSH
}olineStep_e;

void devOffline(void);
extern volatile bool isOnline;
#endif //DEMO1_MYMQTT_H
