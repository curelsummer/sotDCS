#ifndef _SPI_MNG_H
#define _SPI_MNG_H

#include <stdint.h>
#include "driver/spi_master.h"

#define SPI_MISO_NUM        (25)
#define SPI_MOSI_NUM        (33)
#define SPI_SCLK_NUM        (32)


#ifdef __cplusplus
 extern "C" {
#endif 

int32_t spi_mng_bus_init(void);

int32_t spi_mng_bus_add_dev(spi_device_interface_config_t *dev_cfg, spi_device_handle_t *dev_handle);
int32_t spi_mng_remove_dev(spi_device_handle_t handle);

int32_t spi_mng_device_polling_transmit(spi_device_handle_t dev_handle, spi_transaction_t *trans, uint32_t timeout_ms);


#ifdef __cplusplus
}
#endif

#endif /* _SPI_MNG_H */
