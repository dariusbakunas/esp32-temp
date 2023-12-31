#include <stdio.h>

#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_log.h"

#include "wifi.h"
#include "mqtt.h"
#include "dht22.h"
#include "battery.h"

static const char *TAG = "TempSensor";

void app_main(void)
{
    ESP_LOGI(TAG, "[APP] Startup..");
    ESP_LOGI(TAG, "[APP] Free memory: %" PRIu32 " bytes", esp_get_free_heap_size());
    ESP_LOGI(TAG, "[APP] IDF version: %s", esp_get_idf_version());

    esp_log_level_set("*", ESP_LOG_INFO);

    //Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    ESP_ERROR_CHECK(wifi_init_sta());
    ESP_ERROR_CHECK(mqtt5_init());
    ESP_ERROR_CHECK(dht_init());
    ESP_ERROR_CHECK(battery_init());
}
