#ifndef __CES_H
#define __CES_H

#include <stdint.h>
#include "wave_common.h"


#ifdef __cplusplus
 extern "C" {
#endif

waveform_t* ces_generate_waveform(float fi, float dur, float fo, float cur, float wid, float freq, wave_true_sham_t ts);
int32_t ces_set_current(waveform_t* wave, float value);
int32_t ces_get_current(waveform_t* wave, float* value);
int32_t ces_set_width(waveform_t* wave, float value);
int32_t ces_get_width(waveform_t* wave, float* value);
int32_t ces_set_freq(waveform_t* wave, float value);
int32_t ces_get_freq(waveform_t* wave, float* value);

#ifdef __cplusplus
}
#endif

#endif /* __CES_H */
