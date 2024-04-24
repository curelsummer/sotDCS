#ifndef __CONFIG_H
#define __CONFIG_H

#include <stdint.h>

#include "waveform.h"


//stimulate mode
typedef enum _sti_mode_t
{
    ST_MODE_SINGLE = 1,
    ST_MODE_DOUBLE_SYNC = 2,
    ST_MODE_DOUBLE_ASYN = 3
}sti_mode_t;
#define IS_VALIDE_STMODE(n) (n == ST_MODE_SINGLE)


//sti config
typedef struct _sti_config_t
{
    char name[32];
    sti_mode_t sti_mode;
    uint32_t wave_num;
    waveform_t *waves;
}sti_config_t;


#ifdef __cplusplus
 extern "C" {
#endif 


int32_t cfg_init_single_stimulation(sti_config_t *cfg);
int32_t cfg_load_waveform(sti_config_t *cfg, waveform_t *wave);

float cfg_get_total_time(sti_config_t *cfg);
char* cfg_get_name(sti_config_t *cfg);

#ifdef __cplusplus
}
#endif

#endif /* __CONFIG_H */
