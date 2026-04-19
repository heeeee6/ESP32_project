#include <stm32f1xx_hal.h>
#include <tim.h>
#include <stdbool.h>
#include "motor.h"
#include "../json/cJSON.h"
#include "myPrintf.h"

Motor_t  motor;
uint8_t count=10;

/**
 * 马达1初始化
 */
void motor1Init(void)
{
    HAL_TIM_PWM_Start(&htim8,TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim8,TIM_CHANNEL_2);
}

/**
 * 马达2初始化
 */
void motor2Init(void)
{
    HAL_TIM_PWM_Start(&htim8,TIM_CHANNEL_3);
    HAL_TIM_PWM_Start(&htim8,TIM_CHANNEL_4);
}
/**
 * 马达3初始化
 */
void motor3Init(void)
{
    HAL_TIM_PWM_Start(&htim4,TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim4,TIM_CHANNEL_2);
}

/**
 * 马达4初始化
 */
void motor4Init(void)
{
    HAL_TIM_PWM_Start(&htim4,TIM_CHANNEL_3);
    HAL_TIM_PWM_Start(&htim4,TIM_CHANNEL_4);
}
/**
 * 马达1停止
 */
void motor1Stop(void)
{
    HAL_TIM_PWM_Stop(&htim8, TIM_CHANNEL_1);
    HAL_TIM_PWM_Stop(&htim8, TIM_CHANNEL_2);
}
/**
 * 马达3停止
 */
void motor3Stop(void)
{
    HAL_TIM_PWM_Stop(&htim4,TIM_CHANNEL_1);
    HAL_TIM_PWM_Stop(&htim4,TIM_CHANNEL_2);
}

/**
 * 马达4停止
 */
void motor4Stop(void)
{
    HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_3);
    HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_4);
}

/**
 * 1左马达——下   设置方向及速度
 * @param dir ： 方向 true ：顺时针 false : 逆时针
 * @param speed ： 速度 0-100
 */
void motor1Speed(bool dir,uint32_t speed)
{
    if (dir){
        setPWMPulse(&htim8,TIM_CHANNEL_1,speed);
        setPWMPulse(&htim8,TIM_CHANNEL_2,0);
    } else{
        setPWMPulse(&htim8,TIM_CHANNEL_1,0);
        setPWMPulse(&htim8,TIM_CHANNEL_2,speed);
    }
}

/**
 * 2右马达——下   设置方向及速度
 * @param dir ： 方向 true ：顺时针 false : 逆时针
 * @param speed ： 速度 0-100
 */
void motor2Speed(bool dir,uint32_t speed)
{
    if (dir){
        setPWMPulse(&htim8,TIM_CHANNEL_3,speed);
        setPWMPulse(&htim8,TIM_CHANNEL_4,0);
    } else{
        setPWMPulse(&htim8,TIM_CHANNEL_3,0);
        setPWMPulse(&htim8,TIM_CHANNEL_4,speed);
    }
    motor.dir2= dir;
    motor.speed2= speed;
}

/**
 * 3左马达——上   设置方向及速度
 * @param dir ： 方向 true ：顺时针 false : 逆时针
 * @param speed ： 速度 0-100
 */
void motor3Speed(bool dir,uint32_t speed)
{
    if (dir){
        setPWMPulse(&htim4,TIM_CHANNEL_1,speed);
        setPWMPulse(&htim4,TIM_CHANNEL_2,0);
    } else{
        setPWMPulse(&htim4,TIM_CHANNEL_1,0);
        setPWMPulse(&htim4,TIM_CHANNEL_2,speed);
    }
    motor.dir3= dir;
    motor.speed3= speed;
}
/**
 * 右马达——上   设置方向及速度
 * @param dir ： 方向 true ：顺时针 false : 逆时针
 * @param speed ： 速度 0-100
 */
void motor4Speed(bool dir,uint32_t speed)
{
    if (dir){
        setPWMPulse(&htim4,TIM_CHANNEL_3,speed);
        setPWMPulse(&htim4,TIM_CHANNEL_4,0);
    } else{
        setPWMPulse(&htim4,TIM_CHANNEL_3,0);
        setPWMPulse(&htim4,TIM_CHANNEL_4,speed);
    }
    //存储解析后的参数至结构体变量中
    motor.dir4= dir;
    motor.speed4= speed;
}

/**
 * mqtt控制所有马达参数
 * @param pJson
 */
void mqttCtlMotor(cJSON *pJson)
{
    printf_uart1("mqttCtlMotor\r\n");
    if(cJSON_GetObjectItem(pJson, "dir1")==NULL){
        return;
    }
    if(cJSON_GetObjectItem(pJson, "speed1")==NULL){
        return;
    }
    if(cJSON_GetObjectItem(pJson, "dir2")==NULL){
        return;
    }
    if(cJSON_GetObjectItem(pJson, "speed2")==NULL){
        return;
    }
    if(cJSON_GetObjectItem(pJson, "dir3")==NULL){
        return;
    }
    if(cJSON_GetObjectItem(pJson, "speed3")==NULL){
        return;
    }
    if(cJSON_GetObjectItem(pJson, "dir4")==NULL){
        return;
    }
    if(cJSON_GetObjectItem(pJson, "speed4")==NULL){
        return;
    }
    uint32_t dir1 = cJSON_GetObjectItem(pJson, "dir1")->valueint;
    uint32_t speed1 = cJSON_GetObjectItem(pJson, "speed1")->valueint;
    uint32_t dir2 = cJSON_GetObjectItem(pJson, "dir2")->valueint;
    uint32_t speed2 = cJSON_GetObjectItem(pJson, "speed2")->valueint;
    uint32_t dir3 = cJSON_GetObjectItem(pJson, "dir3")->valueint;
    uint32_t speed3 = cJSON_GetObjectItem(pJson, "speed3")->valueint;
    uint32_t dir4 = cJSON_GetObjectItem(pJson, "dir4")->valueint;
    uint32_t speed4 = cJSON_GetObjectItem(pJson, "speed4")->valueint;
    motor1Speed(dir1,speed1);
    motor2Speed(dir2,speed2);
    motor3Speed(dir3,speed3);
    motor4Speed(dir4,speed4);

    count++;
    //每设置10次参数发送一次READY
    if(count>10){
        count=0;
        //告诉主控参数设置完毕
        printf_uart4("READY\r\n");
    }
}

/**
 * 开启所有马达
 */
void motorStart(void){
    motor1Init();
    motor2Init();
    motor3Init();
    motor4Init();
}

/**
 * 关闭所有马达
 */
void motorStop(void){
    motor3Stop();
    motor4Stop();
    motor1Stop();
    motor2Speed(0,0);
}

/**
 * 设置所有电机满转——Difficult模式
 */
void motorFull(void){
    motor1Speed(motor.dir1,100);
    motor2Speed(motor.dir2,100);
    motor3Speed(motor.dir3,100);
    motor4Speed(motor.dir4,100);
}

/**
 * 设置所有电机半转——Easy模式
 */
void motorHalf(void){
    motor1Speed(motor.dir1,50);
    motor2Speed(motor.dir2,50);
    motor3Speed(motor.dir3,50);
    motor4Speed(motor.dir4,50);
}
