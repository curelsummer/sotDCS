#ifndef __SEQ_COMMON_H
#define __SEQ_COMMON_H

#include <stdint.h>
#include "wave_common.h"

#define SEQ_DATA_MODE_0 (0)  //int32 uA
#define SEQ_DATA_MODE_1 (1)  //int32 DAC CODE
#define SEQ_DATA_MODE SEQ_DATA_MODE_0

#define SEQ_TIME_MODE_0 (0)  //int32 us
#define SEQ_TIME_MODE_1 (1)  //int32 beats
#define SEQ_TIME_MODE SEQ_TIME_MODE_1


#if SEQ_DATA_MODE == SEQ_DATA_MODE_1
    #include "cur_output.h"
#endif

#if SEQ_TIME_MODE == SEQ_TIME_MODE_1
    #define TIME_BASE_US (50.0)
    #define SEC_TO_NTB(sec, tb_us) ((sec * 1000000) / tb_us)
    #define US_TO_NTB(us, tb_us) ((us) / tb_us)
    #define NTB_TO_US(ntb, tb_us) ((ntb) * tb_us)
    #define roundu(n) ((uint32_t)(n + 0.5))
    #define roundi(n) ((int32_t)(n + 0.5))
#endif


//WAVE STATE
#define SEQ_STATE_ERR   (-1)
#define SEQ_STATE_NORMAL (0)
#define SEQ_STATE_FADE_IN (1)
#define SEQ_STATE_FADE_OUT (2)
#define SEQ_STATE_SHAM_FI (3)
#define SEQ_STATE_SHAM_FO (4)
#define SEQ_STATE_SHAM_DELAY (5)
#define SEQ_STATE_SHAM_DUR (6)



// #define MAX_SEQ_NAME_SIZE (32) //including '\0'

//sequence value point
typedef struct _seq_value_point_t
{
    int32_t value;
    uint32_t time;
}seq_value_point_t;


typedef struct _seq_value_segment_t
{
    uint32_t pn;
    seq_value_point_t *points;
}seq_value_segment_t;


//sequence state point
typedef struct _seq_state_point_t
{
    int32_t state;
    uint32_t time;
}seq_state_point_t;


typedef struct _seq_state_segment_t
{
    uint32_t pn;
    seq_state_point_t *points;
}seq_state_segment_t;


//sequence
typedef struct _sequence_t
{
    waveform_t *wave;
    uint32_t time_base_us;
    seq_value_segment_t *value_seg;
    seq_state_segment_t *state_seg;
}sequence_t;

#ifdef __cplusplus
 extern "C" {
#endif 

seq_state_segment_t* gen_state_seg(waveform_t *wave, uint32_t tb_us);

int32_t seq_get_value_pn(sequence_t *seq, uint32_t *pn);
int32_t seq_get_state_pn(sequence_t *seq, uint32_t *pn);
// int32_t seq_get_total_time(sequence_t *seq, uint32_t *time);

seq_value_point_t* seq_get_value_head(sequence_t *seq);
seq_state_point_t* seq_get_state_head(sequence_t *seq);

void release_seq(sequence_t *seq);
void release_value_segment(seq_value_segment_t *seg);
void release_state_segment(seq_state_segment_t *seg);

void print_seq(sequence_t *seq);
#ifdef __cplusplus
}
#endif

#endif /* __SEQ_COMMON_H */
