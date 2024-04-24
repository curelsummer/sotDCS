#include <stdint.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "freertos/semphr.h"

#include "spi_mng.h"
#include "ads131e0x_hal.h"

#define LCD_SPI_MOSI_NUM        (33)
#define LCD_SPI_SCLK_NUM        (32)
#define LCD_SPI_CS_NUM          (4)
#define LCD_SPI_RS_NUM          (2)

#define ADS131E0X_SPI_MISO_NUM        (25)
#define ADS131E0X_SPI_MOSI_NUM        (33)
#define ADS131E0X_SPI_SCLK_NUM        (32)
#define ADS131E0X_SPI_CS_NUM          (26)


static SemaphoreHandle_t spi_mutex = NULL;

static int32_t spi_mng_take(uint32_t ms);
static int32_t spi_mng_give(void);

int32_t spi_mng_bus_init(void)
{
    int32_t ret;
    esp_err_t err;

    if(spi_mutex == NULL)
    {   
        spi_bus_config_t buscfg={
        .miso_io_num = SPI_MISO_NUM,
        .mosi_io_num = SPI_MOSI_NUM,
        .sclk_io_num = SPI_SCLK_NUM,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1};

        err = spi_bus_initialize(VSPI_HOST, &buscfg, 2);

        if(err != ESP_OK)
        {
            ret = -1;
            goto exit;
        }
        
        spi_mutex = xSemaphoreCreateMutex();
        if(spi_mutex == NULL)
        {
            ret = -2;
            goto exit;
        }

        ads131e0x_hal_init();
        vTaskDelay(pdMS_TO_TICKS(10));
        ads131e0x_hal_reset_disable();
        
        return 0;
    }
    else
    {
        return 1;
    }

exit:
    if(spi_mutex != NULL)
    {
        vSemaphoreDelete(spi_mutex);
        spi_mutex = NULL;
    }

    return ret;
}

static int32_t spi_mng_take(uint32_t ms)
{
    if(spi_mutex == NULL)
    {
        return -1;
    }

    if(pdTRUE != xSemaphoreTake( spi_mutex, pdMS_TO_TICKS(ms)))
    {
        return -2;
    }

    return 0;
}

static int32_t spi_mng_give(void)
{
    if(spi_mutex == NULL)
    {
        return -1;
    }

    if(pdTRUE != xSemaphoreGive( spi_mutex ))
    {
        return -2;
    }

    return 0;
}


int32_t spi_mng_bus_add_dev(spi_device_interface_config_t *dev_cfg, spi_device_handle_t *dev_handle)
{   
    int32_t ret;
    esp_err_t err;

    ret = spi_mng_take(3000);
    if(ret < 0)
    {
        return -1;
    }

    err = spi_bus_add_device(VSPI_HOST, dev_cfg, dev_handle);

    spi_mng_give();

    if(ESP_OK != err)
    {
        return -2;
    }

    return 0;
}

int32_t spi_mng_remove_dev(spi_device_handle_t handle)
{
    int32_t ret;
    esp_err_t err;

    ret = spi_mng_take(3000);
    if(ret < 0)
    {
        return -1;
    }

    err = spi_bus_remove_device(handle);

    spi_mng_give();

    if(ESP_OK != err)
    {
        return -2;
    }

    return 0;
}

int32_t spi_mng_device_polling_transmit(spi_device_handle_t dev_handle, spi_transaction_t *trans, uint32_t timeout_ms)
{
    int32_t ret = spi_mng_take(timeout_ms);
    if(ret < 0)
    {
        return -1;
    }

    esp_err_t err = spi_device_polling_transmit(dev_handle, trans);

    spi_mng_give();

    if(err != ESP_OK)
    {
        return -2;
    }

    return 0;
}