#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "nvs_flash.h"
#include "nvs.h"

#include "esp_log.h"

#include "sysenv.h"
#include "monitor.h"
#include "waveform.h"

#include "button.h"
#include "calibration.h"

#include "win_basic.h"

#define TAG "win_debug"
#define PIC (pic_dc)

#define NVS_NAMESPACE    "std"
#define NVS_ID_KEY       "dev_id"
#define NVS_MODE_KEY     "dev_mode"

typedef void (*show_cb_t)(sel_state_t);


// #define ITEM_NUM (3)
static uint8_t ITEM_NUM = 4;
#define ROW_BAT (1)

#define ROW_DEV_ID (3)
#define ROW_DEV_TS (4)
#define ROW_CALI (5)
#define ROW_EXIT (6)
#define ROW_DEV_MODE (7)

static uint8_t item_sel = 0;

static int32_t dev_id = 0;
static int32_t dev_mode = 0;
static wave_true_sham_t dev_ts = WAVE_TRUE;

static void disp_str_a16(char *astr, uint8_t row, sel_state_t sel);
static void disp_title(char *str);
static void dbg_show_bat(void);
static void show_background(void);
static void show_status_bar(void);
static void refresh_value_all(void);

static void show_id(sel_state_t sel);
static void show_ts(sel_state_t sel);
static void show_cali(sel_state_t sel);
static void show_devmode(sel_state_t sel);
static void show_exit(sel_state_t sel);

static void change_id(int32_t inc_dec);
static void change_ts(void);
static void change_devmod(int32_t inc_dec);

static void show_busy(void);
static void confirm_complete(void);

show_cb_t item_list[] = {
    show_id,
    show_ts,
    show_cali,
    show_exit,
    show_devmode
};



static uint8_t magic = 0;

