#ifndef __BATTERY_H__
#define __BATTERY_H__

#include <esp_err.h>

#define I2C_MASTER_NUM              0                          /*!< I2C master i2c port number, the number of i2c peripheral interfaces available will depend on the chip */
#define I2C_MASTER_SCL_IO           CONFIG_ESP_I2C_MASTER_SCL  /*!< GPIO number used for I2C master clock */
#define I2C_MASTER_SDA_IO           CONFIG_ESP_I2C_MASTER_SDA  /*!< GPIO number used for I2C master data  */
#define I2C_MASTER_FREQ_HZ          400000                     /*!< I2C master clock frequency */
#define I2C_MASTER_TX_BUF_DISABLE   0                          /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE   0                          /*!< I2C master doesn't need buffer */
#define I2C_MASTER_TIMEOUT_MS       1000

#define MAX17048_SENSOR_ADDR        0x36

// All registers contain two bytes of data and span two addresses.
// Registers which are present on the MAX17048/49 only are prefixed with MAX17048_
#define MAX17048_VCELL              0x02         /*!< R - 16-bit A/D measurement of battery voltage */
#define MAX17048_SOC                0x04         /*!< R - 16-bit state of charge (SOC) */
#define MAX17048_VERSION_REG_ADDR   0x08         /*!< Returns 2 byte version */

esp_err_t battery_init(void);

#endif