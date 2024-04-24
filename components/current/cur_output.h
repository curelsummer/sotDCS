#ifndef __CUR_OUTPUT_H
#define __CUR_OUTPUT_H

#include <stdint.h>

typedef enum _cur_port_t
{
    CUR_PORT_ERR = -1,
    CUR_PORT_0 = 0
}cur_port_t;


typedef struct _cali_cur_para_t
{
    float a0;
    float a1;
}cali_cur_para_t;


#define MAX_CUR_PORT_NUM (1)

#define UA_TO_CODE(n) ((n) * 6.5536)

#ifdef __cplusplus
 extern "C" {
#endif 


int32_t cur_init(void);
int32_t cur_open(uint32_t timeout_ms);
int32_t cur_close(void);


int32_t cur_set_code(int32_t code);
int32_t cur_set_ua(float ua, uint8_t cali_flag);

int32_t cur_power_on(void);
int32_t cur_power_off(void);

int32_t cur_enable_output(void);
int32_t cur_disable_output(void);

int32_t cur_enable_output_relay(void);
int32_t cur_disable_output_relay(void);

int32_t cur_convert_ua_to_code(float ua);

cur_port_t cur_get_port(uint32_t port);

#ifdef __cplusplus
}
#endif



#endif /* __CUR_OUTPUT_H */

