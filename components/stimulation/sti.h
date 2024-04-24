#ifndef __STI_H_
#define __STI_H_

#include <stdint.h>

#include "sti_common.h"
#include "e_pres.h"


typedef void (*sti_status_cb_t)(sti_status_t);


#ifdef __cplusplus
 extern "C" {
#endif 

int32_t sti_start(e_pres_t *e_pres, sti_status_cb_t cb);
int32_t sti_stop(void);

int32_t sti_get_cntdown_time_sec(uint32_t *sec);
int32_t sti_get_running_time_sec(uint32_t *sec);

int32_t sti_get_state(void);

sti_status_t sti_get_running_status(void);
sti_context_t* sti_get_active_ctx(void);
char *sti_get_name(void);

#ifdef __cplusplus
}
#endif

#endif /* __STI_H_ */

// sti_ll 及以下部分封装为 电流输出模块，系统启动时单独初始化该模块，为模块添加互斥锁，电流输出时由sti独占，用完释放，可以供校准使用。
// calibration 读写方式占用电流输出模块，只读方式占用monitor模块，将校准后的值存到NVS中
// calibration 校准电流输出模块时，此时独享该模块，存入NVS后马上刷新即可，但校准monitor 电池采样分压比率时应以读写方式占用monitor，更改校准值后释放。

// 以模块化型式写驱动，资源概念，rw权限，添加互斥锁，一切皆文件，底层不用锁，在资源层面添加。
