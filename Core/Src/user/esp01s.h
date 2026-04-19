#ifndef DEMO1_ESP01S_H
#define DEMO1_ESP01S_H

#include <stdint-gcc.h>
#include <stdbool.h>

//WIFI复位
#define AT_RST "AT+RST\r\n"

///恢复出厂设置
#define AT_RESTORE "AT+RESTORE\r\n"
#define AT_ "AT\r\n"
///查询wifi是否连接路由器
#define AT_CIPSTA "AT+CIPSTA?\r\n"
///配置WiFi模组工作模式为单STA模式，并把配置保存在flash

#define AT_NET1 "AT+CWMODE=1\r\n"
///使能上电自动连接AP
#define AT_NET2 "AT+CWAUTOCONN=1\r\n"
///支持ESP-Touch和Airkiss智能配网，重启后失效

///(如果配网不成功可以将这个改为AT+CWSTARTSMART=3\r\n)
#define AT_NET3 "AT+CWSTARTSMART=2\r\n"

#define EXIT_PASS "+++"

///配置连接的服务器及端口号
#define AT_MQTT1 "AT+CWMODE_DEF=1\r\n"     //配置8266为STA模式
#define AT_MQTT2 "AT+CWAUTOCONN=1\r\n"    //配置8266上电自动连接
#define AT_MQTT3 "AT+SAVETRANSLINK=1,\"%s\",1883,\"TCP\"\r\n"
void enterAirkiss(void);
void setMqttIp(void);
void esp01sInit(void);
void esp01sRecvMode(void);
void registerLoop(void);
uint32_t getRecvBuf(uint32_t loop);
bool exeATCmd(uint8_t *cmd);
bool espCmdProcess(void);
extern bool mqttIpFlag;
#endif //DEMO1_ESP01S_H
