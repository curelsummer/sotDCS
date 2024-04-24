#include <stdint.h>
#include <stdlib.h>

#include <math.h>

#include "tacs.h"
#include "seq_common.h"

#include "tacs_para_table.c"

static seq_value_segment_t* generate_value_seg(waveform_t *wave, uint32_t *tb);


sequence_t* tacs_generate_secquence(waveform_t *wave)
{
    int32_t err = 0;

    if(wave == NULL)
    {
        err = -1;
        return NULL;
    }

    sequence_t *seq = (sequence_t*)calloc(1, sizeof(sequence_t));
    if(seq == NULL)
    {
        err = -2;
        goto exit;
    }

    seq->wave = wave;

    uint32_t time_base;
    seq->value_seg = generate_value_seg(wave, &time_base);
    if(seq->value_seg == NULL)
    {
        err = -4;
        goto exit;
    }

    seq->time_base_us = time_base;
    
    seq->state_seg = gen_state_seg(wave, seq->time_base_us);
    if(seq->state_seg == NULL)
    {
        err = -3;
        goto exit;
    }
    return seq;

exit:
    release_seq(seq);
    return NULL;
}

static seq_value_segment_t* generate_value_seg(waveform_t *wave, uint32_t *tb)
{   
    int32_t err = 0;

    if(wave == NULL)
    {
        err = -1;
        return NULL;
    }

    float current;
    if(tacs_get_current(wave, &current) < 0)
    {
        err = -2;
        return NULL;
    }

    float freq;
    if(tacs_get_freq(wave, &freq) < 0)
    {
        err = -2;
        return NULL;
    }

    float offset;
    if(tacs_get_offset(wave, &offset) < 0)
    {
        err = -2;
        return NULL;
    }

    uint32_t index = roundu(100*(freq-0.05));

    *tb = time_base[index];
    
    seq_value_segment_t *seg = (seq_value_segment_t*)calloc(1, sizeof(seq_value_segment_t));
    if(seg == NULL)
    {
        err = -3;
        goto exit;
    }

    seg->pn = point_num[index];
    seg->points = (seq_value_point_t*)calloc(seg->pn, sizeof(seq_value_point_t));
    if(seg->points == NULL)
    {
        err = -4;
        goto exit;
    }

    seq_value_point_t *point_ptr = seg->points;

    for(uint32_t i=0; i<seg->pn; i++)
    {
        point_ptr->value = (int32_t)((current + offset) * sin(2 * 3.14159265 * i / seg->pn) + current);
        // point_ptr->time = roundu(US_TO_NTB(1000000 / freq / seg->pn));
        point_ptr->time = 1;
        point_ptr++;
    }

    return seg;

exit:
    release_value_segment(seg);
    return NULL;
}
