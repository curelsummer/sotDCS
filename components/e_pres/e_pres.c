#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "e_pres.h"

static void release_e_pres(e_pres_t *e_pres);


e_pres_t *epres_generate_single_mode(char *name, waveform_t *wave)
{
    int32_t err = 0;
    if(name == NULL)
    {
        err = -1;
        return NULL;
    }

    e_pres_t *e_pres = (e_pres_t*)calloc(1, sizeof(e_pres_t));
    if(e_pres == NULL)
    {
        err = -2;
        goto exit;
    }

    //name
    epres_set_name(e_pres, name);

    //mode
    e_pres->sti_mode = ST_MODE_SINGLE;
    e_pres->wave_num = 1;

    //wave
    if(wave != NULL)
    {
        e_pres->waves[0] = wave;
    }
    
    return e_pres;

exit:
    release_e_pres(e_pres);
    return NULL;
}

int32_t epres_set_name(e_pres_t *e_pres, char *name)
{
    if((e_pres == NULL) || (name == NULL))
    {
        return -1;
    }

    char n[MAX_E_PRES_NAME_SIZE];
    memcpy(n, name, MAX_E_PRES_NAME_SIZE);
    n[MAX_E_PRES_NAME_SIZE-1] = '\0';
    strcpy(e_pres->name, n);
    
    return 0;
}

const char *epres_get_name(e_pres_t *e_pres)
{
    if(e_pres == NULL)
    {
        return NULL;
    }

    return e_pres->name;
}

int32_t epres_set_wave(e_pres_t *e_pres, uint32_t index, waveform_t *wave)
{
    if((e_pres == NULL) || (wave == NULL))
    {
        return -1;
    }

    if(index >= e_pres->wave_num)
    {
        return -2;
    }

    e_pres->waves[index] = wave;

    return 0;
}

float epres_get_total_time(e_pres_t *e_pres)
{
    if(e_pres == NULL)
    {
        return -1;
    }

    float max_time = -2;
    for(uint32_t i=0; i<e_pres->wave_num; i++)
    {
        float time;
        if( 0 == wave_get_total_time(e_pres->waves[i], &time))
        {
            max_time = (time > max_time) ? time : max_time;
        }
    }

    return max_time;
}


static void release_e_pres(e_pres_t *e_pres)
{
    if(e_pres == NULL)
    {
        return;
    }

    free(e_pres);
}