#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"

#include "esp_log.h"
#include "esp_err.h"

#include "cur_output.h"
#include "sti.h"

#define TAG "sti"


static void sti_update_output(void *arg);
static int32_t sti_start_timer(uint32_t us, esp_timer_cb_t timer_cb);
static int32_t sti_stop_timer(void);
static void sti_running_task(void *arg);

static esp_timer_handle_t sti_output_timer = NULL;

static sti_context_t *sti_ctx = NULL;
static sti_status_cb_t sti_status_cb = NULL;

static int32_t end_flag = 0;

int32_t sti_start(e_pres_t *e_pres, sti_status_cb_t cb)
{
    int32_t err;
    if(e_pres == NULL)
    {
        return -1;
    }

    if(sti_ctx != NULL)
    {
        return -2;
    }

    sti_status_cb = cb;

    sti_ctx = generate_context(e_pres);
    if(sti_ctx == NULL)
    {
        err = -3;
        goto exit;
    }

    //print_seq(sti_ctx->ports[0]->seq);

    BaseType_t ret;
    uint32_t ram_left =  heap_caps_get_free_size( MALLOC_CAP_INTERNAL );
    ESP_LOGW(TAG, "%d: - create task free INTERNAL RAM left %dB", __LINE__,ram_left);
    ram_left = heap_caps_get_minimum_free_size(MALLOC_CAP_INTERNAL);
    ESP_LOGW(TAG, "%d: - create task minimum INTERNAL RAM left %dB", __LINE__,ram_left);
    ram_left = heap_caps_get_largest_free_block(MALLOC_CAP_INTERNAL);
    ESP_LOGW(TAG, "%d: - create task largest INTERNAL RAM left %dB", __LINE__,ram_left);
    if(ram_left <2048){
        err = -4;
        goto exit;
    }
    ret = xTaskCreatePinnedToCore(sti_running_task, "sti_running_task", 2048, NULL, 10, NULL, 0);
    if(ret != pdPASS)
    {
        err = -4;
        ESP_LOGE(TAG, "task create fail");
        goto exit;
    }
    
    return 0;

exit:
    sti_status_cb = NULL;
    sti_context_t *tmp = sti_ctx;
    sti_ctx = NULL;
    release_context(tmp);

    return err;
}


static void sti_running_task(void *arg)
{
    ESP_LOGI(TAG, "running task start... ");

    ESP_LOGW(TAG, "%d: - INTERNAL RAM left %dB", __LINE__,
                    heap_caps_get_free_size( MALLOC_CAP_INTERNAL ));

    int32_t err;
    err = cur_open(1000);
    if(err < 0)
    {
        sti_ctx->ports[0]->status = STI_STATUS_ERR;
        if(sti_status_cb != NULL)
        {
            sti_status_cb(STI_STATUS_ERR);
        }

        err = -3;
        goto exit;
    }

    //enable output power manager
    //cur_power_on();

    //wait for power good
    vTaskDelay(pdMS_TO_TICKS(300));

    //set output current to 0
    cur_set_ua(0, 1);

    //wait for output stable
    vTaskDelay(pdMS_TO_TICKS(100));

    //enable output
    cur_enable_output();

    //start timer
    ESP_LOGI(TAG, "start timer %p %p", sti_ctx, sti_ctx->ports[0]);
    ESP_LOGI(TAG, "start timer %d", sti_ctx->ports[0]->seq->time_base_us);
    err = sti_start_timer(sti_ctx->ports[0]->seq->time_base_us, sti_update_output);
    if(err < 0)
    {
        err = -4;
        goto exit;
    }

    sti_ctx->ports[0]->status = STI_STATUS_BUSY;
    if(sti_status_cb != NULL)
    {
        sti_status_cb(STI_STATUS_BUSY);
    }
    

    while(1)
    {
        if(end_flag == 1)
        {
            end_flag = 0;
            sti_stop_timer();
            cur_disable_output();
            cur_set_ua(0, 1);
            cur_power_off();

            sti_ctx->ports[0]->status = STI_STATUS_COMPLETE;
            if(sti_status_cb != NULL)
            {
                sti_status_cb(STI_STATUS_COMPLETE);
            }

            break;
        }
        // esp_timer_dump(stdout);
        vTaskDelay(pdMS_TO_TICKS(100));
    }

exit:
    cur_close();

    sti_context_t *tmp = sti_ctx;
    sti_ctx = NULL;
    release_context(tmp);

    ESP_LOGW(TAG, "%d: - INTERNAL RAM left %dB", __LINE__,
                    heap_caps_get_free_size( MALLOC_CAP_INTERNAL ));
    ESP_LOGI(TAG, "running task quit.%d", err);
    vTaskDelete(NULL);
}

