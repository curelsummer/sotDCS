#ifndef __GATTS_SERVER_H
#define __GATTS_SERVER_H


#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/* Attributes State Machine */
enum
{
    IDX_SVC,
    IDX_CHAR_A,
    IDX_CHAR_VAL_A,
    IDX_CHAR_CFG_A,

    IDX_CHAR_B,
    IDX_CHAR_VAL_B,

    IDX_CHAR_C,
    IDX_CHAR_VAL_C,

    HRS_IDX_NB,
};


// typedef struct _sti_send_param_t
// {
//     uint8_t sti_mode;
//     uint8_t sti_status;
//     float sti_cur;          //uA
//     float sti_remain_time;  //min
//     float sti_freq;         //Hz
//     float sti_offset;       //uA
//     float impedance;        //kΩ
//     float bat_vol;          //V
// } sti_send_param_t;

typedef struct _ble_config_t
{
    uint8_t  wave_id;
    float current;
    float frequency;
    float offset;
    float width;
    float duration;
}ble_config_t;


void ble_task(void *arg);
// int32_t gatts_send(sti_send_param_t *param);
uint8_t gatts_is_connected(void);

int32_t ble_check_cmd(void);
void ble_skip_cmd(void);
int32_t ble_get_cmd(ble_config_t *ble_cfg);

#endif

