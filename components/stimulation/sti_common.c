#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sti_common.h"
#include "esp_log.h"





static cur_port_t port_map[] = {CUR_PORT_0};

static sti_port_ctx_t *generate_port_ctx(waveform_t* wave, cur_port_t port);
static void release_port_ctx(sti_port_ctx_t *ctx);


sti_context_t* generate_context(e_pres_t *e_pres)
{
    int32_t err = 0;
    if(e_pres == NULL)
    {
        err = -1;
        return NULL;
    }

    sti_context_t *ctx = (sti_context_t*)calloc(1, sizeof(sti_context_t));
    if(ctx == NULL)
    {
        err = -2;
        goto exit;
    }

    ctx->e_pres = e_pres;
    ctx->port_num = e_pres->wave_num;
    if(ctx->port_num > MAX_CUR_PORT_NUM)
    {
        err = -3;
        goto exit;
    }

    for(uint32_t i=0; i<ctx->port_num; i++)
    {
        ctx->ports[i] = generate_port_ctx(e_pres->waves[i], port_map[i]);
        if(ctx->ports[i] == NULL)
        {
            err = -4;
            goto exit;
        }
    }
    
    return ctx;

exit:
    release_context(ctx);
    return NULL;
}

static sti_port_ctx_t *generate_port_ctx(waveform_t* wave, cur_port_t port)
{
    int32_t err = 0;
    if(wave == NULL)
    {
        err = -1;
        return NULL;
    }

    sti_port_ctx_t *ctx = (sti_port_ctx_t*)calloc(1, sizeof(sti_port_ctx_t));
    if(ctx == NULL)
    {
        err = -2;
        goto exit;
    }

    ctx->port = port;
    ctx->wave_name = wave_get_name(wave);
    ctx->seq = generate_secquence(wave);
    if(ctx->seq == NULL)
    {
        err = -3;
        goto exit;
    }
    //print_seq(ctx->seq);

    float fi;
    if(0 > wave_get_fade_in(wave, &fi))
    {
        err = -4;
        goto exit;
    }
    ctx->fade_in_scale_step = 1.0/fi/US_TO_NTB(1000000, ctx->seq->time_base_us);
    
    float fo;
    if(0 > wave_get_fade_out(wave, &fo))
    {
        err = -5;
        goto exit;
    }
    ctx->fade_out_scale_step = -1.0/fo/US_TO_NTB(1000000, ctx->seq->time_base_us);

    ctx->status = STI_STATUS_IDLE;
    ctx->kill_flag = 0;
    ctx->update_flag = 0;
    ctx->total_tb_cnt = 0;
    
    float ttime;
    // if(0 > seq_get_total_time(ctx->seq, &ttime))
    if(0 > wave_get_total_time(wave, &ttime))
    {
        err = -6;
        goto exit;
    }
    ctx->total_tb_cnt_end = SEC_TO_NTB(ttime, ctx->seq->time_base_us);

    ctx->value_tb_cnt = 0;
    ctx->value_tb_cnt_end = 0;
    ctx->state_tb_cnt = 0;
    ctx->state_tb_cnt_end = 0;
    ctx->value_pn_cnt = 0;

    uint32_t pn;
    if(0 > seq_get_value_pn(ctx->seq, &pn))
    {
        err = -7;
        goto exit;
    }
    ctx->value_pn_cnt_end = pn;

    ctx->state_pn_cnt = 0;

    if(0 > seq_get_state_pn(ctx->seq, &pn))
    {
        err = -8;
        goto exit;
    }
    ctx->state_pn_cnt_end = pn;
    
    ctx->value_head = seq_get_value_head(ctx->seq);
    if(ctx->value_head == NULL)
    {
        err = -9;
        goto exit;
    }

    ctx->state_head = seq_get_state_head(ctx->seq);
    if(ctx->state_head == NULL)
    {
        err = -10;
        goto exit;
    }

    ctx->value_ptr = ctx->value_head;
    ctx->state_ptr = ctx->state_head;

    ctx->dark_cur = 0;
    ctx->cur_value = 0;
    ctx->cur_state = 0;
    ctx->cur_scale_step = 0;
    ctx->cur_scale = 0;

    return ctx;

exit:
    release_port_ctx(ctx);
    return NULL;
}

static void release_port_ctx(sti_port_ctx_t *ctx)
{
    if(ctx != NULL)
    {
        if(ctx->seq != NULL)
        {
            release_seq(ctx->seq);
        }
        
        free(ctx);
    }
}

void release_context(sti_context_t *ctx)
{
    if(ctx != NULL)
    {
        for(uint32_t i=0; i<ctx->port_num; i++)
        {
            if(ctx->ports[i] != NULL)
            {
                release_port_ctx(ctx->ports[i]);
            }
        }
        free(ctx);
    }
}
