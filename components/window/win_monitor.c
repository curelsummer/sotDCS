#include <stdint.h>
#include <string.h>

#include "esp_log.h"

#include "e_pres.h"
#include "sti.h"
#include "monitor.h"
#include "lcd.h"
#include "button.h"

#include "win_basic.h"
#include "dispwin.h"

#define TAG "win_monitor"



static void show_name(void);
static void show_time(void);

static void show_current(void);
static void show_impedance(void);

static void show_running_staus(void);



int32_t disp_monitor(void *arg)
{

    lcd_drawfull((uint8_t *)pic_dc);

    Key_EventClr();

    show_name(cfg->waves);

    sti_context_t sti_ctx;

    int32_t err = sti_init_context(&sti_ctx, cfg);
    if(err < 0)
    {
        ESP_LOGE(TAG, "sti_init_context return error. <%d>", err);
        goto exit;
    }

    err = sti_start(&sti_ctx);
    if(err < 0)
    {
        ESP_LOGE(TAG, "sti_start return error. <%d>", err);
        goto exit;
    }

    while(1)
    {
        //lcd_drawfull((uint8_t *)pic_main);
        show_time();
        show_current();
        show_impedance();

        sti_status_t status = sti_get_running_status();
        show_running_staus(status);

        //while(status != STI_STATUS_BUSY);

        if(status == STI_STATUS_BUSY)
        {
            if(Key_GetEvent(KEY_ENTER, KEY_TYPE_LONG_CLC_ONCE))
            {
                sti_stop();
            }
        }
        else
        {
            if( Key_GetEvent(KEY_DEC, KEY_TYPE_SHORT_CLC) || 
            Key_GetEvent(KEY_INC, KEY_TYPE_SHORT_CLC) || 
            Key_GetEvent(KEY_SEL, KEY_TYPE_SHORT_CLC) || 
            Key_GetEvent(KEY_ENTER, KEY_TYPE_SHORT_CLC))
            {
                goto exit;
            }
        }

        vTaskDelay(pdMS_TO_TICKS(100));
    }

exit:
    sti_release_context(&sti_ctx);
    
    switch(cfg->waves->id)
    {
        case TDCS_WAVE_ID:
            return WIN_ID_DC;

        case WAVE_ID_SINE:
            return WIN_ID_SINE;

        case WAVE_ID_CES:
            return WIN_ID_CES;

        default:
            return WIN_ID_DC;
    }
}

static void show_name(waveform_t *wave)
{
    switch(wave->id)
    {
        case TDCS_WAVE_ID:
            lcd_show_str(LINE_1_X, LINE_1_Y, "tDCS", 0);
        break;

        case WAVE_ID_SINE:
            lcd_show_str(LINE_1_X, LINE_1_Y, "tACS", 0);
        break;

        case WAVE_ID_CES:
            lcd_show_str(LINE_1_X, LINE_1_Y, "CES", 0);
        break;
    }
}

static void show_time(void)
{
    uint32_t time = sti_get_running_time_sec();

    char time_str[10];
    sprintf(time_str, "%u", time);

    lcd_show_str(LINE_2_X, LINE_2_Y, time_str, 0);
}

static void show_current(void)
{
    float current;
    int32_t err = mon_get_current(&current);
    if(err < 0)
    {
        ESP_LOGW(TAG, "mon_get_current err: %d", err);

        lcd_show_str(LINE_3_X, LINE_3_Y, "--", 0);

        return;
    }

    char cur_str[10];
    sprintf(cur_str, "%.0f", current);
    lcd_show_str(LINE_3_X, LINE_3_Y, cur_str, 0);
}

static void show_impedance(void)
{
    float imp;
    int32_t err = mon_get_impedance(&imp);
    if(err < 0)
    {
        ESP_LOGW(TAG, "mon_get_impedance err: %d", err);

        lcd_show_str(LINE_4_X, LINE_4_Y, "--", 0);

        return;
    }

    char imp_str[10];
    sprintf(imp_str, "%.0f", imp);
    lcd_show_str(LINE_4_X, LINE_4_Y, imp_str, 0);
}

static void show_running_staus(sti_status_t status)
{
    switch(status)
    {
        case STI_STATUS_IDLE:
            lcd_show_str(LINE_5_X, LINE_5_Y, "IDLE", 0);
        break;

        case STI_STATUS_STARTING:
            lcd_show_str(LINE_5_X, LINE_5_Y, "STARTING", 0);
        break;

        case STI_STATUS_BUSY:
            lcd_show_str(LINE_5_X, LINE_5_Y, "BUSY", 0);
        break;

        case STI_STATUS_STOP:
            lcd_show_str(LINE_5_X, LINE_5_Y, "STOP", 0);
        break;

        case STI_STATUS_COMPLETE:
            lcd_show_str(LINE_5_X, LINE_5_Y, "COMPLETE", 0);
        break;

        case STI_STATUS_ERR:
            lcd_show_str(LINE_5_X, LINE_5_Y, "ERR", 0);
        break;
    }
}
