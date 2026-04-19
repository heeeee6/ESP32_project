#include <string.h>
#include <main.h>
#include "devInfo.h"
#include "myMqtt.h"
#include "init.h"
#include "chip_id.h"
#include "mqtt_config.h"
#include "myMqttSub.h"
#include "rgb.h"
#include "myPrintf.h"

DevInfo_t devInfoOld;

bool isChange(void)
{
    bool ret=false;
//判断rgb灯是否发生改变
    if (devInfoOld.rgb.r!=rgb.r)
    {
        ret =  true;
    }
    if (devInfoOld.rgb.g!=rgb.g)
    {
        ret =  true;
    }
    if (devInfoOld.rgb.b!=rgb.b)
    {
        ret =  true;
    }
//判断电机1是否改变
    if (devInfoOld.motor.speed1!=motor.speed1)
    {
        ret =  true;
    }
    if (devInfoOld.motor.dir1!=motor.dir1)
    {
        ret =  true;
    }
//电机2
    if (devInfoOld.motor.speed2!=motor.speed2)
    {
        ret =  true;
    }
    if (devInfoOld.motor.dir2!=motor.dir2)
    {
        ret =  true;
    }

//判断电机3
    if (devInfoOld.motor.speed3!=motor.speed3)
    {
        ret =  true;
    }
    if (devInfoOld.motor.dir3!=motor.dir3)
    {
        ret =  true;
    }
//判断电机4
    if (devInfoOld.motor.speed4!=motor.speed4)
    {
        ret =  true;
    }
    if (devInfoOld.motor.dir4!=motor.dir4)
    {
        ret =  true;
    }
//如果发生改变
     if (ret)
        {
            devInfoOld.motor.speed1=motor.speed1;
            devInfoOld.motor.dir1=motor.dir1;
            devInfoOld.motor.speed2=motor.speed2;
            devInfoOld.motor.dir2=motor.dir2;
            devInfoOld.motor.speed3=motor.speed3;
            devInfoOld.motor.dir3=motor.dir3;
            devInfoOld.motor.speed4=motor.speed4;
            devInfoOld.motor.dir4=motor.dir4;
        }
    return ret;
}

/**
 * 上报当前设备状态值
 */
void submitDevInfo(void)
{
    cJSON* root=NULL;
    cJSON* msgArray=NULL;
    cJSON* tempObj=NULL;
    char msg[500]={0};
    root = cJSON_CreateObject();
    cJSON_AddStringToObject(root,"uuid",chipId);
    cJSON_AddNumberToObject(root,"code",MQTT_SUBMIT);

    tempObj = cJSON_CreateObject();
    cJSON_AddNumberToObject(tempObj,"r",rgb.r);
    cJSON_AddNumberToObject(tempObj,"g",rgb.g);
    cJSON_AddNumberToObject(tempObj,"b",rgb.b);
    msgArray = cJSON_CreateArray();
    cJSON_AddItemToArray(msgArray,tempObj);
    cJSON_AddItemToObject(root,"rgb",msgArray);

    tempObj = cJSON_CreateObject();
    cJSON_AddNumberToObject(tempObj,"dir1",motor.dir1);
    cJSON_AddNumberToObject(tempObj,"speed1",motor.speed1);
    cJSON_AddNumberToObject(tempObj,"dir2",motor.dir2);
    cJSON_AddNumberToObject(tempObj,"speed2",motor.speed2);
    cJSON_AddNumberToObject(tempObj,"dir3",motor.dir3);//100
    cJSON_AddNumberToObject(tempObj,"speed3",motor.speed3);//100
    cJSON_AddNumberToObject(tempObj,"dir4",motor.dir4);
    cJSON_AddNumberToObject(tempObj,"speed4",motor.speed4);
    msgArray = cJSON_CreateArray();
    cJSON_AddItemToArray(msgArray,tempObj);
    cJSON_AddItemToObject(root,"motor",msgArray);

    sprintf(msg,"%s", cJSON_PrintUnformatted(root));

    cJSON_Delete(root);
    mqttSerializePublish(msg, strlen(msg),2);

}

/**
 * 循环上报函数
 */
void submitDevInfoLoop(void)
{
    if (isOnline){

        if (isChange())
        {
            //上报当前设备所有状态
            submitDevInfo();
            printf_uart1("上报成功\r\n");

        }
    }
}

/**
 * 注册循环执行函数
 */
void registerSubmitDevInfoLoop(void)
{
    LoopFunc_t loopFunc;
    loopFunc.LoopCallBack=submitDevInfoLoop;
    strcpy(loopFunc.funcName,"submitDevInfoLoop");
    registerLoopCallBack(loopFunc);
}

/**
 * 初始化函数
 */
void devInfoInit(void)
{
    registerSubmitDevInfoLoop();
    devInfoOld.rgb = rgb;
    devInfoOld.motor=motor;
}