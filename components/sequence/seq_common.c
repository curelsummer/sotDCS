#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "seq_common.h"

static seq_state_segment_t* gen_true_state_seg(float fi, float dur, float fo, uint32_t tb_us);
static seq_state_segment_t* gen_false_state_seg(float fi, float dur, float fo, uint32_t tb_us);


seq_state_segment_t* gen_state_seg(waveform_t *wave, uint32_t tb_us)
{
    if(wave == NULL)
    {
        return NULL;
    }

    if(wave->ts == WAVE_SHAM)
    {
        return gen_false_state_seg(wave->fade_in, wave->duration, wave->fade_out, tb_us);
    }
    else
    {
        return gen_true_state_seg(wave->fade_in, wave->duration, wave->fade_out, tb_us);
    }
}


//true stimulation __/```````````````\__
static seq_state_segment_t* gen_true_state_seg(float fi, float dur, float fo, uint32_t tb_us)
{   
    int32_t err = 0;

    seq_state_segment_t *seg = (seq_state_segment_t*)calloc(1, sizeof(seq_state_segment_t));
    if(seg == NULL)
    {
        err = -1;
        goto exit;
    }

    seg->pn = 3; 
    seg->points = (seq_state_point_t*)calloc(seg->pn, sizeof(seq_state_point_t));
    if(seg->points == NULL)
    {
        err = -2;
        goto exit;
    }

    seq_state_point_t *point = seg->points;

    //fade in
    point->state = SEQ_STATE_FADE_IN;
    point->time = roundu(US_TO_NTB(fi * 1000000, tb_us));
    point++;

    //duration
    point->state = SEQ_STATE_NORMAL;
    point->time = roundu(US_TO_NTB(dur * 1000000, tb_us));
    point++;

    //fade out
    point->state = SEQ_STATE_FADE_OUT;
    point->time = roundu(US_TO_NTB(fo * 1000000, tb_us));

    return seg;

exit:
    release_state_segment(seg);
    return NULL;
}


//sham stimulation  __/``\_________/``\__
static seq_state_segment_t* gen_false_state_seg(float fi, float dur, float fo, uint32_t tb_us)
{   
    seq_state_segment_t *seg = (seq_state_segment_t*)calloc(1, sizeof(seq_state_segment_t));
    if(seg == NULL)
    {
        return NULL;
    }

    seg->pn = 7; 
    seg->points = (seq_state_point_t*)calloc(seg->pn, sizeof(seq_state_point_t));
    if(seg->points == NULL)
    {
        free(seg);
        return NULL;
    }

    seq_state_point_t *point_ptr = seg->points;

    //sham stimulation internel fade in/out time is fixed to 15s
    //sham stimulatino internel duration time is fixed to 5s
    float sham_fade_in = fi;//
    float sham_delay = 5;
    float sham_fade_out = fo;//

    //fade in
    point_ptr->state = SEQ_STATE_FADE_IN;
    point_ptr->time = roundu(US_TO_NTB(fi * 1000000, tb_us));
    point_ptr++;
    
    //sham duration
    point_ptr->state = SEQ_STATE_SHAM_DELAY;
    point_ptr->time = roundu(US_TO_NTB(sham_delay * 1000000, tb_us));
    point_ptr++;

    //sham fade out
    point_ptr->state = SEQ_STATE_SHAM_FO;
    point_ptr->time = roundu(US_TO_NTB(sham_fade_out * 1000000, tb_us));
    point_ptr++;

    //sham zero
    float sham_dur = dur - sham_delay - sham_fade_out - sham_fade_in - sham_delay;
    if(sham_dur < 0)
    {
        sham_dur = 1;
    }

    point_ptr->state = SEQ_STATE_SHAM_DUR;
    point_ptr->time = roundu(US_TO_NTB(sham_dur * 1000000, tb_us));
    point_ptr++;

    //sham fade in
    point_ptr->state = SEQ_STATE_SHAM_FI;
    point_ptr->time = roundu(US_TO_NTB(sham_fade_in * 1000000, tb_us));
    point_ptr++;

    //sham duration
    point_ptr->state = SEQ_STATE_SHAM_DELAY;
    point_ptr->time = roundu(US_TO_NTB(sham_delay * 1000000, tb_us));
    point_ptr++;

    //fade out
    point_ptr->state = SEQ_STATE_FADE_OUT;
    point_ptr->time = roundu(US_TO_NTB(fo * 1000000, tb_us));

    return seg;
}

