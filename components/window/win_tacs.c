#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "esp_log.h"

#include "sysenv.h"

#include "button.h"
#include "beep.h"
#include "dispwin.h"
#include "win_basic.h"

#include "e_pres.h"
#include "sti.h"
#include "monitor.h"

#include "gatts_server.h"

#define TAG "win_tacs"

#define PIC (pic_sine)
#define ITEM_NUM (4)

#define ROW_MODE (1)
#define ROW_CUR  (2)
#define ROW_FREQ  (3)
#define ROW_TIME (4)

#define DEFAULT_CURRENT (1000)
#define DEFAULT_FREQ (0.75)
#define DEFAULT_OFFSET (0)
#define DEFAULT_FADE_IN (15)
#define DEFAULT_DURATION (1200)
#define DEFAULT_FADE_OUT (15)


static waveform_t *wave = NULL;
static e_pres_t *epres = NULL;

static uint8_t item_sel=0;

static void show_background(void);
static void refresh_value_all(void);

static void show_label_mode(void);
static void show_label_current(void);
static void show_label_freq(void);
static void show_label_duration(void);

static void show_name(sel_state_t sel);
static void show_current(sel_state_t sel);
static void show_freq(sel_state_t sel);
static void show_duration(sel_state_t sel);

static void set_current(int32_t type, float value);
static void set_duration(int32_t type, float value);
static void set_freq(int32_t type, float value);

static void sti_status_callback(sti_status_t status);
static sti_status_t sti_status = STI_STATUS_IDLE;
static uint32_t sleep_cnt = 0;

