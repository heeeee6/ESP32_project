#include <main.h>
#include <string.h>
#include "net.h"
#include "esp01s.h"
#include "myMqtt.h"
#include "myPrintf.h"

//0xEF 0X01 0X00 0X02 0X00 0X01 0X21


netCmd_t netCmd;
/**
 * 处理模块返回指令
 */
void netCmdProcess(void)
{
    getRecvBuf(1);
    if (netCmd.len>0){
        if (espCmdProcess()){
            return;
        }

        if (mqttMsgProcess()){
            return;
        }

    }
}

/**
 * 注册循环执行函数
 */
void registerNetLoop(void)
{
    LoopFunc_t loopFunc;
    loopFunc.LoopCallBack=netCmdProcess;
    strcpy(loopFunc.funcName,"netCmdProcess");
    registerLoopCallBack(loopFunc);
}

void netInit(void)
{
    registerNetLoop();
}