int32_t sti_stop(void)
{
    if(sti_ctx == NULL)
    {
        return -1;
    }

    if(sti_ctx->ports[0] == NULL)
    {
        return -2;
    }

    sti_ctx->ports[0]->kill_flag = 1;

    if(sti_status_cb != NULL)
    {
        sti_status_cb(STI_STATUS_ABORTING);
    }

    return 0;
}

static inline void IRAM_ATTR sti_update_output(void *arg)
{   
    uint8_t update_flag = 0;

    int32_t output_value = 0;

    sti_port_ctx_t *port_ctx = sti_ctx->ports[0];

    if(port_ctx->value_tb_cnt == port_ctx->value_tb_cnt_end)
    {
        port_ctx->cur_value = port_ctx->value_ptr->value;
        port_ctx->update_flag = 1;

        port_ctx->value_tb_cnt = 0;
        port_ctx->value_tb_cnt_end = port_ctx->value_ptr->time;

        port_ctx->value_ptr++;
        port_ctx->value_pn_cnt++;

        if(port_ctx->value_pn_cnt == port_ctx->value_pn_cnt_end)
        {
            port_ctx->value_ptr = port_ctx->value_head;
            port_ctx->value_pn_cnt = 0;
        }
    }

    if(port_ctx->kill_flag == 0)
    {
        if(port_ctx->state_tb_cnt == port_ctx->state_tb_cnt_end)
        {
            if(port_ctx->state_pn_cnt == port_ctx->state_pn_cnt_end)
            {
                ESP_LOGI(TAG, "time-up, sti_stop_timer %d", sti_stop_timer());
                end_flag = 1;
                return;
            }

            port_ctx->cur_state = port_ctx->state_ptr->state;
            port_ctx->update_flag = 1;
 
            port_ctx->state_tb_cnt = 0;
            port_ctx->state_tb_cnt_end = port_ctx->state_ptr->time;

            port_ctx->state_ptr++;
            port_ctx->state_pn_cnt++;
            
        }

        switch(port_ctx->cur_state)
        {   
            case SEQ_STATE_SHAM_DELAY:
            case SEQ_STATE_NORMAL:
                port_ctx->dark_cur = 0;
                port_ctx->cur_scale = 1;
                break;

            case SEQ_STATE_SHAM_FI:
            case SEQ_STATE_FADE_IN: 
                port_ctx->dark_cur = 0;
                port_ctx->cur_scale = port_ctx->cur_scale + port_ctx->fade_in_scale_step;
                port_ctx->update_flag = 1;
                break;
            case SEQ_STATE_SHAM_FO:
            case SEQ_STATE_FADE_OUT: 
                port_ctx->dark_cur = 0;
                port_ctx->cur_scale = port_ctx->cur_scale + port_ctx->fade_out_scale_step;
                port_ctx->update_flag = 1;
                break;

            case SEQ_STATE_SHAM_DUR:
                port_ctx->dark_cur = 20;
                port_ctx->cur_scale = 0;
                break;

            default: 
                ESP_LOGI(TAG, "state error, sti_stop_timer %d", sti_stop_timer());
                end_flag = 1;
                return;
        }
    }
    else
    {
        port_ctx->dark_cur = 0;
        port_ctx->cur_scale = port_ctx->cur_scale - (0.5 / US_TO_NTB(1000000, port_ctx->seq->time_base_us));
        port_ctx->update_flag = 1;

        if(port_ctx->cur_scale < 0)
        {
            ESP_LOGI(TAG, "kill, sti_stop_timer %d", sti_stop_timer());
            end_flag = 1;
            return;
        }
    }

    if(port_ctx->update_flag)
    {
        output_value = port_ctx->cur_value * port_ctx->cur_scale + port_ctx->dark_cur;
        if (sti_ctx->ports[0]->seq->wave->id == CES_WAVE_ID)
        {
            int32_t sec;
            sec = NTB_TO_US(sti_ctx->ports[0]->total_tb_cnt, sti_ctx->ports[0]->seq->time_base_us) / 1000000;
            int32_t worktime = 30;
            int32_t resttime = 30;
            int32_t rest_flag = sec % (worktime + resttime) - worktime;
            if (rest_flag > 0)
            {
                cur_set_ua(0, 1);
            }
            else
            {
                cur_set_ua(output_value, 1);
            }
        }
        else
        {
            cur_set_ua(output_value, 1);
        }
    }
    port_ctx->total_tb_cnt++;
    port_ctx->value_tb_cnt++;
    port_ctx->state_tb_cnt++;  
}

