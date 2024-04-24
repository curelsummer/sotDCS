#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "wave_common.h"


//para def
typedef struct _rtacs_para_t
{
    float current;
    float start_freq;
    float end_freq;
    float period;
}rtacs_para_t;


waveform_t* rtacs_generate_waveform(float fi, float dur, float fo, float current, float start_freq, float end_freq, float period, wave_true_sham_t ts)
{
    int32_t err = 0;

    waveform_t* wave = (waveform_t*)calloc(1, sizeof(waveform_t));
    if(wave == NULL)
    {
        err = -1;
        goto exit;
    }

    rtacs_para_t *para = (rtacs_para_t*)calloc(1, sizeof(rtacs_para_t));
    if(para == NULL)
    {
        err = -2;
        goto exit;
    }
    para->current = current;
    para->start_freq = start_freq;
    para->end_freq = end_freq;
    para->period = period;

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

int32_t rtacs_set_current(waveform_t* wave, float value)
{
    if(wave == NULL)
    {
        return -1;
    }

    rtacs_para_t *para = (rtacs_para_t*)wave->para;

    para->current = value;

    return 0;
}


int32_t rtacs_get_current(waveform_t* wave, float *value)
{
    if(wave == NULL)
    {
        return -1;
    }

    rtacs_para_t *para = (rtacs_para_t*)wave->para;

    *value = para->current;

    return 0;
}

int32_t rtacs_set_start_freq(waveform_t* wave, float value)
{
    if(wave == NULL)
    {
        return -1;
    }

    rtacs_para_t *para = (rtacs_para_t*)wave->para;

    para->start_freq = value;

    return 0;
}


int32_t rtacs_get_start_freq(waveform_t* wave, float *value)
{
    if(wave == NULL)
    {
        return -1;
    }

    rtacs_para_t *para = (rtacs_para_t*)wave->para;

    *value = para->start_freq;

    return 0;
}

int32_t rtacs_set_end_freq(waveform_t* wave, float value)
{
    if(wave == NULL)
    {
        return -1;
    }

    rtacs_para_t *para = (rtacs_para_t*)wave->para;

    para->end_freq = value;

    return 0;
}


int32_t rtacs_get_end_freq(waveform_t* wave, float *value)
{
    if(wave == NULL)
    {
        return -1;
    }

    rtacs_para_t *para = (rtacs_para_t*)wave->para;

    *value = para->end_freq;

    return 0;
}


int32_t rtacs_set_period(waveform_t* wave, float value)
{
    if(wave == NULL)
    {
        return -1;
    }

    rtacs_para_t *para = (rtacs_para_t*)wave->para;

    para->period = value;

    return 0;
}


int32_t rtacs_get_period(waveform_t* wave, float *value)
{
    if(wave == NULL)
    {
        return -1;
    }

    rtacs_para_t *para = (rtacs_para_t*)wave->para;

    *value = para->period;

    return 0;
}