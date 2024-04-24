#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "rtacs.h"
#include "seq_common.h"


static seq_value_segment_t* generate_value_seg(waveform_t *wave, uint32_t tb_us);


sequence_t* rtacs_generate_secquence(waveform_t *wave)
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
    if(rtacs_get_current(wave, &current) < 0)
    {
        err = -2;
        return NULL;
    }

    float start_freq;
    if(rtacs_get_start_freq(wave, &start_freq) < 0)
    {
        err = -2;
        return NULL;
    }

    float end_freq;
    if(rtacs_get_end_freq(wave, &end_freq) < 0)
    {
        err = -2;
        return NULL;
    }

    float period;
    if(rtacs_get_period(wave, &period) < 0)
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

    float total_time;
    uint32_t total_time_i;
    float freq;
    float fp_dur;
    float kf = (end_freq - start_freq) / period;

    freq = start_freq;
    fp_dur = 1/(2*freq);
    total_time = fp_dur;
    total_time_i = roundu(US_TO_NTB(1000000 * fp_dur, tb_us));

    uint32_t i;
    // for(i=1; i<5000; i++)
    // {
    //     freq = freq + kf * fp_dur;
    //     fp_dur = 1 / (2 * freq);

    //     if(total_time + fp_dur > period)
    //     {
    //         break;
    //     }
    //     else
    //     {
    //         total_time += fp_dur;
    //         total_time_i += roundu(US_TO_NTB(1000000 * fp_dur));
    //     }
    //     //printf("freq[%u]: %f, fp_dur[%u]: %f, total: %f, total_i: %u\n", i, freq, i, fp_dur, total_time, total_time_i);
    // }
    
    for(i=1;; i++)
    {
        freq = freq + kf * fp_dur;
        fp_dur = 1 / (2 * freq);

        if(total_time + fp_dur > period)
        {
            break;
        }
        else
        {
            total_time += fp_dur;
            total_time_i += roundu(US_TO_NTB(1000000 * fp_dur, tb_us));
        }
        //printf("freq[%u]: %f, fp_dur[%u]: %f, total: %f, total_i: %u\n", i, freq, i, fp_dur, total_time, total_time_i);
    }

    if(i > 5000)
    {
        printf("space is not enough %u.\n", i);
        return NULL;
    }

    printf("point number %u.\n", i);

    seg->pn = i;
    seg->points = (seq_value_point_t*)calloc(seg->pn, sizeof(seq_value_point_t));
    if(seg->points == NULL)
    {
        free(seg);
        return NULL;
    }


    seq_value_point_t *point_ptr = seg->points;
    int32_t value = (int32_t)current;

    freq = start_freq;
    fp_dur = 1/(2*freq);
    point_ptr->value = value;
    point_ptr->time = roundu(US_TO_NTB(1000000 * fp_dur, tb_us));
    point_ptr++;

    for(i=1; i<seg->pn; i++)
    {
        freq = freq + kf * fp_dur;
        fp_dur = 1 / (2 * freq);

        point_ptr->value = ((i & 0x01) == 0) ? value : (0-value);
        point_ptr->time = roundu(US_TO_NTB(1000000 * fp_dur, tb_us));

        point_ptr++;
    }

    point_ptr--;
    point_ptr->time = roundu(US_TO_NTB(1000000 * (fp_dur + period - total_time), tb_us));

    // ESP_LOGI(TAG, "freq[%u]: %f, fp_dur[%u]: %f, total: %f, total_i: %u\n", i, freq, i, fp_dur, total_time, total_time_i);
    return seg;

exit:
    release_value_segment(seg);
    return NULL;
}

//
// rtacs wave: __________`````````________```````______`````____```__`_
// static seq_segment_t* gen_sti_value(float cur, float fi, float dur, float fo)
// {
//     seq_segment_t *seg = (seq_segment_t*)calloc(1, sizeof(seq_segment_t));
//     if(seg == NULL)
//     {
//         return NULL;
//     }

//     float kf = (end_freq - start_freq) / period; // Hz/s

    
//     uint32_t max = 100000;
//     uint32_t mid = 50000;

//     float  freq_k = max * NTB_TO_US(1) / 1000000;
    

//     float freq = start_freq;
//     float freq_step = kf * NTB_TO_US(1) / 1000000;

//     uint32_t inc_cnt = 0;
//     uint32_t i = 0;
//     float total_time = 0;

//     float value = 0;
//     float time = 0;
    
//     while(1)
//     {
//         if(inc_cnt < mid)
//         {
//             if(value != cur)
//             {
//                 i++;
//                 printf("pn:%u, freq:%f, total_time: %f\n", i, freq, total_time);
//             }
//             value = cur;
//         }
//         else
//         {
//             if(value != (0 - cur))
//             {
//                 i++;
//                 printf("pn:%u, freq:%f, total_time: %f\n", i, freq, total_time);
//             }

//             value = 0 - cur;
//         }

//         total_time += NTB_TO_US(1) / 1000000;

//         inc_cnt += freq * freq_k;
//         inc_cnt %= max;

//         freq += freq_step;

//         if(total_time >= period)
//         {
//             break;
//         }
//     }

//     // printf("pn:%u, freq:%f\n", i, freq);



//     return NULL;


//     // seg->pn = i;
//     // seg->points = (seq_point_t*)calloc(seg->pn, sizeof(seq_point_t));



//     // return seg;
// }