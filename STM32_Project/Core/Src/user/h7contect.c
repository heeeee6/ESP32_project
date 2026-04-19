#include <string.h>
#include <gpio.h>
#include <stm32f1xx_it.h>
#include <stdbool.h>
#include "stm32f1xx_hal.h"
#include "h7contect.h"
#include "myPrintf.h"
#include "init.h"
#include "motor.h"
#include "esp01s.h"

#define F1_RX_BUF_SIZE 200

uint8_t F1RcvBuf[F1_RX_BUF_SIZE];

//F1WIFI连接标志位
bool F1H7Flag=false;
//F1WIFI传参标志位
bool WIFI_transFlag=false;

/**
 * H7F1通信建立
 */
void enterConnectMode(void){
    //判断是否成功连接
    uint32_t len=0;
    uint8_t temp[F1_RX_BUF_SIZE];
    memset(temp,0,sizeof(temp));
    len= getH7RecvBuf(temp, sizeof(temp));

    if(len>0){
        if (!F1H7Flag) {
            F1H7Flag=true;
            printf_uart4("SetOK!\r\n");

        }
            ///判断H7是否已经接收到F1连接mqtt服务器指令
        else if(F1H7Flag) {
            //等待H7发送指令
            control_F1(temp,len);
            printf_uart1(temp);
            //调试
            printf_uart1("Connect OK\r\n");
            //发送当前电机状态
            reportDeviceStatus();
        }
        return;
    }
}

/*
 * 上报当前电机状态
 */
void reportDeviceStatus(void){
    exeF1Cmd((uint8_t *)motor.dir1);
    exeF1Cmd((uint8_t *)motor.speed1);
    exeF1Cmd((uint8_t *)motor.dir2);
    exeF1Cmd((uint8_t *)motor.speed2);
    exeF1Cmd((uint8_t *)motor.dir3);
    exeF1Cmd((uint8_t *)motor.speed3);
    exeF1Cmd((uint8_t *)motor.dir4);
    exeF1Cmd((uint8_t *)motor.speed4);
}
/**
 * 以上为应用层——————————————————————————————————————————
 */

/**
 *  F1受控制流程
 *  @param cmd 需要发送的关键参数(motor/adc)
 *  @return 是否成功控制
 */
bool control_F1(uint8_t *temp,uint32_t len)
{
    if(len>0){
        if (strstr(temp,"START")!=NULL){
            //开启所有电机
            printf_uart4("START");
            START
            return true;
        }
        else if (strstr(temp,"STOP")!=NULL){
            //关闭所有电机
            printf_uart4("STOP");
            STOP
            return true;
        }
        else if (strstr(temp,"DIFFICULT")!=NULL){
            //所有电机满转
            printf_uart4("DIFFICULT");
            DIFFICULT
            return true;
        }
        else if (strstr(temp,"EASY")!=NULL){
            //所有电机慢转
            printf_uart4("E2ASY");
            EASY
            return true;
        }
        else if (strstr(temp,"DA")!=NULL){
            //所有电机慢转
            printf_uart4("DA");
            DA
            return true;
        }
        else if (strstr(temp,"EA")!=NULL){
            //所有电机慢转
            printf_uart4("EA");
            EA
            return true;
        }
        else if (strstr(temp,"DB")!=NULL){
            //所有电机慢转
            printf_uart4("DB");
            DIFFICULT
            return true;
        }
        else if (strstr(temp,"EB")!=NULL){
            //所有电机慢转
            printf_uart4("EB");
            EASY
            return true;
        }
    }
    return false;
}

/**
 * 以上为应用层——————————————————————————————————————————————
 */

/**
 * F1接收模式设置——初始化中加
 */
void F1RecvMode(void)
{
    //禁用DMA通道
    __HAL_DMA_DISABLE(&hdma_uart4_rx);
    //设置DMA通道的当前地址为接收缓冲区的起始地址
    hdma_uart4_rx.Instance->CNDTR=sizeof(F1RcvBuf);
    //启动DMA通道，开始接收数据
    UART_Start_Receive_DMA(&huart4,F1RcvBuf,sizeof(F1RcvBuf));
    //清空接收缓冲区
    memset(F1RcvBuf,0,sizeof(F1RcvBuf));
    //启用DMA通道
    __HAL_DMA_ENABLE(&hdma_uart4_rx);
}

/**
 * 获取H7串口不定长数据
 * @param buf       获取到的数据，由调用方传入
 * @param bufLen    数据长度
 * @return          是否成功获取到数据，成功返回bufLen，失败返回0
 */
uint32_t getH7RecvBuf(uint8_t *buf,uint32_t bufLen)
{
    uint32_t len=0;
    uint32_t count=0;
    if (hdma_uart4_rx.Instance->CNDTR<sizeof(F1RcvBuf)){
        count = hdma_uart4_rx.Instance->CNDTR;     //待传输的剩余字节数，接收一个自减
        delay_ms(30);
        //判断是否有接收数据
        if (hdma_uart4_rx.Instance->CNDTR==count){
            memset(buf,0,bufLen);

            len = sizeof(F1RcvBuf) - count;
            for (int i = 0; i < len; ++i) {
                buf[i] = F1RcvBuf[i];
            }
            F1RecvMode();
            ///接收应答
//            printf_uart4("OK\r\n");

            return len;
        }
    }
    return 0;
}

/**
 * 以上为协议层——————————————————————————————————————————————
 */

/**
 * F1发送一般指令/数据
 * H7执行F1指令。执行错误重启模块
 * @param cmd ： 要执行的F1命令指针
 * @return ： 执行成功 ： true   执行失败： false
 */
bool exeF1Cmd(uint8_t *cmd)
{
    uint8_t temp[F1_RX_BUF_SIZE];
    //F1输出指令
    printf_uart4(cmd);
//    delay_ms(1000);
//    getH7RecvBuf(temp,sizeof(temp));
    //判断H7是否接收成功
//    if(strstr(temp,"OK")==NULL){
//        printf_uart1("H7接收失败！");
//        printf_uart4("H7接收失败");
//        //再次设置F1进入串口接收
//        F1RecvMode();
//        F1H7Flag= false;
//        //未连接
//        return false;
//    }
//    F1H7Flag=true;
    return true;
}


/**
 * F1与H7通信总循环函数（在主函数中）
 */
void loopF1H7(void)
{
    //循环调用连接函数，每次循环设置参数
    enterConnectMode();
    registerConnectLoop();
}

/**
 * 注册循环执行函数
 */
void registerConnectLoop(void)
{
    LoopFunc_t loopFunc;
    loopFunc.LoopCallBack=loopF1H7;
    //注册loopF1H7函数到主函数循环中
    strcpy(loopFunc.funcName,"loopF1H7");
    registerLoopCallBack(loopFunc);
}


/**
 * F1与H7通信初始化
 */
void F1H7Init(void)
{
    F1RecvMode();
    registerConnectLoop();
}

/**
 * 以上为通信层——————————————————————————————————————————
 */
