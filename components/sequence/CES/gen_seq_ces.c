#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "wave_ces.h"
#include "gen_seq.h"


#define roundi(n) ((int32_t)((n) + 0.5))
#define roundu(n) ((uint32_t)((n) + 0.5))

static seq_segment_t* gen_sti_value(float cur, float fi, float dur, float fo);
static seq_segment_t* gen_true_sti_scale(float fi, float dur, float fo, uint32_t ramp_freq);
static seq_segment_t* gen_false_sti_scale(float fi, float dur, float fo, uint32_t ramp_freq);


//"CONVERT_FACTOR" which define by hardware, is used to convert mA to DAC_CODE. eg. 1mA * CONVERT_FACTOR = DAC_CODE (1mA)
//"scale" adjust the output amplitude. This mechanism is used to produce fade in, fade out and sham control waveform.
sequence_t* gen_seq_ces(waveform_t *wave, int32_t ts)
{
    if(wave == NULL)
    {
        return NULL;
    }

//get paras
    int32_t err;

    float current;
    float fade_in;
    float duration;
    float fade_out;

    err = wave_get_ces_cur(wave, &current);
    if(err < 0)
    {
        return NULL;
    }

    err = wave_get_ces_fade_in(wave, &fade_in);
    if(err < 0)
    {
        return NULL;
    }

    err = wave_get_ces_duration(wave, &duration);
    if(err < 0)
    {
        return NULL;
    }

    err = wave_get_ces_fade_out(wave, &fade_out);
    if(err < 0)
    {
        return NULL;
    }

    sequence_t *seq = (sequence_t*)calloc(1, sizeof(sequence_t));
    if(seq == NULL)
    {
        return NULL;
    }

//load value sequence
    seq->value = gen_sti_value(current, fade_in, duration, fade_out);
    if(seq->value == NULL)
    {
        goto exit;
    }


//load scale sequence   
    // 100 points per second for fade in/out.
    uint32_t ramp_freq = 100; 

    if(ts == 0)
    {//sham stimulation  __/``\_________/``\__
        seq->scale = gen_false_sti_scale(fade_in, duration, fade_out, ramp_freq);
        if(seq->scale == NULL)
        {
            goto exit;
        }
    }
    else
    {//true stimulation __/```````````````\__
        seq->scale = gen_true_sti_scale(fade_in, duration, fade_out, ramp_freq);
        if(seq->scale == NULL)
        {
            goto exit;
        }
    }
    

    return seq;

exit:
    release_seq(seq);
    free(seq);
    return NULL;
}

//
// ces wave: ----_---``--___-````____-```--__---`----
static seq_segment_t* gen_sti_value(float cur, float fi, float dur, float fo)
{
    seq_segment_t *seg = (seq_segment_t*)calloc(1, sizeof(seq_segment_t));
    if(seg == NULL)
    {
        return NULL;
    }

    seg->pn = 15;
    seg->points = (seq_point_t*)calloc(seg->pn, sizeof(seq_point_t));
    if(seg->points == NULL)
    {
        free(seg);
        return NULL;
    }

    seq_point_t *point_ptr = seg->points;

    int32_t value = (int32_t)cur;

    point_ptr->value.i32 = 0;
    point_ptr->time = roundu(US_TO_NTB(1 * 1000000));
    point_ptr++;
    
    point_ptr->value.i32 = 0 - value;
    point_ptr->time = roundu(US_TO_NTB(0.25 * 1000000));
    point_ptr++;

    point_ptr->value.i32 = 0;
    point_ptr->time = roundu(US_TO_NTB(0.75 * 1000000));
    point_ptr++;
    
    point_ptr->value.i32 = value;
    point_ptr->time = roundu(US_TO_NTB(0.5 * 1000000));
    point_ptr++;
    
    point_ptr->value.i32 = 0;
    point_ptr->time = roundu(US_TO_NTB(0.5 * 1000000));
    point_ptr++;
    
    point_ptr->value.i32 = 0 - value;
    point_ptr->time = roundu(US_TO_NTB(0.75 * 1000000));
    point_ptr++;
    
    point_ptr->value.i32 = 0;
    point_ptr->time = roundu(US_TO_NTB(0.25 * 1000000));
    point_ptr++;
    
    point_ptr->value.i32 = value;
    point_ptr->time = roundu(US_TO_NTB(1 * 1000000));
    point_ptr++;
    
    point_ptr->value.i32 = 0 - value;
    point_ptr->time = roundu(US_TO_NTB(1 * 1000000));
    point_ptr++;
    
    point_ptr->value.i32 = 0;
    point_ptr->time = roundu(US_TO_NTB(0.25 * 1000000));
    point_ptr++;
    
    point_ptr->value.i32 = value;
    point_ptr->time = roundu(US_TO_NTB(0.75 * 1000000));
    point_ptr++;
    
    point_ptr->value.i32 = 0;
    point_ptr->time = roundu(US_TO_NTB(0.5 * 1000000));
    point_ptr++;
    
    point_ptr->value.i32 = 0 - value;
    point_ptr->time = roundu(US_TO_NTB(0.5 * 1000000));
    point_ptr++;
    
    point_ptr->value.i32 = 0;
    point_ptr->time = roundu(US_TO_NTB(0.75 * 1000000));
    point_ptr++;
    
    point_ptr->value.i32 = value;
    point_ptr->time = roundu(US_TO_NTB(0.25 * 1000000));

    return seg;
}

