#ifndef __SYSENV_H
#define __SYSENV_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void sysenv_load(void);

int32_t sysenv_save_device_mode(int32_t mode);
int32_t sysenv_get_device_mode(void);

int32_t sysenv_get_ts(void);
int32_t sysenv_save_ts(int32_t ts);

int32_t sysenv_get_device_id(void);
int32_t sysenv_save_device_id(int32_t id);

const char* sysenv_get_device_name(void);
int32_t sysenv_save_device_name(char *name);

const char* sysenv_get_uid_a(void);



#ifdef __cplusplus
}
#endif

#endif/*__SYSENV_H*/