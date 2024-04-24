#include <stdint.h>
#include <stdio.h>

#include "esp_log.h"

#include "sysenv.h"
#include "pic.h"
#include "lcd.h"

#include "win_basic.h"
#include "sti.h"
#include "monitor.h"

#include "gatts_server.h"

#define TAG "win_basic"





void show_back_pic(const uint8_t *pic)
{
    lcd_drawfull(pic);
}

void show_bat(const uint8_t *pic)
{
    uint32_t bat_l;
    mon_get_battery_level(&bat_l);

    switch(bat_l)
    {
        case 0:
            lcd_show_bat(BAT_ICON_X, BAT_ICON_Y, pic_bat0, pic);
            break;
        case 1:
            lcd_show_bat(BAT_ICON_X, BAT_ICON_Y, pic_bat1, pic);
            break;
        case 2:
            lcd_show_bat(BAT_ICON_X, BAT_ICON_Y, pic_bat2, pic);
            break;
        case 3:
            lcd_show_bat(BAT_ICON_X, BAT_ICON_Y, pic_bat3, pic);
            break;
    }
}

void show_ble(const uint8_t *pic)
{
    
    if(0 == gatts_is_connected())
    {
        lcd_show_bt(BT_ICON_X, BT_ICON_Y, pic, 0);
    }
    else
    {
        lcd_show_bt(BT_ICON_X, BT_ICON_Y, pic, 1);
    }
}

void show_devid(const uint8_t *pic)
{
    int32_t devid = sysenv_get_device_id();

    const uint8_t *icon;
    switch(devid)
    {
        case 1: icon = deviceNo_1;break;
        case 2: icon = deviceNo_2;break;
        case 3: icon = deviceNo_3;break;
        case 4: icon = deviceNo_4;break;
        case 5: icon = deviceNo_5;break;
        case 6: icon = deviceNo_6;break;
        default: return;
    }

    lcd_show_devn(DEVN_ICON_X, DEVN_ICON_Y, icon, pic);
}

// 
// sti_status_t sti_get_running_status(void);

// void show_name(uint8_t row, sel_state_t sel, const uint8_t *pic)
// {
//     char *name = sti_get_wave_name();
//     char str[10];
//     sprintf(str, "%5s", name);

//     uint16_t x, y;

//     switch(row)
//     {
//         case 1: x = LINE_1_X; y = LINE_1_Y; break;
//         case 2: x = LINE_2_X; y = LINE_2_Y; break;
//         case 3: x = LINE_3_X; y = LINE_3_Y; break;
//         case 4: x = LINE_4_X; y = LINE_4_Y; break;
//         case 5: x = LINE_5_X; y = LINE_5_Y; break;
//         default: x = 0; y = 0; break;
//     }

//     lcd_show_str(x, y, str, sel, pic);
// }


void show_cntdown_time(uint8_t row, const uint8_t *pic)
{
    
    uint32_t time;
    if(0 > sti_get_cntdown_time_sec(&time))
    {
        return;
    }

    char str[10];
    sprintf(str, "%5d", time);

    uint16_t x, y;

    switch(row)
    {
        case 1: x = LINE_1_X; y = LINE_1_Y; break;
        case 2: x = LINE_2_X; y = LINE_2_Y; break;
        case 3: x = LINE_3_X; y = LINE_3_Y; break;
        case 4: x = LINE_4_X; y = LINE_4_Y; break;
        case 5: x = LINE_5_X; y = LINE_5_Y; break;
        default: x = 0; y = 0; break;
    }

    lcd_show_str(x, y, str, 0, pic);
}

void show_running_status(sti_status_t status)
{   
    uint16_t cstr[15];
    uint16_t *ptr = cstr;
    switch(status)
    {
        case STI_STATUS_IDLE:
            *ptr++ = 1;
            *ptr++ = 2;
            *ptr++ = 3;
            *ptr++ = 4;
            *ptr++ = 0;
        break;

        case STI_STATUS_STARTING:
            *ptr++ = 5;
            *ptr++ = 6;
            *ptr++ = 7;
            *ptr++ = 8;
            *ptr++ = 0;
        break;

        case STI_STATUS_BUSY:
            *ptr++ = 5;
            *ptr++ = 6;
            *ptr++ = 9;
            *ptr++ = 10;
            *ptr++ = 0;
        break;

        case STI_STATUS_ABORTING:
            *ptr++ = 5;
            *ptr++ = 6;
            *ptr++ = 11;
            *ptr++ = 12;
            *ptr++ = 0;
        break;

        case STI_STATUS_COMPLETE:
            *ptr++ = 25;
            *ptr++ = 26;
            *ptr++ = 15;
            *ptr++ = 16;
            *ptr++ = 0;
        break;

        case STI_STATUS_ERR:
            *ptr++ = 25;
            *ptr++ = 26;
            *ptr++ = 19;
            *ptr++ = 20;
            *ptr++ = 0;
        break;

        default:
            *ptr++ = 21;
            *ptr++ = 22;
            *ptr++ = 23;
            *ptr++ = 24;
            *ptr++ = 0;
        break;
    }

    lcd_show_cstr24(STAT_X, STAT_Y, cstr, 1, pic_dc);
}