//true stimulation __/```````````````\__
static seq_segment_t* gen_true_sti_scale(float fi, float dur, float fo, uint32_t ramp_freq)
{   
    seq_segment_t *seg = (seq_segment_t*)calloc(1, sizeof(seq_segment_t));
    if(seg == NULL)
    {
        return NULL;
    }

    //calculate fade in/out point number
    uint32_t fi_pn = roundu(fi * ramp_freq);
    uint32_t fo_pn = roundu(fo * ramp_freq);

    seg->pn = fi_pn + 1 + fo_pn;
    seg->points = (seq_point_t*)calloc(seg->pn, sizeof(seq_point_t));
    if(seg->points == NULL)
    {
        free(seg);
        return NULL;
    }


    seq_point_t *point_ptr = seg->points;

    //fade in
    float scale_step = 1.0/fi_pn;
    float scale_value = 0;
    uint32_t interval_time = roundu(US_TO_NTB(1000000 / ramp_freq));

    uint32_t i;
    for(i=0; i<fi_pn; i++)
    {   
        
        point_ptr->value.f32 = scale_value;
        point_ptr->time = interval_time;

        point_ptr++;
        scale_value += scale_step;
    }
    
    //duration
    point_ptr->value.f32 = scale_value;
    point_ptr->time = roundu(US_TO_NTB(dur * 1000000));
    point_ptr++;

    //fade out
    scale_step = 1.0/fo_pn;

    for(i=0; i<fo_pn; i++)
    {
        scale_value -= scale_step;

        point_ptr->value.f32 = scale_value;
        point_ptr->time = interval_time;

        point_ptr++;
    }

    return seg;
}


//sham stimulation  __/``\_________/``\__
static seq_segment_t* gen_false_sti_scale(float fi, float dur, float fo, uint32_t ramp_freq)
{   
    seq_segment_t *seg = (seq_segment_t*)calloc(1, sizeof(seq_segment_t));
    if(seg == NULL)
    {
        return NULL;
    }

    //calculate fade point number
    uint32_t fi_pn = roundu(fi * ramp_freq);
    uint32_t fo_pn = roundu(fo * ramp_freq);

    //sham stimulation internel fade in/out time is fixed to 15s
    //sham stimulatino internel duration time is fixed to 5s
    float sham_fade_in = 15;//
    float sham_duration = 5;
    float sham_fade_out = 15;//

    uint32_t sham_fi_pn = roundu(sham_fade_in * ramp_freq);
    uint32_t sham_fo_pn = roundu(sham_fade_out * ramp_freq);
    uint32_t sham_pn = 1 + sham_fo_pn + 1 + sham_fi_pn + 1; 

    seg->pn = fi_pn + sham_pn + fo_pn;
    seg->points = (seq_point_t*)calloc(seg->pn, sizeof(seq_point_t));
    if(seg->points == NULL)
    {
        free(seg);
        return NULL;
    }

    seq_point_t *point_ptr = seg->points;

    //fade in
    float scale_step = 1.0/fi_pn;
    float scale_value = 0;
    uint32_t interval_time = roundu(US_TO_NTB(1000000 / ramp_freq));

    uint32_t i;
    for(i=0; i<fi_pn; i++)
    {   
        point_ptr->value.f32 = scale_value;
        point_ptr->time = interval_time;

        point_ptr++;
        scale_value += scale_step;
    }
    
    //sham duration
    point_ptr->value.f32 = scale_value;
    point_ptr->time = roundu(US_TO_NTB(sham_duration * 1000000));
    point_ptr++;

    //sham fade out
    scale_step = 1.0/sham_fo_pn;

    for(i=0; i<sham_fo_pn; i++)
    {
        scale_value -= scale_step;

        point_ptr->value.f32 = scale_value;
        point_ptr->time = interval_time;

        point_ptr++;
    }

    //sham zero
    float sham_zero_time = dur - sham_duration - sham_fade_out - sham_fade_in - sham_duration;
    if(sham_zero_time < 0)
    {
        sham_zero_time = 1;
    }

    point_ptr->value.f32 = scale_value;
    point_ptr->time = roundu(US_TO_NTB(sham_zero_time * 1000000));
    point_ptr++;

    //sham fade in
    scale_step = 1.0/sham_fi_pn;
    for(i=0; i<sham_fi_pn; i++)
    {
        point_ptr->value.f32 = scale_value;
        point_ptr->time = interval_time;

        point_ptr++;
        scale_value += scale_step;
    }

    //sham duration
    point_ptr->value.f32 = scale_value;
    point_ptr->time = roundu(US_TO_NTB(sham_duration * 1000000));
    point_ptr++;

    //fade out
    scale_step = 1.0/fo_pn;

    for(i=0; i<fo_pn; i++)
    {
        scale_value -= scale_step;

        point_ptr->value.f32 = scale_value;
        point_ptr->time = interval_time;

        point_ptr++;
    }

    return seg;
}