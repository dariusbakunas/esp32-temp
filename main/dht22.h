#ifndef __DHT22_H__
#define __DHT22_H__

#include <stdbool.h>

#define ESP_DHT_GPIO_PIN       CONFIG_ESP_DHT_GPIO_PIN

typedef struct {
    float temperature;
    float humidity;
} dht_reading_t;

extern QueueHandle_t dht_reading_queue;
esp_err_t dht_init(void);

#endif // __DHT22_H__