#include <stdint-gcc.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include "mqttCmd.h"
#include "../json/cJSON.h"
#include "chip_id.h"
#include "mqtt_config.h"
#include "motor.h"
#include "rgb.h"

/**
 * 判断消息是否针对当前设备
 * @param payLoadIn ：json消息体
 * @return ： true为有效 。false 为无效
 */

bool isValid(uint8_t * payLoadIn)
{
    cJSON *root=NULL;
    ///将json数据解析到root中
    root=cJSON_Parse((char *)payLoadIn);
    if (root!=NULL)
    {
        cJSON* uuid_array = cJSON_GetObjectItem(root, "uuid");
        if (cJSON_IsArray(uuid_array))
        {
            uint32_t uuid_count = cJSON_GetArraySize(uuid_array);
            for (int i = 0; i < uuid_count; ++i) {
                cJSON* uuid_item = cJSON_GetArrayItem(uuid_array, i);
                if (cJSON_IsString(uuid_item)) {
                    const char* uuid_value = uuid_item->valuestring;
                    if (strcmp(uuid_value,chipId)==0)
                    {
                        cJSON_Delete(root);
                        return true;
                    }
                }
            }
        }
        else{
            cJSON_Delete(root);
            return true;
        }

        cJSON_Delete(root);
        return false;
    }

}

/**
 * 解析接收到mqtt消息
 * @param payLoadIn ： 实际消息体 json格式
      {
        "code":2,
        "uuid":["99887654664576"，"45533332"],
        "msg":{

        }
    }
 */
void mqttCmdProcess(uint8_t * payLoadIn)
{
    cJSON *root=NULL;
    uint16_t code=0;

    //如果没有针对的uuid则不执行
    if (!isValid(payLoadIn)){
        return;
    }

    root=cJSON_Parse((char *)payLoadIn);

    if (root!=NULL)
    {
        code = cJSON_GetObjectItem(root,"code")->valueint;
        cJSON *msg = cJSON_GetObjectItem(root,"msg");
        if (msg==NULL)
        {
            cJSON_Delete(root);
            return;
        }
        switch (code) {
            //mqtt远程控制rgb调色
//            case MQTT_CODE_RGB:
//                mqttCtlRGB(msg);
//                break;
                //mqtt控制四个马达
            case MQTT_CODE_MOTOR:
                mqttCtlMotor(msg);
                break;
        }

        cJSON_Delete(root);
    }
}



