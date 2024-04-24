#ifndef __STI_COMMON_H
#define __STI_COMMON_H

#include <stdint.h>
#include "e_pres.h"
#include "seq.h"
#include "cur_output.h"

typedef enum _sti_status_t
{
    STI_STATUS_ERR = -1,
    STI_STATUS_IDLE = 0,
    STI_STATUS_STARTING,
    STI_STATUS_BUSY,
    STI_STATUS_ABORTING,
    STI_STATUS_COMPLETE
}sti_status_t;

typedef struct _sti_port_ctx_t
{
    cur_port_t port;
    sequence_t *seq;

    char *wave_name;

    float fade_in_scale_step;
    float fade_out_scale_step;

    volatile sti_status_t status;
    volatile uint32_t kill_flag;
    volatile uint32_t update_flag;

    volatile uint32_t total_tb_cnt;  //time base counter
    volatile uint32_t total_tb_cnt_end;

    volatile uint32_t value_tb_cnt;
    volatile uint32_t value_tb_cnt_end;

    volatile uint32_t state_tb_cnt;
    volatile uint32_t state_tb_cnt_end;

    volatile uint32_t value_pn_cnt; //value point number counter
    volatile uint32_t state_pn_cnt;
    uint32_t value_pn_cnt_end;
    uint32_t state_pn_cnt_end;
    volatile seq_value_point_t *value_head;
    volatile seq_state_point_t *state_head;
    volatile seq_value_point_t *value_ptr;
    volatile seq_state_point_t *state_ptr;

    volatile int32_t dark_cur;
    volatile int32_t cur_value;
    volatile int32_t cur_state;
    volatile float cur_scale_step;
    volatile float cur_scale;

}sti_port_ctx_t;


typedef struct _sti_context_t
{
    e_pres_t *e_pres;
    uint32_t port_num;
    sti_port_ctx_t *ports[MAX_CUR_PORT_NUM];
}sti_context_t;


#ifdef __cplusplus
 extern "C" {
#endif 

sti_context_t* generate_context(e_pres_t *e_pres);
void release_context(sti_context_t *ctx);


#ifdef __cplusplus
}
#endif

#endif /* __STI_COMMON_H */
