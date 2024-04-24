#include <stdint.h>
#include <stdlib.h>

#include "wave_common.h"


void release_waveform(waveform_t *wave)
{
    if(wave != NULL)
    {
        if(wave->para != NULL)
        {
            free(wave->para);
        }

        free(wave);
    }
}


int32_t wave_set_fade_in(waveform_t *wave, float value)
{
    if(wave == NULL)
    {
        return -1;
    }

    wave->fade_in = value;

    return 0;
}

int32_t wave_set_fade_out(waveform_t *wave, float value)
{
    if(wave == NULL)
    {
        return -1;
    }

    wave->fade_out = value;

    return 0;
}

int32_t wave_set_duration(waveform_t *wave, float value)
{
    if(wave == NULL)
    {
        return -1;
    }

    wave->duration = value;

    return 0;
}


int32_t wave_set_ts(waveform_t *wave, wave_true_sham_t ts)
{
    if(wave == NULL)
    {
        return -1;
    }

    wave->ts = ts;

    return 0;
}


int32_t wave_get_fade_in(waveform_t *wave, float *value)
{
    if((wave == NULL) || (value == NULL))
    {
        return -1;
    }

    *value = wave->fade_in;

    return 0;
}


int32_t wave_get_fade_out(waveform_t *wave, float *value)
{
    if((wave == NULL) || (value == NULL))
    {
        return -1;
    }

    *value = wave->fade_out;

    return 0;
}


int32_t wave_get_duration(waveform_t *wave, float *value)
{
    if((wave == NULL) || (value == NULL))
    {
        return -1;
    }

    *value = wave->duration;

    return 0;
}

int32_t wave_get_ts(waveform_t *wave, wave_true_sham_t *ts)
{
    if((wave == NULL) || (ts == NULL))
    {
        return -1;
    }

    *ts = wave->ts;

    return 0;
}

int32_t wave_get_total_time(waveform_t *wave, float *time)
{
    if((wave == NULL) || (time == NULL))
    {
        return -1;
    }

    *time = wave->fade_in + wave->duration + wave->fade_out;

    return 0;
}

char *wave_get_name(waveform_t *wave)
{
    if(wave == NULL)
    {
        return NULL;
    }

    return wave->name;
}