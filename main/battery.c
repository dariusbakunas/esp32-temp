#include "driver/i2c_master.h"
#include <esp_log.h>

#include "battery.h"

static const char *TAG = "BATTERY";

i2c_master_dev_handle_t dev_handle;

esp_err_t read_16(uint8_t address, uint16_t* result) {
    bool success = false;
    uint8_t retries = 3;
    uint8_t addr[2] = {address };
    uint8_t buffer[2] = { 0, 0 };

    while ((success == false) && (retries > 0))
    {
        esp_err_t err = i2c_master_transmit_receive(dev_handle, addr, sizeof(addr), buffer, 2, -1);

        if (err == ESP_OK) {
            *result = ((uint16_t)buffer[0] << 8) | buffer[1];
            success = true;
        } else {
            ESP_LOGW(TAG, "Failed to read battery monitor register: %X", address);
            retries--;
            esp_rom_delay_us( 500 );
        }
    }

    return success ? ESP_OK : ESP_ERR_INVALID_ARG;
}

esp_err_t read_voltage(float* voltage) {
    uint16_t buffer = 0;
    esp_err_t err = read_16(MAX17048_VCELL, &buffer);

    if (err != ESP_OK) {
        return err;
    }

    float _full_scale = 5.12f; // specific to MAX17048
    float divider = 65536.0f / _full_scale;
    *voltage = (((float)buffer) / divider);
    return ESP_OK;
}

esp_err_t read_soc(float* percent) {
    uint16_t buffer = 0;
    esp_err_t err = read_16(MAX17048_SOC, &buffer);

    if (err != ESP_OK) {
        return err;
    }

    *percent = (float)((buffer & 0xFF00) >> 8);
    *percent += ((float)(buffer & 0x00FF)) / 256.0f;

    return ESP_OK;
}

esp_err_t battery_init(void) {
    int i2c_master_port = I2C_MASTER_NUM;

    i2c_master_bus_config_t i2c_mst_config = {
            .clk_source = I2C_CLK_SRC_DEFAULT,
            .i2c_port = i2c_master_port,                     // auto assign
            .scl_io_num = I2C_MASTER_SCL_IO,
            .sda_io_num = I2C_MASTER_SDA_IO,
            .glitch_ignore_cnt = 7,
            .flags.enable_internal_pullup = true,
    };

    i2c_master_bus_handle_t bus_handle;
    ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_mst_config, &bus_handle));

    i2c_device_config_t dev_cfg = {
            .dev_addr_length = I2C_ADDR_BIT_LEN_7,
            .device_address = MAX17048_SENSOR_ADDR,
            .scl_speed_hz = I2C_MASTER_FREQ_HZ,
    };

    ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &dev_cfg, &dev_handle));

    uint8_t retries = 3;
    bool success = false;

    while ((success == false) && (retries > 0)){
        esp_err_t err = i2c_master_probe(bus_handle, dev_cfg.device_address, -1);

        if (err == ESP_OK) {
            success = true;
            ESP_LOGI(TAG, "Battery monitor connected");
        } else {
            retries--;
            ESP_LOGW(TAG, "Battery monitor not connected, retrying...");
            esp_rom_delay_us( 1000 );
        }
    }

    if (success) {
        uint16_t version = 0;
        float voltage = 0;
        float soc = 0;
        ESP_ERROR_CHECK(read_16(MAX17048_VERSION_REG_ADDR, &version));
        ESP_LOGI(TAG, "VERSION = %X", version);

        ESP_ERROR_CHECK(read_voltage( &voltage));
        ESP_LOGI(TAG, "Battery voltage: %.2fV", voltage);

        ESP_ERROR_CHECK(read_soc( &soc));
        ESP_LOGI(TAG, "Battery SOC: %.2f%%", soc);
    } else {
        ESP_LOGE(TAG, "Battery monitor not found");
    }

    ESP_ERROR_CHECK(i2c_master_bus_rm_device(dev_handle));
    ESP_ERROR_CHECK(i2c_del_master_bus(bus_handle));

    return ESP_OK;
}