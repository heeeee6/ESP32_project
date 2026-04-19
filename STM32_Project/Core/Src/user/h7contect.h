#ifndef DEMO1_H7CONTECT_H
#define DEMO1_H7CONTECT_H

void F1RecvMode(void);
uint32_t getH7RecvBuf(uint8_t *buf,uint32_t bufLen);
bool exeF1Cmd(uint8_t *cmd);
bool H7CmdProcess(void);
void F1H7Init(void);
bool control_F1(uint8_t *temp,uint32_t len);
void reportDeviceStatus(void);
void registerConnectLoop(void);

//F1WIFI连接标志位
extern bool F1H7Flag;
//F1WIFI传参标志位
extern bool WIFI_transFlag;

#define START   motorStart();
#define STOP    motorStop();
#define DIFFICULT   motorFull();
#define EASY    motorHalf();
#define DA      motor1Speed(1,100);
#define DB      motor1Speed(motor.dir1,100);
#define EA      motor1Speed(1,50);
#define EB      motor4Speed(motor.dir4,50);

#endif //DEMO1_H7CONTECT_H