int32_t disp_win_tacs(void)
{
    int32_t err;
    int32_t next_win = WIN_CUR;
    
    show_background();

    if(epres == NULL)
    {
        epres = epres_generate_single_mode("e_pres_tacs", NULL);
        if(epres == NULL)
        {
            err = -1;
            goto exit;
        }
    }

    if(wave == NULL)
    {
        wave = tacs_generate_waveform(  DEFAULT_FADE_IN, 
                                        DEFAULT_DURATION, 
                                        DEFAULT_FADE_OUT, 
                                        DEFAULT_CURRENT, 
                                        DEFAULT_FREQ, 
                                        DEFAULT_OFFSET, 
                                        sysenv_get_ts());
        if(wave == NULL)
        {
            err = -2;
            goto exit;
        }
    }

    epres_set_wave(epres, 0, wave);

    refresh_value_all();
    Key_EventClr();

    int32_t ret = ble_check_cmd();
    if(ret == TACS_WAVE_ID)
    {
        ble_config_t ble_cfg;
        ret = ble_get_cmd(&ble_cfg);
        if(ret == 1)
        {
            set_current(0, ble_cfg.current);
            set_freq(0, ble_cfg.frequency);
            set_duration(0, ble_cfg.duration);
            refresh_value_all();
        }
    }
    
    while(1)
    {
        show_bat(PIC);
        show_ble(PIC);

        if(Key_GetEvent(KEY_DEC, KEY_TYPE_SHORT_CLC))
        {
            switch(item_sel)
            {
                case 0:
                    next_win = WIN_PRE;
                    goto exit;
            
                case 1:
                    set_current(1, -100);
                    show_current(SELECT);
                    break;
                case 2:
                    set_freq(1, -0.01);
                    show_freq(SELECT);
                    break; 
                case 3:
                    set_duration(1, -30);
                    show_duration(SELECT);
                    break;
            }
        }

        if(Key_GetEvent(KEY_DEC, KEY_TYPE_LONG_CLC_CYC))
        {
            switch(item_sel)
            {          
                case 1:
                    set_current(1, -100);
                    show_current(SELECT);
                    break;
                case 2:
                    set_freq(1, -0.01);
                    show_freq(SELECT);
                    break; 
                case 3:
                    set_duration(1, -30);
                    show_duration(SELECT);
                    break;
            }
        }

        if(Key_GetEvent(KEY_INC, KEY_TYPE_SHORT_CLC))
        {
            switch(item_sel)
            {
                case 0:
                    next_win = WIN_NEXT;
                    goto exit;
            
                case 1:
                    set_current(1, 100);
                    show_current(SELECT);
                    break;
                case 2:
                    set_freq(1, 0.01);
                    show_freq(SELECT);
                    break; 
                case 3:
                    set_duration(1, 30);
                    show_duration(SELECT);
                    break;
            }
        }

        if(Key_GetEvent(KEY_INC, KEY_TYPE_LONG_CLC_CYC))
        {
            switch(item_sel)
            {
                case 1:
                    set_current(1, 100);
                    show_current(SELECT);
                    break;
                case 2:
                    set_freq(1, 0.01);
                    show_freq(SELECT);
                    break; 
                case 3:
                    set_duration(1, 30);
                    show_duration(SELECT);
                    break;
            }
        }

        if(Key_GetEvent(KEY_SEL, KEY_TYPE_SHORT_CLC))
        {
            item_sel++;
            if(item_sel >= ITEM_NUM)
            {
                item_sel = 0;
            }
            refresh_value_all();
        }
        
        if(Key_GetEvent(KEY_ENTER, KEY_TYPE_SHORT_CLC))
        {
            show_background();

            show_name(DESELECT);
            show_current(DESELECT);
            show_freq(DESELECT);

            break;
        }

        int32_t ret = ble_check_cmd();
        switch(ret)
        {
            case -1:
            case 0: break;
            case TDCS_WAVE_ID: next_win = WIN_ID_TDCS; goto exit;
            case TACS_WAVE_ID: next_win = WIN_ID_TACS; goto exit;
            case CES_WAVE_ID: next_win = WIN_ID_CES; goto exit;
            default: ble_skip_cmd();break;
        }

        vTaskDelay(pdMS_TO_TICKS(100));
    }

    float bat_v;
    mon_get_battery_v(&bat_v);
    if(bat_v < 7200)
    {
        ESP_LOGE(TAG, "battery low level.");
        show_message_lowbat();
        beep_start(100, 2);
        vTaskDelay(pdMS_TO_TICKS(1000));
        goto exit;
    }


    err = sti_start(epres, sti_status_callback);
    if(err < 0)
    {
        ESP_LOGE(TAG, "sti_start return error. <%d>", err);
        goto exit;
    }

    while(sti_status != STI_STATUS_BUSY)
    {
        show_bat(PIC);
        show_ble(PIC);
        // show_running_status(sti_status);

        vTaskDelay(pdMS_TO_TICKS(300));
    }

    Key_EventClr();
    sleep_cnt = 0;
    while(1)
    {
        if(Key_GetEvent(KEY_ENTER, KEY_TYPE_LONG_CLC_ONCE))
        {
            sleep_cnt = 0;
            sti_stop();
        }

        if( Key_GetEvent(KEY_DEC, KEY_TYPE_SHORT_CLC) || 
            Key_GetEvent(KEY_INC, KEY_TYPE_SHORT_CLC) || 
            Key_GetEvent(KEY_SEL, KEY_TYPE_SHORT_CLC) || 
            Key_GetEvent(KEY_ENTER, KEY_TYPE_SHORT_CLC))
        {
            sleep_cnt = 0;
        }

        float vol;
        mon_get_voltage(&vol);
        if((vol > 26000) || (vol < -26000))
        {
            sleep_cnt = 0;
            sti_stop();
        }

        if(sti_status != STI_STATUS_BUSY)
        {
            lcd_backlight_on();
            break;
        }

        show_bat(PIC);
        show_ble(PIC);
        show_cntdown_time(ROW_TIME, PIC);

        show_real_current(NULL, 0);
        show_real_impedance(NULL, 0);

        vTaskDelay(pdMS_TO_TICKS(200));

        sleep_cnt++;
        if(sleep_cnt > 150)
        {
            lcd_backlight_off();
        }
        else
        {
            lcd_backlight_on();
        }
    }

    while(1)
    {
        vTaskDelay(pdMS_TO_TICKS(300));
        
        show_bat(PIC);
        show_ble(PIC);
        show_running_status(sti_status);

        show_cntdown_time(ROW_TIME, PIC);

        show_real_current(NULL, 0);
        show_real_impedance(NULL, 0);

        if(sti_status == STI_STATUS_COMPLETE)
        {
            beep_start(300, 3);
            break;
        }
    }

    Key_EventClr();
    while(1)
    {   
        if( Key_GetEvent(KEY_DEC, KEY_TYPE_SHORT_CLC) || 
            Key_GetEvent(KEY_INC, KEY_TYPE_SHORT_CLC) || 
            Key_GetEvent(KEY_SEL, KEY_TYPE_SHORT_CLC) || 
            Key_GetEvent(KEY_ENTER, KEY_TYPE_SHORT_CLC))
        {
            goto exit;
        }

        show_bat(PIC);
        show_ble(PIC);
        show_running_status(sti_status);

        show_real_current(NULL, 0);
        show_real_impedance(NULL, 0);

        vTaskDelay(pdMS_TO_TICKS(200));
    }

exit:
    return next_win;
}

