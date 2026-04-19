#include <stdint-gcc.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "myPrintf.h"
#include "stm32f1xx_hal.h"
#include "usart.h"

void printf_uart1(char *format,...){
    uint8_t buffer[1000];
    va_list arg;
    va_start(arg, format);
    vsnprintf(buffer, sizeof(buffer), format, arg);
    va_end(arg);
    for (int i = 0; i< strlen(buffer); ++i) {
        HAL_UART_Transmit(&huart1,  &buffer[i], 1,100);
    }
}
void printf_uart2(char *format,...){
    uint8_t buffer[1000];
    va_list arg;
    va_start(arg, format);
    vsnprintf(buffer, sizeof(buffer), format, arg);
    va_end(arg);
    for (int i = 0; i< strlen(buffer); ++i) {
        HAL_UART_Transmit(&huart2,  &buffer[i], 1,500);
    }
}
void printf_uart3(char *format,...){
    uint8_t buffer[1000];
    va_list arg;
    va_start(arg, format);
    vsnprintf(buffer, sizeof(buffer), format, arg);
    va_end(arg);
    for (int i = 0; i< strlen(buffer); ++i) {
        HAL_UART_Transmit(&huart3,  &buffer[i], 1,100);
    }
}

void printf_uart4(char *format,...){
    uint8_t buffer[1000];
    va_list arg;
    va_start(arg, format);
    vsnprintf(buffer, sizeof(buffer), format, arg);
    va_end(arg);
    for (int i = 0; i< strlen(buffer); ++i) {
        HAL_UART_Transmit(&huart4,  &buffer[i], 1,100);
    }
}
