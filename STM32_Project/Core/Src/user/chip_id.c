#include <stdio.h>
#include "chip_id.h"

uint8_t chipId[50]={0};

/**
 * 获取芯片chipID
 */
void getChipID(void)
{
    uint32_t CpuID[3];
    CpuID[0]=*(uint32_t*)(0x1ffff7e8);
    CpuID[1]=*(uint32_t*)(0x1ffff7ec);
    CpuID[2]=*(uint32_t*)(0x1ffff7f0);
    sprintf((char *)chipId,"%d%d%d",CpuID[0],CpuID[1],CpuID[2]);
}