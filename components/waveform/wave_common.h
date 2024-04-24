#ifndef __WAVE_COMMON_H
#define __WAVE_COMMON_H


#include <stdint.h>

#define MAX_WAVE_NAME_SIZE (32)


#define TDCS_WAVE_ID (1)
#define TDCS_WAVE_NAME "直流模式"

#define TACS_WAVE_ID (2)
#define TACS_WAVE_NAME "交流模式"

#define CES_WAVE_ID (3)
#define CES_WAVE_NAME "微电流模式"

#define RTACS_WAVE_ID (4)
#define RTACS_WAVE_NAME "频率扫描模式"

typedef enum _wave_true_sham_t
{
    WAVE_SHAM = 0,
    WAVE_TRUE = 1
}wave_true_sham_t;


// typedef struct 
typedef struct _waveform_t
{
    uint32_t id;
    char name[MAX_WAVE_NAME_SIZE];
    wave_true_sham_t ts;
    float fade_in;
    float duration;
    float fade_out;
    void *para;
}waveform_t;


#ifdef __cplusplus
 extern "C" {
#endif


void release_waveform(waveform_t *wave);

int32_t wave_set_fade_in(waveform_t *wave, float value);
int32_t wave_set_fade_out(waveform_t *wave, float value);
int32_t wave_set_duration(waveform_t *wave, float value);
int32_t wave_set_ts(waveform_t *wave, wave_true_sham_t ts);

int32_t wave_get_fade_in(waveform_t *wave, float *value);
int32_t wave_get_fade_out(waveform_t *wave, float *value);
int32_t wave_get_duration(waveform_t *wave, float *value);
int32_t wave_get_ts(waveform_t *wave, wave_true_sham_t *ts);

int32_t wave_get_total_time(waveform_t *wave, float *time);

char *wave_get_name(waveform_t *wave);

#ifdef __cplusplus
}
#endif

#endif /* __WAVECOMMON_H */