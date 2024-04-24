#ifndef __RTACS_H
#define __RTACS_H

#include <stdint.h>
#include "wave_common.h"



#ifdef __cplusplus
 extern "C" {
#endif 

waveform_t* rtacs_generate_waveform(float fi, float dur, float fo, float current, float start_freq, float end_freq, float period, wave_true_sham_t ts);

int32_t rtacs_set_current(waveform_t *wave, float value);
int32_t rtacs_set_start_freq(waveform_t *wave, float value);
int32_t rtacs_set_end_freq(waveform_t *wave, float value);
int32_t rtacs_set_period(waveform_t *wave, float value);

int32_t rtacs_get_current(waveform_t *wave, float *value);
int32_t rtacs_get_start_freq(waveform_t *wave, float *value);
int32_t rtacs_get_end_freq(waveform_t *wave, float *value);
int32_t rtacs_get_period(waveform_t *wave, float *value);


#ifdef __cplusplus
}
#endif

#endif /* __RTACS_H */