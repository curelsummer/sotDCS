#include <stdio.h>
#include <stdint.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"

#include "nvs_flash.h"
#include "nvs.h"


#include "cur_output.h"
#include "monitor.h"

#define TAG "calibration"

#define PNT_N (101)

#define NVS_NAMESPACE      "std"
#define NVS_CALI_KEY       "cali_cur"


static int32_t save_cali_cur_para(cali_cur_para_t *para);



int32_t cal_start(void)
{
    int32_t ret;

    // ret = cur_init();
    // if(ret < 0)
    // {
    //     ret = -1;
    //     goto exit;
    // }

    ret = cur_open(1000);
    if(ret < 0)
    {
        ESP_LOGI(TAG, "cur open timeout.");
        goto exit;
    }

    cur_enable_output();

    vTaskDelay(pdMS_TO_TICKS(1000));

    uint32_t i;
    float cali_input[PNT_N];
    float cali_output[PNT_N];
    float step = 2000 / (PNT_N - 1);
    float set_v = 0;
    for(i = 0; i<PNT_N; i++)
    {
        cali_input[i] = set_v;
        set_v += step;
    }

    float input_mean = 0;
    float output_mean = 0;

    for(i = 0; i<PNT_N; i++)
    {
        ESP_LOGI(TAG, "input: %.0f", cali_input[i]);

        cur_set_ua(cali_input[i], 0);

        do
        {
            vTaskDelay(pdMS_TO_TICKS(1000));
        }while(mon_get_current(&cali_output[i]) < 0);


        mon_get_current(&cali_output[i]);

        input_mean += cali_input[i];
        output_mean += cali_output[i];

        ESP_LOGI(TAG, "output: %f", cali_output[i]);
    }

    cur_close();

    input_mean /= i;
    output_mean /= i;

    float num = 0;
    float den = 0;
    for(i = 0; i<PNT_N; i++)
    {
        num += (cali_input[i] - input_mean) * (cali_output[i] - output_mean);
        den += (cali_input[i] - input_mean) * (cali_input[i] - input_mean);
    }

    float b1 = num / den;
    float b0 = output_mean - b1 * input_mean;

    cali_cur_para_t para;
    para.a1 = 1/b1;
    para.a0 = 0 - b0/b1;

    ESP_LOGI(TAG, "y = %f * x + %f", b1, b0);
    ESP_LOGI(TAG, "a0 = %f, a1 = %f",  para.a0,  para.a1);

    ret = save_cali_cur_para(&para);
    if(ret < 0)
    {
        ESP_LOGE(TAG, "save cali para error.");
        goto exit;
    }

    ESP_LOGI(TAG, "save cali para success.");

    ret = cur_open(1000);
    if(ret < 0)
    {
        goto exit;
        ESP_LOGI(TAG, "cur open 2 timeout.");
    }

    cur_enable_output();

    vTaskDelay(pdMS_TO_TICKS(1000));

    float test[] = {0, 100, 500, 1000, 2000, 2500};

    for(i = 0; i<(sizeof(test)/sizeof(float)); i++)
    {
        ESP_LOGI(TAG, "input: %.0f", test[i]);

        cur_set_ua(test[i], 1);
        vTaskDelay(pdMS_TO_TICKS(1000));
        mon_get_current(&test[i]);

        ESP_LOGI(TAG, "output: %f", test[i]);
    }

    ret = 0;

exit:
    cur_close();

    return ret;
}



static int32_t save_cali_cur_para(cali_cur_para_t *para)
{
    if(para == NULL)
    {
        return -1;
    }

    int32_t ret = 0;
    esp_err_t err;
    nvs_handle nvs_handle;

    err = nvs_flash_init();
    if(ret != ESP_OK)
    {
        ESP_LOGE(TAG, "NVS init failed.");
        return -2;
    }

    err = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &nvs_handle);
    if(err != ESP_OK)
    {
        ESP_LOGE(TAG, "save_cali_cur_para: open nvs failed.(%d)", err);
        ret = -3;
        goto exit;
    }

    err = nvs_set_blob(nvs_handle, NVS_CALI_KEY, para, sizeof(cali_cur_para_t));
    if(err != ESP_OK)
    {
        ESP_LOGE(TAG, "save_cali_cur_para: set calibration failed.(%d)", err);
        ret = -4;
        goto exit;
    }

    err = nvs_commit(nvs_handle);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "save_cali_cur_para: nvs commit failed.(%d)", err);
        ret = -5;
        goto exit;
    }

exit:
    nvs_close(nvs_handle);
    return ret;
}