#ifndef __CALIBRATION_H
#define __CALIBRATION_H

#include <stdint.h>
#include "cur_output.h"


#ifdef __cplusplus
 extern "C" {
#endif 

int32_t cal_start(void);
int32_t load_cali_cur_para(cali_cur_para_t *para);

#ifdef __cplusplus
}
#endif

#endif /* __CALIBRATION_H */
