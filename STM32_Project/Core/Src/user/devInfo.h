#ifndef DEMO1_DEVINFO_H
#define DEMO1_DEVINFO_H
#include <stdbool.h>
#include "motor.h"
#include "rgb.h"

void devInfoInit(void);
typedef struct DevInfo_T{
    Motor_t  motor;
    RGB_t rgb;
//    float voltage;
//    uint16_t curr;
}DevInfo_t;


#endif //DEMO1_DEVINFO_H
