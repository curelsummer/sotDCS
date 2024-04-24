#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "wave_common.h"

//para def
typedef struct _tacs_para_t
{
    float current;
    float freq;
    float offset;
}tacs_para_t;


waveform_t* tacs_generate_waveform(float fi, float dur, float fo, float cur, float freq, float offset, wave_true_sham_t ts)
{
    int32_t err = 0;

    waveform_t* wave = (waveform_t*)calloc(1, sizeof(waveform_t));
    if(wave == NULL)
    {
        err = -1;
        goto exit;
    }

    tacs_para_t *para = (tacs_para_t*)calloc(1, sizeof(tacs_para_t));
    if(para == NULL)
    {
        err = -2;
        goto exit;
    }
    para->current = cur;
    para->freq = freq;
    para->offset = offset;

    wave->id = TACS_WAVE_ID;
    strcpy(wave->name, TACS_WAVE_NAME);
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

int32_t tacs_set_current(waveform_t* wave, float value)
{
    if(wave == NULL)
    {
        return -1;
    }

    tacs_para_t *para = (tacs_para_t*)wave->para;

    para->current = value;

    return 0;
}


int32_t tacs_get_current(waveform_t* wave, float *value)
{
    if(wave == NULL)
    {
        return -1;
    }

    tacs_para_t *para = (tacs_para_t*)wave->para;

    *value = para->current;

    return 0;
}

int32_t tacs_set_freq(waveform_t* wave, float value)
{
    if(wave == NULL)
    {
        return -1;
    }

    tacs_para_t *para = (tacs_para_t*)wave->para;

    para->freq = value;

    return 0;
}


int32_t tacs_get_freq(waveform_t* wave, float *value)
{
    if(wave == NULL)
    {
        return -1;
    }

    tacs_para_t *para = (tacs_para_t*)wave->para;

    *value = para->freq;

    return 0;
}

int32_t tacs_set_offset(waveform_t* wave, float value)
{
    if(wave == NULL)
    {
        return -1;
    }

    tacs_para_t *para = (tacs_para_t*)wave->para;

    para->offset = value;

    return 0;
}


int32_t tacs_get_offset(waveform_t* wave, float *value)
{
    if(wave == NULL)
    {
        return -1;
    }

    tacs_para_t *para = (tacs_para_t*)wave->para;

    *value = para->offset;

    return 0;
}