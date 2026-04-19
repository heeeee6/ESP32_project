#include <stdint-gcc.h>
#include "gpio.h"
#include "key.h"
#include "stm32f1xx_hal.h"
#include "stm32f1xx_it.h"
#include "rgb.h"

volatile bool keyFlag=false;

/**
 * 获取长按事件状态
 * @return
 */
bool getKeyFlag(void)
{
    return keyFlag;
}

/**
 * 清除长按标志
 */
void cleanKeyFlag(void)
{
    keyFlag=false;
}

/**
 * 定时器中断3s
 */
void keyEvent(void)
{
    if (!keyFlag){
        keyFlag=true;
        setRGBColor(1000,1000,1000);
    }
    HAL_TIM_Base_Stop_IT(&htim2);
}

/**
 * 按键中断
 */
void keyProcess(void)
{
    //按键按下，下降沿触发
    if (HAL_GPIO_ReadPin(KEY_GPIO_Port,KEY_Pin)==0)
    {
        __HAL_TIM_CLEAR_FLAG(&htim2, TIM_SR_UIF);
        htim2.Instance->CNT=0;
        HAL_TIM_Base_Start_IT(&htim2);
    } else{
        HAL_TIM_Base_Stop_IT(&htim2);
    }
}

