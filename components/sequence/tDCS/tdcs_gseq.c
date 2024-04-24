#include <stdint.h>
#include <stdlib.h>

#include "tdcs.h"
#include "seq_common.h"


static seq_value_segment_t* generate_value_seg(waveform_t *wave, uint32_t tb_us);


sequence_t* tdcs_generate_secquence(waveform_t *wave)
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

    seq->time_base_us = 100;
    seq->wave = wave;
    seq->state_seg = gen_state_seg(wave, seq->time_base_us);
    if(seq->state_seg == NULL)
    {
        err = -3;
        goto exit;
    }

    seq->value_seg = generate_value_seg(wave, seq->time_base_us);
    if(seq->value_seg == NULL)
    {
        err = -4;
        goto exit;
    }

    return seq;

exit:
    release_seq(seq);
    return NULL;
}

static seq_value_segment_t* generate_value_seg(waveform_t *wave, uint32_t tb_us)
{   
    int32_t err = 0;

    if(wave == NULL)
    {
        err = -1;
        return NULL;
    }

    float current;
    if(tdcs_get_current(wave, &current) < 0)
    {
        err = -2;
        return NULL;
    }
    
    seq_value_segment_t *seg = (seq_value_segment_t*)calloc(1, sizeof(seq_value_segment_t));
    if(seg == NULL)
    {
        err = -3;
        goto exit;
    }

    seg->pn = 1;

    seg->points = (seq_value_point_t*)calloc(seg->pn, sizeof(seq_value_point_t));
    if(seg->points == NULL)
    {
        err = -4;
        goto exit;
    }

    seg->points->value = (int32_t)current;
    seg->points->time = UINT32_MAX;

    return seg;

exit:
    release_value_segment(seg);
    return NULL;
}