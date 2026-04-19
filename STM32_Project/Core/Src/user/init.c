#include "init.h"
#include "motor.h"
#include "h7contect.h"
#include "esp01s.h"
#include "net.h"
#include "myMqtt.h"
#include "mqtt_queue.h"
#include "chip_id.h"
#include "devInfo.h"
#include "rgb.h"

/**
 * 不精确延时
 * @param ms
 */
void delay_ms(uint32_t ms)
{
    uint32_t loops = ms*12000;
    while (loops){
        loops--;
    }
}

/**
 * 软件复位函数
 */
void software_reset(void) {
    //关闭所有中断
    __disable_irq();
    //将值写入复位寄存器
    SCB->AIRCR = (0x5FA << SCB_AIRCR_VECTKEY_Pos) | SCB_AIRCR_SYSRESETREQ_Msk;
    //等待复位成功
    while (1);
}

void userInit(void)
{
    getChipID();

    //初始化电机
    motor1Init();
    motor2Init();
    motor3Init();
    motor4Init();
    F1H7Init();
    esp01sInit();
    rgbInit();
    netInit();
    mqttInit();
    mqttMsgInit();
    devInfoInit();
}