int32_t seq_get_value_pn(sequence_t *seq, uint32_t *pn)
{
    if((seq == NULL) || (pn == NULL))
    {
        return -1;
    }

    if(seq->value_seg == NULL)
    {
        return -2;
    }

    *pn = seq->value_seg->pn;

    return 0;
}

int32_t seq_get_state_pn(sequence_t *seq, uint32_t *pn)
{
    if((seq == NULL) || (pn == NULL))
    {
        return -1;
    }

    if(seq->state_seg == NULL)
    {
        return -2;
    }

    *pn = seq->state_seg->pn;

    return 0;
}

seq_value_point_t* seq_get_value_head(sequence_t *seq)
{
    if(seq == NULL)
    {
        return NULL;
    }

    if(seq->value_seg == NULL)
    {
        return NULL;
    }

    return seq->value_seg->points;
}

seq_state_point_t* seq_get_state_head(sequence_t *seq)
{
    if(seq == NULL)
    {
        return NULL;
    }

    if(seq->state_seg == NULL)
    {
        return NULL;
    }

    return seq->state_seg->points;
}


void print_seq(sequence_t *seq)
{
    if(seq == NULL)
    {
        printf("sequence is empty.\n");
        return;
    }

    printf("\n\nprint sequence(0x%08x)\n\n", (uint32_t)seq);
    if(seq->wave == NULL)
    {
        printf("\tseq->wave: NULL\n");
    }
    else
    {
        printf("\tseq->wave:\n");
        printf("\t\tseq->wave->name: %s\n", wave_get_name(seq->wave));
    }
    
    if(seq->value_seg == NULL)
    {
        printf("\tseq->value_seg: NULL\n");
    }
    else
    {
        printf("\tseq->value_seg:\n");

        printf("\t\tseq->value_seg->pn: %u\n", seq->value_seg->pn);
        if(seq->value_seg->points == NULL)
        {
            printf("\t\tseq->value_seg->points: NULL\n");
        }
        else
        {
            printf("\t\tseq->value_seg->points:\n");
            uint32_t pn = 50;
            pn = (seq->value_seg->pn < pn) ? seq->value_seg->pn : pn;
            for(uint32_t i=0; i<pn; i++)
            {
                printf("\t\t\t%d\t%u\n", seq->value_seg->points[i].value, seq->value_seg->points[i].time);
            }
        }
    }

    if(seq->state_seg == NULL)
    {
        printf("\tseq->state_seg: NULL\n");
    }
    else
    {
        printf("\tseq->state_seg:\n");

        printf("\t\tseq->state_seg->pn: %u\n", seq->state_seg->pn);
        if(seq->state_seg->points == NULL)
        {
            printf("\t\tseq->state_seg->points: NULL\n");
        }
        else
        {
            printf("\t\tseq->state_seg->points:\n");
            uint32_t pn = 100;
            pn = (seq->state_seg->pn < pn) ? seq->state_seg->pn : pn;
            for(uint32_t i=0; i<pn; i++)
            {
                printf("\t\t\t%d\t%u\n", seq->state_seg->points[i].state, seq->state_seg->points[i].time);
            }
        }
    }

}





void release_seq(sequence_t *seq)
{
    if(seq == NULL)
    {
        return;
    }

    if(seq->value_seg != NULL)
    {
        release_value_segment(seq->value_seg);
        seq->value_seg = NULL;
    }

    if(seq->state_seg != NULL)
    {
        release_state_segment(seq->state_seg);
        seq->state_seg = NULL;
    }

    free(seq);
}

void release_value_segment(seq_value_segment_t *seg)
{
    if(seg == NULL)
    {
        return;
    }

    if(seg->points != NULL)
    {
        free(seg->points);
        seg->points = NULL;
    }
    
    free(seg);
}

void release_state_segment(seq_state_segment_t *seg)
{
    if(seg == NULL)
    {
        return;
    }

    if(seg->points != NULL)
    {
        free(seg->points);
        seg->points = NULL;
    }

    free(seg);
}