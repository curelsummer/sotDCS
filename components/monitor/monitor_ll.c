#include "esp_log.h"

#include "monitor_ll.h"
#include "ads131e0x.h"

int32_t mon_adc_init(void)
{
    uint32_t retry = 3;
    int32_t err;

open_adc:
    err = ads131e0x_open();
    if(err < 0)
    {   
        if(retry == 0)
        {
            err = -1;
            goto exit;
        }

        retry--;
        goto open_adc;
    }

    err = ads131e0x_calibrate_offset();
    if(err < 0)
    {
        ads131e0x_close();
        err = -2;
        goto exit;
    }

    return 0;

exit:
    return err;
    
}

int32_t mon_adc_start(void)
{
    return ads131e0x_start_acquisition();
}


int32_t mon_adc_stop(void)
{
    return ads131e0x_stop_acquisition();
}


int32_t mon_adc_read_mv(float *mv, uint8_t num)
{
    if(num > ADS131E0X_CH_NUM)
    {
        return -1;
    }

    int32_t buf[ADS131E0X_CH_NUM];
    int32_t err;
    err = ads131e0x_read_data(buf, 1, 0);
    if(err < 0)
    {
        return -2;
    }

    // ESP_LOGI("monitor_ll", "address: %x, %d %d %d %d", (uint32_t)buf, buf[0], buf[1], buf[2], buf[3]);

    for(uint32_t i=0; i<num; i++)
    {
        *mv++ = buf[i] * 2500.0 / 0x7fffff;
    }

    return 0;
}