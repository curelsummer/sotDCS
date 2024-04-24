#include <stdio.h>
#include <string.h>
#include "e_pres.h"
#include "waveform.h"

int32_t cfg_init_single_stimulation(sti_config_t *cfg)
{
    if(cfg == NULL)
    {
        return -1;
    }

    memset(cfg, 0, sizeof(sti_config_t));

    strcpy(cfg->name, "null");

    cfg->sti_mode = ST_MODE_SINGLE;
    cfg->wave_num = 1;
    
    return 0;
}

int32_t cfg_load_waveform(sti_config_t *cfg, waveform_t *wave)
{
    if(cfg == NULL)
    {
        return -1;
    }

    cfg->waves = wave;

    return 0;
}

float cfg_get_total_time(sti_config_t *cfg)
{
    if(cfg == NULL)
    {
        return -1;
    }

    float max_time = 0;
    waveform_t *wave = cfg->waves;
    for(uint32_t i=0; i<cfg->wave_num; i++)
    {
      float time;
      if(0 == wave_get_total_time(wave, &time))
      {
          max_time = (time > max_time) ? time : max_time;
      }
      wave++;
    }

    return max_time;
}

char* cfg_get_name(sti_config_t *cfg)
{
    if(cfg == NULL)
    {
        return NULL;
    }

    return cfg->name;
}