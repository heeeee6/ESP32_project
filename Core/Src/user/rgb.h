#ifndef DEMO1_RGB_H
#define DEMO1_RGB_H

#include <stdint-gcc.h>
#include <stm32f1xx_hal.h>
#include <tim.h>
#include <stdbool.h>
#include <string.h>
#include "../json/cJSON.h"

typedef struct RGB_S{
    uint32_t r;
    uint32_t g;
    uint32_t b;
}RGB_t;
extern RGB_t rgb;
void setRGBColor(uint32_t rPWM,uint32_t gPWM,uint32_t bPWM);
void rgbInit(void);
void onBreathingLamp(void);
void offBreathingLamp(void);
void breathingLamp(void);
void mqttCtlRGB(cJSON *pJson);

#endif //DEMO1_RGB_H
