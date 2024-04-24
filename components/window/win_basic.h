#ifndef __WIN_BASIC_H
#define __WIN_BASIC_H

#include "sti.h"
#include "lcd.h"

#define LINE_1_X  120
#define LINE_1_Y  45

#define LINE_2_X  120
#define LINE_2_Y  105

#define LINE_3_X  120
#define LINE_3_Y  165

#define LINE_4_X  120
#define LINE_4_Y  225

#define LINE_5_X  120
#define LINE_5_Y  285

#define LABEL_X  20
#define LABEL_UNIT_X 252

#define LINE_IMP_X  38
#define LINE_IMP_Y  370

#define LINE_CUR_X  184
#define LINE_CUR_Y  370

#define STAT_X  0
#define STAT_Y  0

#define BAT_ICON_X  278
#define BAT_ICON_Y  8

#define BT_ICON_X  248
#define BT_ICON_Y  6

#define DEVN_ICON_X 210
#define DEVN_ICON_Y 6

#define ID_X 168
#define ID_Y 3


#define WIN_ID_TDCS (0)
#define WIN_ID_TACS (1)
#define WIN_ID_CES (2)
#define WIN_ID_RTACS (3)

#define WIN_ID_HOME_TDCS (54)
#define WIN_ID_HOME_CES (55)

// #define WIN_NUM (6)

#define WIN_CUR (100)
#define WIN_NEXT (101)
#define WIN_PRE (102)


typedef enum _sel_state_t
{
    DESELECT = 0,
    SELECT
}sel_state_t;


#ifdef __cplusplus
 extern "C" {
#endif

void show_back_pic(const uint8_t *pic);
void show_bat(const uint8_t *pic);
void show_ble(const uint8_t *pic);
void show_devid(const uint8_t *pic);

int32_t show_real_current(float *value, uint8_t disp_flag);
int32_t show_real_impedance(float *value, uint8_t disp_flag);

void show_cntdown_time(uint8_t row, const uint8_t *pic);
void show_running_status(sti_status_t status);

void show_value_astr(char *astr, uint8_t row, sel_state_t sel, const uint8_t *pic);
void show_label_astr(char *astr, uint8_t row, sel_state_t sel, const uint8_t *pic);
void show_label_cstr(uint16_t *cstr, uint8_t row, sel_state_t sel, const uint8_t *pic);
void show_unit_astr(char *astr, uint8_t row, sel_state_t sel, const uint8_t *pic);
void show_message_astr(char *astr, const uint8_t *pic);

void show_message_lowbat(void);

// void show_name(waveform_t *wave, uint8_t row, sel_state_t sel, const uint8_t *pic);
// void show_current(waveform_t *wave, uint8_t row, sel_state_t sel, const uint8_t *pic);
// void show_duration(waveform_t *wave, uint8_t row, sel_state_t sel, const uint8_t *pic);

// void show_label_mode(uint8_t row, sel_state_t sel, const uint8_t *pic);
// void show_label_cur(uint8_t row, sel_state_t sel, const uint8_t *pic);
// void show_label_freq(uint8_t row, sel_state_t sel, const uint8_t *pic);
// void show_label_time(uint8_t row, sel_state_t sel, const uint8_t *pic);

#ifdef __cplusplus
}
#endif

#endif /* __WINDOWS_H */