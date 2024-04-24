#ifndef __ADS131E0X_HAL_H
#define __ADS131E0X_HAL_H


#include <stdint.h>


typedef void(*drdy_cb_t)(void*);



#ifdef __cplusplus
extern "C" {
#endif



#define ADS131E0X_SPI_MISO_NUM        (25)
#define ADS131E0X_SPI_MOSI_NUM        (33)
#define ADS131E0X_SPI_SCLK_NUM        (32)
#define ADS131E0X_SPI_CS_NUM          (26)
 

//#define ADS131E0X_PWDN_NUM            (4)
#define ADS131E0X_RST_NUM             (12)
#define ADS131E0X_START_NUM           (14)
#define ADS131E0X_DRDY_NUM            (27)

//#define ADS131E0X_PWDN_SEL            (1ULL<<ADS131E0X_PWDN_NUM)
#define ADS131E0X_RST_SEL             (1ULL<<ADS131E0X_RST_NUM)
#define ADS131E0X_START_SEL           (1ULL<<ADS131E0X_START_NUM)
#define ADS131E0X_DRDY_SEL            (1ULL<<ADS131E0X_DRDY_NUM)


int32_t ads131e0x_hal_init(void);

// uint8_t ads131e0x_hal_read_write_byte(uint8_t tx_data);
int32_t ads131e0x_hal_read_write(uint8_t *tx_buf, uint8_t *rx_buf, uint32_t length, uint32_t timeout_ms);

// int32_t ads131e0x_hal_power_on(void);
// int32_t ads131e0x_hal_power_off(void);

int32_t ads131e0x_hal_reset_enable(void);
int32_t ads131e0x_hal_reset_disable(void);

int32_t ads131e0x_hal_start_enable(void);
int32_t ads131e0x_hal_start_disable(void);

int32_t ads131e0x_drdy_interrupt_enable(drdy_cb_t cb);
int32_t ads131e0x_drdy_interrupt_disable (void);


#ifdef __cplusplus
}
#endif

#endif

