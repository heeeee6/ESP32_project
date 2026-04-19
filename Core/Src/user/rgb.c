#include "rgb.h"
#include "../json/cJSON.h"

bool rgbFlag=false;

RGB_t rgb;

/**
 * 设置RGB颜色
 * @param rPWM ： 红色的配比  0-4999
 * @param gPWM ： 绿色的配比  0-4999
 * @param bPWM ： 蓝色的配比  0-4999
 */
void setRGBColor(uint32_t rPWM,uint32_t gPWM,uint32_t bPWM)
{
    setPWMPulse(&htim2,TIM_CHANNEL_2,rPWM);
    setPWMPulse(&htim3,TIM_CHANNEL_2,gPWM);
    setPWMPulse(&htim3,TIM_CHANNEL_1,bPWM);

    rgb.r = rPWM;
    rgb.g = gPWM;
    rgb.b = bPWM;
}

/**
 * 呼吸灯循环执行
 */
void breathingLamp(void){


    static uint32_t count=0;
    static bool flag=true;

    if (!rgbFlag){
        return;
    }
    if (flag){
        count=count+20;
    } else{
        count=count-20;
    }
    if (4500==count)
    {
        flag=false;
    }
    if (0==count)
    {
        flag=true;
    }
    setRGBColor(count,count,count);
}

/**
 * 初始化rgb
 */
void rgbInit(void)
{
    HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_1);
    //关闭RGB
    setRGBColor(0,0,0);
}

/**
 * 打开呼吸灯
 */
void onBreathingLamp(void)
{
    HAL_TIM_Base_Start_IT(&htim5);
    rgbFlag=true;
}

/**
 * 关闭呼吸灯
 */
void offBreathingLamp(void)
{
    rgbFlag=false;
    HAL_TIM_Base_Stop_IT(&htim5);
    setRGBColor(0,0,0);
}

/**
 * 远程控制RGB调色
 * @param pJson
 *  * {
        "r":1000,
        "g":2000,
        "b":3000,
        "num":0
    }
 */
void mqttCtlRGB(cJSON *pJson) {

    if (cJSON_GetObjectItem(pJson, "r")==NULL){
        return;
    }
    if (cJSON_GetObjectItem(pJson, "g")==NULL){
        return;
    }
    if (cJSON_GetObjectItem(pJson, "b")==NULL){
        return;
    }

    uint32_t r = cJSON_GetObjectItem(pJson, "r")->valueint;
    uint32_t g = cJSON_GetObjectItem(pJson, "g")->valueint;
    uint32_t b = cJSON_GetObjectItem(pJson, "b")->valueint;
    setRGBColor(r,g,b);
}