static void show_background(void)
{
    show_back_pic(PIC);

    show_bat(PIC);
    show_ble(PIC);
    show_devid(PIC);

    show_label_mode();
    show_label_current();
    show_label_freq();
    show_label_duration();
}

static void refresh_value_all(void)
{
    switch(item_sel)
    {
        case 0:
            show_name(SELECT);
            show_current(DESELECT);
            show_freq(DESELECT);
            show_duration(DESELECT);
        break;

        case 1:
            show_name(DESELECT);
            show_current(SELECT);
            show_freq(DESELECT);
            show_duration(DESELECT);
        break;

        case 2:
            show_name(DESELECT);
            show_current(DESELECT);
            show_freq(SELECT);
            show_duration(DESELECT);
        break;

        case 3:
            show_name(DESELECT);
            show_current(DESELECT);
            show_freq(DESELECT);
            show_duration(SELECT);
	    break;

        default:
            ESP_LOGW(TAG, "item select error.");
            item_sel = 0;
            break;
    }
}

static void show_label_mode(void)
{
    uint16_t cstr[] = {1, 2, 0};
    show_label_cstr(cstr, ROW_MODE, 0, PIC);
}

static void show_label_current(void)
{
    uint16_t cstr[] = {3, 4, 0};
    show_label_cstr(cstr, ROW_CUR, 0, PIC);
    show_unit_astr("uA", ROW_CUR, 0, PIC);
}

static void show_label_freq(void)
{
    uint16_t cstr[] = {5, 6, 0};
    show_label_cstr(cstr, ROW_FREQ, 0, PIC);
    show_unit_astr("Hz", ROW_FREQ, 0, PIC);
}

static void show_label_duration(void)
{
    uint16_t cstr[] = {7, 8, 0};
    show_label_cstr(cstr, ROW_TIME, 0, PIC);
    show_unit_astr("s", ROW_TIME, 0, PIC);
}

static void show_name(sel_state_t sel)
{
    char str[10];
    sprintf(str, "%5s", "so-tDCS");
    show_value_astr(str, ROW_MODE, sel, PIC);
}

static void show_current(sel_state_t sel)
{
    float current;
    int32_t err = tacs_get_current(wave, &current);

    if(err < 0)
    {
        ESP_LOGW(TAG, "tacs_get_current err. <%d>", err);
        return;
    }
    
    char str[16];
    sprintf(str, "%5.0f", current);
    
    show_value_astr(str, ROW_CUR, sel, PIC);
}

static void show_freq(sel_state_t sel)
{
    float freq;
    int32_t err = tacs_get_freq(wave, &freq);
    if(err < 0)
    {
        ESP_LOGW(TAG, "tacs_get_freq err. <%d>", err);
        return;
    }

    char str[16];
    sprintf(str, "%5.2f", freq);
    show_value_astr(str, ROW_FREQ, sel, PIC);
}

static void show_duration(sel_state_t sel)
{
    float duration;
    int32_t err = wave_get_duration(wave, &duration);
    if(err < 0)
    {
        ESP_LOGW(TAG, "wave_get_duration err. <%d>", err);
        return;
    }

    char str[16];
    sprintf(str, "%5.0f", duration);
    show_value_astr(str, ROW_TIME, sel, PIC);
}


static void set_current(int32_t type, float cur)
{
    const float min = 100;
    const float max = 2500;
    float value;

    if(type == 1)
    {
        rtacs_get_current(wave, &value);
        value += cur;
    }
    else
    {
        value = cur;
    }
    
    if(value > max)
    {
        value = max;
    }
    else if(value < min)
    {
        value = min;
    }

    rtacs_set_current(wave, value);
}

static void set_freq(int32_t type, float freq)
{
    float value;

    if(type == 1)
    {
        tacs_get_freq(wave, &value);
        value += freq;
    }
    else
    {
        value = freq;
    }
    
    if(value > 1.05)
    {
        value = 1.05;
    }
    else if(value < 0.06)
    {
        value = 0.06;
    }

    tacs_set_freq(wave, value);
}


static void set_duration(int32_t type, float dur)
{
    float value;

    if(type == 1)
    {
        wave_get_duration(wave, &value);
        value += dur;
    }
    else
    {
        value = dur;
    }
    
    if(value > 3600)
    {
        value = 3600;
    }
    else if(value < 60)
    {
        value = 60;
    }

    wave_set_duration(wave, value);
}

static void sti_status_callback(sti_status_t status)
{
    // switch(status)
    // {
    //     case 
    // }
    sti_status = status;
    ESP_LOGW(TAG, "STI report: %d", status);
}
