#ifndef __E_PRES_H
#define __E_PRES_H

#include <stdint.h>
#include "waveform.h"

#define MAX_E_PRES_NAME_SIZE (32)
#define MAX_STI_CH (2)

//stimulate mode
typedef enum _sti_mode_t
{
    ST_MODE_SINGLE = 1,
    ST_MODE_DOUBLE_SYNC = 2,
    ST_MODE_DOUBLE_ASYN = 3
}sti_mode_t;
#define IS_VALIDE_STMODE(n) (n == ST_MODE_SINGLE)


//e-prescription
typedef struct _e_pres_t
{
    char name[MAX_E_PRES_NAME_SIZE];
    sti_mode_t sti_mode;
    uint32_t wave_num;
    waveform_t *waves[MAX_STI_CH];
}e_pres_t;


#ifdef __cplusplus
 extern "C" {
#endif 


e_pres_t *epres_generate_single_mode(char *name, waveform_t *wave);
int32_t epres_set_name(e_pres_t *e_pres, char *name);
const char *epres_get_name(e_pres_t *e_pres);
int32_t epres_set_wave(e_pres_t *e_pres, uint32_t index, waveform_t *wave);

float epres_get_total_time(e_pres_t *e_pres);


#ifdef __cplusplus
}
#endif

#endif /* __E_PRES_H */
