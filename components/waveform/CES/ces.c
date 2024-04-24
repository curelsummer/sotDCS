#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "ces.h"
#include "wave_common.h"

//para def
typedef struct _ces_para_t
{
    float current;
    float width;
    float freq;
}ces_para_t;

waveform_t* ces_generate_waveform(float fi, float dur, float fo, float cur, float wid, float freq, wave_true_sham_t ts)
{
    int32_t err = 0;

    waveform_t* wave = (waveform_t*)calloc(1, sizeof(waveform_t));
    if(wave == NULL)
    {
        err = -1;
        goto exit;
    }

    ces_para_t *para = (ces_para_t*)calloc(1, sizeof(ces_para_t));
    if(para == NULL)
    {
        err = -2;
        goto exit;
    }
    para->current = cur;
    para->width = wid;
    para->freq = freq;

    wave->id = CES_WAVE_ID;
    strcpy(wave->name, CES_WAVE_NAME);
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

int32_t ces_set_current(waveform_t* wave, float value)
{
    if(wave == NULL)
    {
        return -1;
    }

    ces_para_t *para = (ces_para_t*)wave->para;

    para->current = value;

    return 0;
}

int32_t ces_get_current(waveform_t* wave, float *value)
{
    if(wave == NULL)
    {
        return -1;
    }

    ces_para_t *para = (ces_para_t*)wave->para;

    *value = para->current;

    return 0;
}

int32_t ces_set_width(waveform_t* wave, float value)
{
    if (wave == NULL)
    {
        return -1;
    }

    ces_para_t* para = (ces_para_t*)wave->para;

    para->width = value;

    return 0;
}

int32_t ces_get_width(waveform_t* wave, float* value)
{
    if (wave == NULL)
    {
        return -1;
    }

    ces_para_t* para = (ces_para_t*)wave->para;

    *value = para->width;

    return 0;
}

int32_t ces_set_freq(waveform_t* wave, float value)
{
    if (wave == NULL)
    {
        return -1;
    }

    ces_para_t* para = (ces_para_t*)wave->para;

    para->freq = value;

    return 0;
}

int32_t ces_get_freq(waveform_t* wave, float* value)
{
    if (wave == NULL)
    {
        return -1;
    }

    ces_para_t* para = (ces_para_t*)wave->para;

    *value = para->freq;

    return 0;
}
