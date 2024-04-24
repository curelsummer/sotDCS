#ifndef __MONITOR_LL_H
#define __MONITOR_LL_H

#include <stdint.h>

#define MON_REFRESH_FREQ (10) //refresh 10 times per second

typedef struct _cali_mon_info_t
{
    float bat_ratio;
}cali_mon_info_t;

#ifdef __cplusplus
 extern "C" {
#endif 


int32_t mon_adc_init(void);
int32_t mon_adc_start(void);
int32_t mon_adc_read_mv(float *mv, uint8_t num);
int32_t mon_adc_stop(void);




#ifdef __cplusplus
}
#endif



#endif /* __MONITOR_LL_H */

