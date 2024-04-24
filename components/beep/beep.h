#ifndef BEEP_H
#define BEEP_H



#ifdef __cplusplus
extern "C" {
#endif

#include "../includes_common/includes_common.h"



#define Buz_ctl_IO          10
#define Buz_ctl_IO_SEL      (1ULL<<Buz_ctl_IO)

void beep_init(void);
void beep_start(uint32_t ms, uint32_t n);

#ifdef __cplusplus
}
#endif

#endif/*BEEP_H*/