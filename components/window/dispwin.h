#ifndef __WINDOWS_H
#define __WINDOWS_H

#include <stdint.h>

#include "win_basic.h"

// #include "e_pres.h"
// #include "waveform.h"


#ifdef __cplusplus
 extern "C" {
#endif

void dispwin(void *arg);

int32_t disp_win_tdcs(void);
int32_t disp_win_tacs(void);
int32_t disp_win_ces(void);
int32_t disp_win_rtacs(void);

int32_t disp_win_home_tdcs(void);
int32_t disp_win_home_ces(void);

int32_t disp_win_debug(void);

#ifdef __cplusplus
}
#endif

#endif /* __WINDOWS_H */