int32_t disp_win_debug(void)
{
    show_background();

    dev_id = sysenv_get_device_id();
    dev_mode = sysenv_get_device_mode();
    dev_ts = sysenv_get_ts();

    refresh_value_all();
    Key_EventClr();

    while(1)
    {
        dbg_show_bat();

        if(Key_GetEvent(KEY_DEC, KEY_TYPE_SHORT_CLC))
        {
            if(item_sel == 0)
            {
                change_id(-1);
                show_id(1);
            }

            if(item_sel == 1)
            {
                change_ts();
                show_ts(1);
            }

            if((item_sel == 3) && (magic < 6))
            {
                switch(magic)
                {
                    case 0:
                    case 2:
                    case 4: magic++;break;
                    default: magic = 0;break;
                }
            }

            if(item_sel == 4)
            {
                change_devmod(-1);
                show_devmode(1);
            }
        }

        if(Key_GetEvent(KEY_INC, KEY_TYPE_SHORT_CLC))
        {
            if(item_sel == 0)
            {
                change_id(1);
                show_id(1);
            }
            
            if(item_sel == 1)
            {
                change_ts();
                show_ts(1);
            }

            if((item_sel == 3) && (magic < 6))
            {
                switch(magic)
                {
                    case 1:
                    case 3: magic++; break;
                    case 5: 
                        ITEM_NUM++;
                        show_background();
                        refresh_value_all();
                        break;
                    default: magic = 0; break;
                }
            }

            if(item_sel == 4)
            {
                change_devmod(1);
                show_devmode(1);
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
            switch(item_sel)
            {
                case 0:
                    show_busy();
                    sysenv_save_device_id(dev_id);
                    confirm_complete();
                    break;

                case 1:
                    show_busy();
                    sysenv_save_ts(dev_ts);
                    confirm_complete();
                    break;

                case 2:
                    show_busy();
                    cal_start();
                    confirm_complete();
                    break;

                case 3:
                    goto exit;

                case 4:
                    show_busy();
                    sysenv_save_device_mode(dev_mode);
                    confirm_complete();
                    break;
            }

            Key_EventClr();
            show_background();
            refresh_value_all();
        }

        vTaskDelay(pdMS_TO_TICKS(100));
    }

exit:
    return dev_mode;
}

static void show_background(void)
{
    lcd_draw_rect(0, 0, 320, 480, 0x00000000);
    // lcd_drawfull(pic_back);
    disp_title("DEBUG MODE");
}


// static void refresh_value_all(void)
// {   
//     switch(item_sel)
//     {
//         case 0:
//             show_id(SELECT);
//             show_cali(DESELECT);
//             show_exit(DESELECT);
//             if(magic > 5)
//             {
//                 show_devmode(DESELECT);
//             }
//         break;

//         case 1:
//             show_id(DESELECT);
//             show_cali(SELECT);
//             show_exit(DESELECT);
//             if(magic > 5)
//             {
//                 show_devmode(DESELECT);
//             }
//         break;

//         case 2:
//             show_id(DESELECT);
//             show_cali(DESELECT);
//             show_exit(SELECT);
//             if(magic > 5)
//             {
//                 show_devmode(DESELECT);
//             }
//         break;

//         case 3:
//             if(magic > 5)
//             {
//                 show_id(DESELECT);
//                 show_cali(DESELECT);
//                 show_exit(DESELECT);
//                 show_devmode(SELECT);
//             }
//             else
//             {
//                 item_sel = 0;
//             }
//             break;

//         default:
//             ESP_LOGW(TAG, "item select error.");
//             item_sel = 0;
//             break;
//     }
// }

static void refresh_value_all(void)
{   
    for(uint32_t i=0; i<ITEM_NUM; i++)
    {
        if(i == item_sel)
        {
            item_list[i](SELECT);
        }
        else
        {
            item_list[i](DESELECT);
        }
    }
}

static void dbg_show_bat(void)
{
    float vol;
    mon_get_battery_v(&vol);

    char str[64];
    sprintf(str, "Battery voltage: %.0fmV", vol);

    disp_str_a16(str, ROW_BAT, 0);
}

static void show_id(sel_state_t sel)
{ 
    char str[64];
    sprintf(str, "Device ID: tDCS%d", dev_id);
    disp_str_a16(str, ROW_DEV_ID, sel);
}

static void show_ts(sel_state_t sel)
{
    if(dev_ts == WAVE_SHAM)
    {
        disp_str_a16("Stimulation type: SHAM", ROW_DEV_TS, sel);
    }
    else
    {
        disp_str_a16("Stimulation type: TRUE", ROW_DEV_TS, sel);
    }
}

static void show_cali(sel_state_t sel)
{ 
    disp_str_a16("Push Enter to start Calibration.", ROW_CALI, sel);
}

static void show_devmode(sel_state_t sel)
{ 
    char str[64];
    switch(dev_mode)
    {
        case WIN_ID_HOME_TDCS: sprintf(str, "Device Mode: HOME_TDCS");break;
        case WIN_ID_HOME_CES: sprintf(str, "Device Mode: HOME_CES ");break;
        default: sprintf(str, "Device Mode: STANDARD ");break;
    }

    disp_str_a16(str, ROW_DEV_MODE, sel);
}

static void show_exit(sel_state_t sel)
{ 
    disp_str_a16("Push to exit.", ROW_EXIT, sel);
}


static void change_id(int32_t inc_dec)
{
    dev_id += inc_dec;

    if(dev_id > 6)
    {
        dev_id = 1;
    }
    else if(dev_id < 1)
    {
        dev_id = 6;
    }
}

static void change_ts(void)
{
    if(dev_ts == WAVE_SHAM)
    {
        dev_ts = WAVE_TRUE;
    }
    else
    {
        dev_ts = WAVE_SHAM;
    }
}

static void change_devmod(int32_t inc_dec)
{
    switch(dev_mode)
    {
        case WIN_ID_HOME_TDCS:
            if(inc_dec > 0)
            {
                dev_mode = WIN_ID_HOME_CES;
            }
            else if(inc_dec < 0)
            {
                dev_mode = WIN_ID_TDCS;
            }
            break;

        case WIN_ID_HOME_CES:
            if(inc_dec > 0)
            {
                dev_mode = WIN_ID_TDCS;
            }
            else if(inc_dec < 0)
            {
                dev_mode = WIN_ID_HOME_TDCS;
            }
            break;

        default:
            if(inc_dec > 0)
            {
                dev_mode = WIN_ID_HOME_TDCS;
            }
            else if(inc_dec < 0)
            {
                dev_mode = WIN_ID_HOME_CES;
            }
            break;
    }
}

static void show_busy(void)
{
    char *str = "BUSY...";
    uint32_t x = 160 - 4 * strlen(str);

    lcd_show_str16(x, 240, str, 1);
}


static void confirm_complete(void)
{
    char *str = "FINISH.";
    uint32_t x = 160 - 4 * strlen(str);
    lcd_show_str16(x, 240, str, 1);

    while(1)
    {
        if(Key_GetEvent(KEY_ENTER, KEY_TYPE_SHORT_CLC))
        {
            break;
        }

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}


static void show_status_bar(void)
{
    lcd_disp_part_pic(0, 0, 320, 40, PIC);
}


static void disp_str_a16(char *astr, uint8_t row, sel_state_t sel)
{
    if(astr == NULL)
    {
        return;
    }

    uint16_t y = 5 + 18 * row;

    lcd_show_str16(20, y, astr, sel);
}

static void disp_title(char *str)
{
    uint32_t x = 160 - 4 * strlen(str);
    lcd_show_str16(x, 6, str, 0);
}