#include <stdint.h>
#include <string.h>

#include "driver/spi_master.h"
#include "driver/gpio.h"


#define dac8562_SPI_MOSI_NUM        (23)
#define dac8562_SPI_SCLK_NUM        (18)
#define dac8562_SPI_CS_NUM          (5)


#define dac8562_CLR_NUM             (9)



static spi_device_handle_t spi_dac8562;


 

/*
int32_t dac8562_hal_CLR_enable(void)
{
    gpio_set_level(dac8562_CLR_NUM, 0);
    return 0;
}
*/
static int32_t dac8562_hal_CLR_disable(void)
{
    //gpio_set_level(dac8562_CLR_NUM, 1);
    return 0;
}



static int32_t dac8562_spi_init(void)
{
    esp_err_t err;

    spi_bus_config_t buscfg={
        .miso_io_num = -1,
        .mosi_io_num = dac8562_SPI_MOSI_NUM,
        .sclk_io_num = dac8562_SPI_SCLK_NUM,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1
    };

    err = spi_bus_initialize(HSPI_HOST, &buscfg, 0);
    if(err != ESP_OK)
    {
        return -1;
    }

    spi_device_interface_config_t devcfg={
        .clock_speed_hz = 5000*1000, 
        .mode = 1, 
        .spics_io_num = dac8562_SPI_CS_NUM, 
        .queue_size = 4, //We want to be able to queue 4 transactions at a time
        .cs_ena_posttrans = 10
    };

    err = spi_bus_add_device(HSPI_HOST, &devcfg, &spi_dac8562);
    if(err != ESP_OK)
    {
        return -2;
    }

    return 0;
}

int32_t dac8562_hal_init(void)
{
    esp_err_t err;
    
    // gpio_config_t io_conf;

    // dac8562_hal_CLR_disable();
    // io_conf.pin_bit_mask = 1ULL<<dac8562_CLR_NUM;//CLR pin
    // io_conf.mode = GPIO_MODE_OUTPUT;
    // io_conf.intr_type = GPIO_PIN_INTR_DISABLE;
    // io_conf.pull_down_en = 0;
    // io_conf.pull_up_en =0;
    // gpio_config(&io_conf);

    err = dac8562_spi_init();
    if(err != ESP_OK)
    {
        return -3;
    } 

    return 0;
}


int32_t dac8562_hal_write_bytes(uint8_t *w_data, uint16_t length)
{
    if(w_data == NULL)
    {
        printf("null pointer error in dac8562_readwrite_regs.\r\n");
        return -1;
    }

    spi_transaction_t trans;

    memset(&trans, 0, sizeof(spi_transaction_t));
    trans.flags = 0;
    trans.length = length*8;
    trans.tx_buffer = w_data;

    esp_err_t ret = spi_device_polling_transmit(spi_dac8562, &trans);
    assert(ret==ESP_OK);
    
    return 0;
}




