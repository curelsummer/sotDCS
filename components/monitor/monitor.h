#ifndef __MONITOR_H
#define __MONITOR_H

#include <stdint.h>

#define MAX_MON_PORT (1)

typedef struct _monitor_t
{
    float current[MAX_MON_PORT];
    float voltage[MAX_MON_PORT];
    float impedance[MAX_MON_PORT];
    float battery;
}monitor_t;


#ifdef __cplusplus
 extern "C" {
#endif 

void monitor(void *arg);


int32_t mon_init(void);

int32_t mon_start(void);
void mon_update_info(void);
int32_t mon_stop(void);

int32_t mon_get_current(float *cur);
int32_t mon_get_voltage(float *vol);
int32_t mon_get_impedance(float *imp);
int32_t mon_get_battery_v(float *bat);
int32_t mon_get_battery_level(uint32_t *level);

#ifdef __cplusplus
}
#endif



#endif /* __MONITOR_H */

