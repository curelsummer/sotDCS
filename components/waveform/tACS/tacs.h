#ifndef __TACS_H
#define __TACS_H

#include <stdint.h>
#include "wave_common.h"





#ifdef __cplusplus
 extern "C" {
#endif 

waveform_t* tacs_generate_waveform(float fi, float dur, float fo, float cur, float freq, float offset, wave_true_sham_t ts);

int32_t tacs_set_current(waveform_t *wave, float value);
int32_t tacs_set_freq(waveform_t *wave, float value);
int32_t tacs_set_offset(waveform_t *wave, float value);



int32_t tacs_get_current(waveform_t *wave, float *value);
int32_t tacs_get_freq(waveform_t *wave, float *value);
int32_t tacs_get_offset(waveform_t *wave, float *value);



#ifdef __cplusplus
}
#endif

#endif /* __TACS_H */