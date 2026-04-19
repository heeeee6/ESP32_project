#ifndef DEMO1_MOTOR_H
#define DEMO1_MOTOR_H
#include <stm32f1xx_hal.h>
#include <stdbool.h>
#include "../json/cJSON.h"

typedef struct Motor_T{
    uint8_t dir1;
    uint32_t speed1;
    uint8_t dir2;
    uint32_t speed2;
    uint8_t dir3;
    uint32_t speed3;
    uint8_t dir4;
    uint32_t speed4;
}Motor_t;

extern Motor_t motor;

void motorStart(void);
void motorStop(void);
void motor1Init(void);
void motor2Init(void);
void motor3Init(void);
void motor4Init(void);

void motor1Speed(bool dir,uint32_t speed);
void motor2Speed(bool dir,uint32_t speed);
void motor3Speed(bool dir,uint32_t speed);
void motor4Speed(bool dir,uint32_t speed);

void mqttCtlMotor(cJSON *pJson);
void motorStop(void);
void motorStart(void);
void motorFull(void);
void motorHalf(void);


#endif //DEMO1_MOTOR_H
