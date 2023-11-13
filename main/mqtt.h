#ifndef __MQTT_H__
#define __MQTT_H__

#define ESP_MQTT_USERNAME       CONFIG_ESP_MQTT_USERNAME
#define ESP_MQTT_PASSWORD       CONFIG_ESP_MQTT_PASSWORD

void mqtt5_init(void);

#endif // __MQTT_H__