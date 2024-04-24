#include "beep.h"




void beep_init(void)
{
    gpio_config_t io_conf;
  
    io_conf.pin_bit_mask = Buz_ctl_IO_SEL;//led pin
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.intr_type = GPIO_PIN_INTR_DISABLE;
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);
	
}


void beep_start(uint32_t ms, uint32_t n)
{
    while(n--)
    {
        gpio_set_level(Buz_ctl_IO, 0);
        vTaskDelay(pdMS_TO_TICKS(ms));
        gpio_set_level(Buz_ctl_IO, 1);
        vTaskDelay(pdMS_TO_TICKS(ms));
    }
}

// void beep_test(void)
// {
    

//         gpio_set_level(Buz_ctl_IO, 0);

//         vTaskDelay(pdMS_TO_TICKS(3000));
        
//        gpio_set_level(Buz_ctl_IO, 1);

//         vTaskDelay(pdMS_TO_TICKS(3000));
// }
/*
void leds_task(void *arg)
{
    leds_init();
    
    while(1)
    {

        gpio_set_level(Buz_ctl_IO, 0);


        vTaskDelay(500 / portTICK_RATE_MS);
        
        gpio_set_level(Buz_ctl_IO, 1);

        vTaskDelay(500 / portTICK_RATE_MS);
    }
}*/



