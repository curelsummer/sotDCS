#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "wave_common.h"

//para def
typedef struct _tdcs_para_t
{
    float current;
}tdcs_para_t;


waveform_t* tdcs_generate_waveform(float fi, float dur, float fo, float cur, wave_true_sham_t ts)
{
    int32_t err = 0;

    waveform_t* wave = (waveform_t*)calloc(1, sizeof(waveform_t));
    if(wave == NULL)
    {
        err = -1;
        goto exit;
    }

    tdcs_para_t *para = (tdcs_para_t*)calloc(1, sizeof(tdcs_para_t));
    if(para == NULL)
    {
        err = -2;
        goto exit;
    }
    para->current = cur;

    wave->id = TDCS_WAVE_ID;
    strcpy(wave->name, TDCS_WAVE_NAME);
    wave->fade_in = fi;
    wave->duration = dur;
    wave->fade_out = fo;
    wave->ts = ts;
    wave->para = para;
    
    return wave;

exit:
    release_waveform(wave);
    return NULL;
}


int32_t tdcs_set_current(waveform_t* wave, float value)
{
    if(wave == NULL)
    {
        return -1;
    }

    tdcs_para_t *para = (tdcs_para_t*)wave->para;

    para->current = value;

    return 0;
}


int32_t tdcs_get_current(waveform_t* wave, float *value)
{
    if(wave == NULL)
    {
        return -1;
    }

    tdcs_para_t *para = (tdcs_para_t*)wave->para;

    *value = para->current;

    return 0;
}