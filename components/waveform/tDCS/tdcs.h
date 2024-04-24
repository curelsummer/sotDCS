#ifndef __TDCS_H
#define __TDCS_H

#include <stdint.h>
#include "wave_common.h"


#ifdef __cplusplus
 extern "C" {
#endif

waveform_t* tdcs_generate_waveform(float fi, float dur, float fo, float cur, wave_true_sham_t ts);
int32_t tdcs_set_current(waveform_t* wave, float value);
int32_t tdcs_get_current(waveform_t* wave, float *value);



#ifdef __cplusplus
}
#endif

#endif /* __WAVE_TDCS_H */
