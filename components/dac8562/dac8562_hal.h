#ifndef __dac8562_HAL_H
#define __dac8562_HAL_H

#include <stdint.h>




#ifdef __cplusplus
extern "C" {
#endif

int32_t dac8562_hal_init(void);
int32_t dac8562_hal_write_bytes(uint8_t *w_data, uint16_t length);



#ifdef __cplusplus
}
#endif

#endif
