#ifndef __PIC_H
#define __PIC_H 

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define pic_ces pic_dc
#define pic_sine pic_dc
#define pic_rtacs pic_dc

const uint8_t pic_back[460800];
const uint8_t pic_dc[460800];

const uint8_t pic_bat0[2100];
const uint8_t pic_bat1[2100];
const uint8_t pic_bat2[2100];
const uint8_t pic_bat3[2100];

const uint8_t pic_bt[48];

const uint8_t deviceNo_1[1728];
const uint8_t deviceNo_2[1728];
const uint8_t deviceNo_3[1728];
const uint8_t deviceNo_4[1728];
const uint8_t deviceNo_5[1728];
const uint8_t deviceNo_6[1728];


#ifdef __cplusplus
}
#endif

#endif

