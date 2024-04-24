#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#include "iir.h"

#include "monitor_ll.h"
#include "monitor.h"

#define TAG "monitor"
//#define OLD_HW

static SemaphoreHandle_t mon_mutex = NULL;

static monitor_t mon_info;
static iir_filter_handle_t iir_handle[3];


static int32_t mon_mng_take(uint32_t ms);
static int32_t mon_mng_give(void);

void monitor(void *arg)
{
    ESP_LOGI(TAG, "Start monitor...");

    mon_info.battery = 0;
    for(uint32_t i=0; i<MAX_MON_PORT; i++)
    {
        mon_info.current[i] = 0;
        mon_info.voltage[i] = 0;
        mon_info.impedance[i] = 0;
    }

//init iir filter
    int32_t err;
    for(uint32_t i=0; i<3; i++)
    {
        err = iir_init(&iir_handle[i]);
        if(err < 0)
        {
            err = -1;
            goto exit;
        }
    }

//init adc and start acquisition
    err = mon_adc_init();
    if(err < 0)
    {
        err = -2;
        goto exit;
    }

    err = mon_adc_start();
    if(err < 0)
    {
        err = -3;
        goto exit;
    }

//updata value every 300ms
    //ESP_LOGI(TAG, "START...");
    //vTaskDelay(pdMS_TO_TICKS(500));
        
    mon_mutex = xSemaphoreCreateMutex();
    if(mon_mutex == NULL)
    {
        err = -4;
        goto exit;
    }

    // mon_update_info();
    
    while(1)
    {
        mon_update_info();
        //ESP_LOGI(TAG, "current: %.2f uA\tvoltage: %.2f mV\timpedance: %.1f\tbattery: %.2f mV\n", mon_info.current[0], mon_info.voltage[0], mon_info.impedance[0], mon_info.battery);
        vTaskDelay(pdMS_TO_TICKS(300));
    }

exit:
    ESP_LOGE(TAG, "ERR: %d", err);
    if(mon_mutex != NULL)
    {
        vSemaphoreDelete(mon_mutex);
        mon_mutex = NULL;
    }

    vTaskDelete(NULL);
}


void mon_update_info(void)
{
    int32_t err;
    float mv[3];
    uint8_t update_flag = 0;

    while(1)
    {
        err = mon_adc_read_mv(mv, 3);
        if(err < 0)
        {
            break;
        }

        update_flag = 1;
#ifdef OLD_HW
        mv[1] = 0 - mv[1];
#endif

        iir_filter(&iir_handle[2], mv[2], &mv[2]);
        //ESP_LOGI(TAG, "voltage_before:\t%.2f\t%.2f\t%.2f",mv[0], mv[1], mv[2]);
        // for(uint32_t i=0; i<3; i++)
        // {
        //     iir_filter(&iir_handle[i], mv[i], &mv[i]);
        // }
        //ESP_LOGI(TAG, "voltage_alter:\t%.2f\t%.2f\t%.2f",mv[0], mv[1], mv[2]);
        //ESP_LOGI(TAG, "current: %.2f uA\tvoltage: %.2f mV\timpedance: %.1f\tbattery: %.2f mV\n", mon_info.current[0], mon_info.voltage[0], mon_info.impedance[0], mon_info.battery);
    }

    if(update_flag == 1)
    {
        while(1)
        {
            err = mon_mng_take(1000);
            if(err == 0)
            {
                break;
            }
        }

        // mon_info.battery = 3.4 * mv[2];
        mon_info.battery = 4.6 * mv[2];
        mon_info.current[0] = mv[1] / 0.25; //uA
        mon_info.voltage[0] = 21 * mv[0] - mv[1]; // mV

        static uint32_t invalid_cnt = 0;
        if((mon_info.voltage[0] < 26000) && (mon_info.voltage[0] > -26000) && ((mon_info.current[0] > 10) || (mon_info.current[0] < -10)))
        {
            mon_info.impedance[0] = ( mon_info.voltage[0] / mon_info.current[0]) - 2;
            invalid_cnt = 0;
        }
        else
        {
            invalid_cnt++;
            
            if(invalid_cnt > 5)
            {
                mon_info.impedance[0] = -1;
            }
        }
        
        mon_mng_give(); 
    }

}

int32_t mon_stop(void)
{
    return mon_adc_stop();
}

int32_t mon_get_current(float *cur)
{
    while(1)
    {
        int32_t ret = mon_mng_take(1000);
        if(ret == -1)
        {
            vTaskDelay(pdMS_TO_TICKS(1000));
        }

        if(ret == 0)
        {
            break;
        }
    }

    *cur = mon_info.current[0];

    mon_mng_give(); 
    return 0;
}

int32_t mon_get_voltage(float *vol)
{
    while(1)
    {
        int32_t ret = mon_mng_take(1000);
        if(ret == -1)
        {
            vTaskDelay(pdMS_TO_TICKS(1000));
        }

        if(ret == 0)
        {
            break;
        }
    }

    *vol = mon_info.voltage[0];

    mon_mng_give(); 

    return 0;
}

int32_t mon_get_impedance(float *imp)
{
    while(1)
    {
        int32_t ret = mon_mng_take(1000);
        if(ret == -1)
        {
            vTaskDelay(pdMS_TO_TICKS(1000));
        }

        if(ret == 0)
        {
            break;
        }
    }

    *imp = mon_info.impedance[0];

    mon_mng_give(); 

    return 0;
}

int32_t mon_get_battery_v(float *bat)
{
    while(1)
    {
        int32_t ret = mon_mng_take(1000);
        if(ret == -1)
        {
            vTaskDelay(pdMS_TO_TICKS(1000));
        }

        if(ret == 0)
        {
            break;
        }
    }

    *bat = mon_info.battery;

    mon_mng_give(); 
    return 0;
}

int32_t mon_get_battery_level(uint32_t *level)
{
    float bat;
    mon_get_battery_v(&bat);

    if(bat > 8000)
    {
        *level = 3;
    }
    else if(bat > 7500)
    {
        *level = 2;
    }
    else if(bat > 7000)
    {
        *level = 1;
    }
    else
    {
        *level = 0;
    }

    return 0;
}


// static int32_t init_mon_info(monitor_t *mon)
// {   
//     if(mon == NULL)
//     {
//         return -1;
//     }

//     mon->battery = 0;
//     for(uint32_t i=0; i<MAX_MON_PORT; i++)
//     {
//         mon->current[i] = 0;
//         mon->voltage[i] = 0;
//         mon->impedance[i] = 0;

//         mon++;
//     }

//     return 0;
// }

static int32_t mon_mng_take(uint32_t ms)
{
    if(mon_mutex == NULL)
    {
        return -1;
    }

    if(pdTRUE != xSemaphoreTake( mon_mutex, pdMS_TO_TICKS(ms)))
    {
        return -2;
    }

    return 0;
}

static int32_t mon_mng_give(void)
{
    if(mon_mutex == NULL)
    {
        return -1;
    }

    if(pdTRUE != xSemaphoreGive( mon_mutex ))
    {
        return -2;
    }

    return 0;
}