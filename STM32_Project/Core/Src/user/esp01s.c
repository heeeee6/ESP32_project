#include <string.h>
#include <stm32f1xx_hal.h>
#include <usart.h>
#include <stm32f1xx_it.h>
#include "esp01s.h"
#include "myPrintf.h"
#include "init.h"
#include "key.h"
#include "myMqtt.h"
#include "net.h"
#include "rgb.h"

uint8_t espRecvBuf[1000];

bool mqttIpFlag=false;

/**
 * 执行AT指令。执行错误重启模块
 * @param cmd ： 要执行的AT命令指针
 * @return ： 执行成功 ： true   执行失败： false
 */
bool exeATCmd(uint8_t *cmd)
{
    printf_uart2(cmd);
    delay_ms(500);
    getRecvBuf(2);
    if(strstr(netCmd.buf,"OK")==NULL){
        printf_uart2(AT_RST);
        printf_uart4("Error!\r\n");
        delay_ms(2000);
        //再次设置进入串口接收2
        esp01sRecvMode();
        return false;
    }
    return true;
}
/**
 * 进入配网模式（按键长按3s）
 */
void enterAirkiss(void)
{
    if (getKeyFlag()){
        //查询是否处在AT命令模式
        printf_uart2(AT_);
        delay_ms(100);

        //判断是否处在AT命令，如果不在则退出透传模式
        getRecvBuf(2);
        if(strstr(netCmd.buf,"OK")==NULL){
            printf_uart2(EXIT_PASS);
            delay_ms(200);
            esp01sRecvMode();
        }

        //执行配网第1条指令
        if (!exeATCmd(AT_NET1)){
            return;
        }
        //执行配网第2条指令
        if (!exeATCmd(AT_NET2)){
            return;
        }
        //执行配网第3条指令
        if (!exeATCmd(AT_NET3)){
            return;
        }
        /**
         * 进入配网模式整个流程正确执行
         */
        printf_uart1("配网完成\r\n");
        printf_uart1("打开呼吸灯\r\n");
        onBreathingLamp();
        //配网完成后发送标志给主控
        printf_uart4("AT_OK\r\n");
        cleanKeyFlag();
    }

}


/**
 * 设置mqtt服务器ip
 */
void setMqttIp(void)
{
    static uint8_t count=0;
    uint8_t mqtt_at[60];

    //重连WiFi
    if (!mqttIpFlag){
        //查询是否处在AT命令模式
        printf_uart2(AT_);
        delay_ms(500);

        //判断是否处在AT命令，如果不在则退出透传模式
        getRecvBuf(2);
        if(strstr((const char *)netCmd.buf,"OK")==NULL){
            printf_uart2(EXIT_PASS);
            delay_ms(1000);
            esp01sRecvMode();
        }

        printf_uart1("EXIT_PASS！");
        printf_uart2(AT_CIPSTA);
        delay_ms(500);
        getRecvBuf(2);
        if(strstr((const char *)netCmd.buf,"OK")!=NULL){
            //连接到路由器
            if(strstr((const char *)netCmd.buf,"0.0.0.0")!=NULL){
                count++;
                if (count>10){
                    count=0;
                    printf_uart2(AT_RST);
                    delay_ms(1000);
                    //再次设置进入串口接收，准备接收JSON数据
                    esp01sRecvMode();
                }
                return;
            }
        }

        //执行设置mqtt服务器第1条指令
        if (!exeATCmd((uint8_t *)AT_MQTT1)){
            return;
        }
        //执行设置mqtt服务器第2条指令
        if (!exeATCmd((uint8_t *)AT_MQTT2)){
            return;
        }

        //执行设置mqtt服务器第3条指令
        sprintf((char *)mqtt_at,AT_MQTT3,MQTT_HOST);
        if (!exeATCmd(mqtt_at)){
            return;
        }
        mqttIpFlag=true;
        printf_uart2(AT_RST);
        delay_ms(500);
        //再次设置进入串口接收，准备接收json数据
        esp01sRecvMode();
        //这里获取到ip即使在配网模式应该也退出，关闭呼吸灯
        offBreathingLamp();
        printf_uart1("mqtt服务器设置成功\r\n");

        printf_uart2("mqttOK\r\n");
        //告诉主控已经连接上mqtt服务器，可以让用户在手机设置参数了
        printf_uart4("mqttOK\r\n");
    }
}

/**
 * 获取串口接收不定长度数据
 * @param loop ： 如果数据还在接收中，这里循环几次判断
 * @param buf ： 获取到的数据buf,由调用方传入
 * @param size ： buf长度
 * @return
 */

uint32_t getRecvBuf(uint32_t loop)
{
    uint32_t len=0;
    uint32_t count=0;
    memset(netCmd.buf,0,sizeof(netCmd.buf));
    netCmd.len=0;
    for (int i = 0; i < loop; ++i) {
        if (hdma_usart2_rx.Instance->CNDTR<sizeof(espRecvBuf)){
            count = hdma_usart2_rx.Instance->CNDTR;     //待传输的剩余字节数
            delay_ms(20);
            if (hdma_usart2_rx.Instance->CNDTR==count){
                len = sizeof(espRecvBuf) - count;
                for (int j = 0; j < len; ++j) {
                    /**
                     * 这里需要对espRecvBuf进行处理，因为espRecvBuf是DMA接收的，不然数据会覆盖
                     */
                    netCmd.buf[j] = espRecvBuf[j];
                }
                printf_uart1("buf = %s\r\n",netCmd.buf);
                esp01sRecvMode();
                //将数据长度写入netCmd.len
                netCmd.len=len;
                return len;
            }
        }
    }
    return 0;
}


/**
 * 模块at命令返回处理
 * @return
 */
bool espCmdProcess(void)
{
    if (strstr(netCmd.buf,"smartconfig connected wifi")!=NULL){
        printf_uart1("自动配网成功！");
        printf_uart3("AT_OK\r\n");
        offBreathingLamp();
        software_reset();
        return true;
    }
    return false;
}

/**
 * esp01s总循环函数（在主函数中）
 */
void loopEsp01s(void)
{
    //循环调用配网函数
    enterAirkiss();
    //设置mqtt服务器IP
    setMqttIp();
}

/**
 * 注册循环执行函数
 */
void registerLoop(void)
{
    LoopFunc_t loopFunc;
    loopFunc.LoopCallBack=loopEsp01s;
    strcpy(loopFunc.funcName,"loopEsp01s");
    registerLoopCallBack(loopFunc);
}

/**
 * esp01s初始化
 */
void esp01sInit(void)
{
    esp01sRecvMode();
    registerLoop();
}

/**
 * 对于esp01s接收模式设置
 */
void esp01sRecvMode(void)
{
    __HAL_DMA_DISABLE(&hdma_usart2_rx);
    hdma_usart2_rx.Instance->CNDTR=sizeof(espRecvBuf);
    UART_Start_Receive_DMA(&huart2,espRecvBuf,sizeof(espRecvBuf));
    memset(espRecvBuf,0,sizeof(espRecvBuf));
    __HAL_DMA_ENABLE(&hdma_usart2_rx);
}