int32_t sti_get_cntdown_time_sec(uint32_t *sec)
{
    if(sti_ctx == NULL)
    {
        return -1;
    }

    if(sti_ctx->ports[0] == NULL)
    {
        return -2;
    }

    *sec = NTB_TO_US(sti_ctx->ports[0]->total_tb_cnt_end - sti_ctx->ports[0]->total_tb_cnt, sti_ctx->ports[0]->seq->time_base_us) / 1000000;

    return 0;
}

int32_t sti_get_running_time_sec(uint32_t *sec)
{
    if(sti_ctx == NULL)
    {
        return -1;
    }

    if(sti_ctx->ports[0] == NULL)
    {
        return -2;
    }
    

    *sec = NTB_TO_US(sti_ctx->ports[0]->total_tb_cnt, sti_ctx->ports[0]->seq->time_base_us) / 1000000;

    return 0;
}

sti_status_t sti_get_running_status(void)
{
    if(sti_ctx == NULL)
    {
        return STI_STATUS_IDLE;
    }

    if(sti_ctx->ports[0] == NULL)
    {
        return STI_STATUS_IDLE;
    }

    return sti_ctx->ports[0]->status;
}

int32_t sti_get_state(void)
{
    if(sti_ctx == NULL)
    {
        return -1;
    }

    if(sti_ctx->ports[0] == NULL)
    {
        return -1;
    }

    return sti_ctx->ports[0]->cur_state;
}

static int32_t sti_start_timer(uint32_t us, esp_timer_cb_t timer_cb)
{
    esp_timer_create_args_t sti_output_timer_arg;
    
    sti_output_timer_arg.callback = timer_cb;
    sti_output_timer_arg.arg = NULL;
    sti_output_timer_arg.name = "sti_output_timer";
    
	esp_err_t err = esp_timer_create(&sti_output_timer_arg, &sti_output_timer);
    if(err != ESP_OK)
    {
        sti_output_timer = NULL;
        return -1;
    }
    
	err = esp_timer_start_periodic(sti_output_timer,  us);
    if(err != ESP_OK)
    {
        esp_timer_delete(sti_output_timer);
        sti_output_timer = NULL;

        return -2;
    }

    return 0;
}

static int32_t sti_stop_timer(void)
{
    if(sti_output_timer == NULL)
    {
        return 1;
    }

    esp_err_t err = esp_timer_stop(sti_output_timer);
    
    if(err != ESP_OK)
    {
        sti_output_timer = NULL;
        return -1;
    }
    
    err = esp_timer_delete(sti_output_timer);
    if(err != ESP_OK)
    {
        sti_output_timer = NULL;
        return -2;
    }
    
    sti_output_timer = NULL;

    return 0;
}

sti_context_t* sti_get_active_ctx(void)
{
    return sti_ctx;
}
