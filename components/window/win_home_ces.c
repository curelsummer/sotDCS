#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "esp_log.h"

#include "sysenv.h"
#include "ces.h"
#include "button.h"
#include "beep.h"
#include "dispwin.h"
#include "win_basic.h"

#include "e_pres.h"
#include "sti.h"
#include "monitor.h"

#include "gatts_server.h"

#define TAG "win_ces"

#define PIC (pic_dc)
#define ITEM_NUM (3)

#define ROW_MODE (1)
#define ROW_CUR  (2)
#define ROW_TIME (3)


#define DEFAULT_CURRENT (2000)
#define DEFAULT_FADE_IN (15)
#define DEFAULT_DURATION (3600)
#define DEFAULT_FADE_OUT (15)
#define DEFAULT_WIDTH    (250)
#define DEFAULT_FREQ (120)

static waveform_t *wave = NULL;
static e_pres_t *epres = NULL;

// static sti_context_t sti_ctx;


static uint8_t item_sel=0;

static void show_background(void);
static void refresh_value_all(void);

static void show_label_mode(void);
static void show_label_current(void);
static void show_label_duration(void);


static void show_name(sel_state_t sel);
static void show_current(sel_state_t sel);
static void show_duration(sel_state_t sel);


static void set_current(int32_t type, float value);
static void set_duration(int32_t type, float value);

static void sti_status_callback(sti_status_t status);

static sti_status_t sti_status = STI_STATUS_IDLE;

static uint32_t sleep_cnt = 0;

int32_t disp_win_home_ces(void)
{
    int32_t err = 0;
    int32_t next_win = WIN_CUR;

    show_background();

    if(epres == NULL)
    {
        epres = epres_generate_single_mode("e_pres_ces", NULL);
        if(epres == NULL)
        {
            err = -1;
            goto exit;
        }
    }

    if(wave == NULL)
    {
        wave = ces_generate_waveform(   DEFAULT_FADE_IN, 
                                        DEFAULT_DURATION, 
                                        DEFAULT_FADE_OUT, 
                                        DEFAULT_CURRENT,
                                        DEFAULT_WIDTH,
                                        DEFAULT_FREQ,
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
    if(ret == CES_WAVE_ID)
    {
        ble_config_t ble_cfg;
        ret = ble_get_cmd(&ble_cfg);
        if(ret == 1)
        {
            set_current(0, ble_cfg.current);
            set_duration(0, ble_cfg.duration);
            refresh_value_all();
        }
    }

    while(1)
    {
        show_bat(PIC);
        show_ble(PIC);
        
        if(Key_GetEvent(KEY_SEL, KEY_TYPE_SHORT_CLC) || Key_GetEvent(KEY_INC, KEY_TYPE_SHORT_CLC))
        {
            show_background();

            show_name(DESELECT);
            show_current(DESELECT);

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

    // show_background();
    Key_EventClr();
    sleep_cnt = 0;
    while(1)
    {
        if(Key_GetEvent(KEY_SEL, KEY_TYPE_LONG_CLC_ONCE) || Key_GetEvent(KEY_INC, KEY_TYPE_LONG_CLC_ONCE))
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

        show_real_current(NULL, 1);
        show_real_impedance(NULL, 1);

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

        show_real_current(NULL, 1);
        show_real_impedance(NULL, 1);

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
    show_label_duration();
}


static void refresh_value_all(void)
{

    show_name(DESELECT);
    show_current(DESELECT);
    show_duration(DESELECT);

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

static void show_label_duration(void)
{
    uint16_t cstr[] = {7, 8, 0};
    show_label_cstr(cstr, ROW_TIME, 0, PIC);
    show_unit_astr("s", ROW_TIME, 0, PIC);
}

static void show_name(sel_state_t sel)
{
    char str[10];
    sprintf(str, "%5s", "CES");
    show_value_astr(str, ROW_MODE, sel, PIC);
}

static void show_current(sel_state_t sel)
{
    float current;
    int32_t err = ces_get_current(wave, &current);

    if(err < 0)
    {
        ESP_LOGW(TAG, "wave_get_dc_cur err. <%d>", err);
        return;
    }
    
    char str[16];
    sprintf(str, "%5.0f", current);
    
    show_value_astr(str, ROW_CUR, sel, PIC);
}

static void show_duration(sel_state_t sel)
{
    float duration;
    int32_t err = wave_get_duration(wave, &duration);
    if(err < 0)
    {
        ESP_LOGW(TAG, "wave_get_dc_duration err. <%d>", err);
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
        ces_get_current(wave, &value);
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

    ces_set_current(wave, value);
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



