#ifndef DEMO1_MYMQTTSUB_H
#define DEMO1_MYMQTTSUB_H
void handle_publish(unsigned char * buf,int bufLen);
void sendMqttOnlineMsg(void);
void mqttSerializePublish(uint8_t *msg,uint16_t len,uint8_t qos);
#endif //DEMO1_MYMQTTSUB_H
