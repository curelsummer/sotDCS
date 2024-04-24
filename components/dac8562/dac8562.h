#ifndef dac8562_H
#define dac8562_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void dac8562_init(void);
void dac8562_output_b(uint16_t data);




#ifdef __cplusplus
}
#endif

#endif/*dac8562_H*/