int32_t show_real_current(float *value, uint8_t disp_flag)
{
    float current;
    int32_t err = mon_get_current(&current);
    if(err < 0)
    {
        ESP_LOGW(TAG, "mon_get_current err: %d", err);

        lcd_show_str(LINE_CUR_X, LINE_CUR_Y, " -- ", 0, pic_dc);

        return -1;
    }

    int32_t cur_i32 = (int32_t)(current + 0.5);
    char str[16];
    sprintf(str, "%-4d", cur_i32);

    if(disp_flag == 0)
    {
        lcd_show_str(LINE_CUR_X, LINE_CUR_Y, " -- ", 0, pic_dc);
    }
    else
    {
        lcd_show_str(LINE_CUR_X, LINE_CUR_Y, str, 0, pic_dc);
    }
    
    
    if(value != NULL)
    {
        *value = current;
    }
    
    return 0;
}

int32_t show_real_impedance(float *value, uint8_t disp_flag)
{
    float imp;
    int32_t err = mon_get_impedance(&imp);
    if(err < 0)
    {
        ESP_LOGW(TAG, "mon_get_impedance err: %d", err);

        lcd_show_str(LINE_IMP_X, LINE_IMP_Y, " -- ", 0, pic_dc);

        return -1;
    }

    if(imp < 0)
    {
        lcd_show_str(LINE_IMP_X, LINE_IMP_Y, " -- ", 0, pic_dc);
        return -2;
    }

    if(imp > 99.9)
    {
       imp = 99.9;
    }

    char str[16];
    sprintf(str, "%4.1f", imp);

    if((disp_flag == 0) || (imp < 0))
    {
        lcd_show_str(LINE_IMP_X, LINE_IMP_Y, " -- ", 0, pic_dc);
    }
    else
    {
        lcd_show_str(LINE_IMP_X, LINE_IMP_Y, str, 0, pic_dc);
    }
    
    if(value != NULL)
    {
        *value = imp;
    }

    return 0;
}

void show_value_astr(char *cstr, uint8_t row, sel_state_t sel, const uint8_t *pic)
{
    if(cstr == NULL)
    {
        return;
    }

    uint16_t x, y;

    switch(row)
    {
        case 1: x = LINE_1_X; y = LINE_1_Y; break;
        case 2: x = LINE_2_X; y = LINE_2_Y; break;
        case 3: x = LINE_3_X; y = LINE_3_Y; break;
        case 4: x = LINE_4_X; y = LINE_4_Y; break;
        case 5: x = LINE_5_X; y = LINE_5_Y; break;
        default: x = 0; y = 0; break;
    }

    lcd_show_str(x, y, cstr, sel, pic);
}

void show_label_cstr(uint16_t *cstr, uint8_t row, sel_state_t sel, const uint8_t *pic)
{
    if(cstr == NULL)
    {
        return;
    }

    uint16_t y;

    switch(row)
    {
        case 1: y = LINE_1_Y; break;
        case 2: y = LINE_2_Y; break;
        case 3: y = LINE_3_Y; break;
        case 4: y = LINE_4_Y; break;
        case 5: y = LINE_5_Y; break;
        default: y = 0; break;
    }

    lcd_show_cstr(LABEL_X, y, cstr, 0, pic);
}

void show_label_astr(char *astr, uint8_t row, sel_state_t sel, const uint8_t *pic)
{
    if(astr == NULL)
    {
        return;
    }

    uint16_t y;

    switch(row)
    {
        case 1: y = LINE_1_Y; break;
        case 2: y = LINE_2_Y; break;
        case 3: y = LINE_3_Y; break;
        case 4: y = LINE_4_Y; break;
        case 5: y = LINE_5_Y; break;
        default: y = 0; break;
    }

    lcd_show_str(LABEL_X, y, astr, 0, pic);
}


void show_unit_astr(char *astr, uint8_t row, sel_state_t sel, const uint8_t *pic)
{
    if(astr == NULL)
    {
        return;
    }

    uint16_t y;

    switch(row)
    {
        case 1: y = LINE_1_Y; break;
        case 2: y = LINE_2_Y; break;
        case 3: y = LINE_3_Y; break;
        case 4: y = LINE_4_Y; break;
        case 5: y = LINE_5_Y; break;
        default: y = 0; break;
    }

    lcd_show_str(LABEL_UNIT_X, y, astr, 0, pic);
}

void show_message_astr(char *astr, const uint8_t *pic)
{
    lcd_show_str(STAT_X, STAT_Y, astr, 1, pic);
}

void show_message_lowbat(void)
{
    uint16_t cstr[] = {27, 28, 29, 30, 31, 32, 0};
    
    lcd_show_cstr24(STAT_X, STAT_Y, cstr, 1, pic_dc);
}