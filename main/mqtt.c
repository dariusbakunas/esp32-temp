#include "mqtt.h"
#include "esp_log.h"
#include "esp_event.h"
#include "mqtt_client.h"

static const char *TAG = "MQTT5";

static esp_mqtt5_user_property_item_t user_property_arr[] = {
        {"board", "esp32"},
        {"u", "user"},
        {"p", "password"}
};

#define USE_PROPERTY_ARR_SIZE   sizeof(user_property_arr)/sizeof(esp_mqtt5_user_property_item_t)

/*
 * @brief Event handler registered to receive MQTT events
 *
 *  This function is called by the MQTT client event loop.
 *
 * @param handler_args user data registered to the event.
 * @param base Event base for the handler(always MQTT Base in this example).
 * @param event_id The id for the received event.
 * @param event_data The data for the event, esp_mqtt_event_handle_t.
 */
static void event_handler(void *args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%" PRIi32, base, event_id);
    esp_mqtt_event_handle_t event = event_data;
    esp_mqtt_client_handle_t client = event->client;
    int msg_id;

    ESP_LOGD(TAG, "free heap size is %" PRIu32 ", minimum %" PRIu32, esp_get_free_heap_size(), esp_get_minimum_free_heap_size());
    switch ((esp_mqtt_event_id_t)event_id) {
        case MQTT_EVENT_CONNECTED:
            break;
        case MQTT_EVENT_ANY:
            break;
        case MQTT_EVENT_ERROR:
            break;
        case MQTT_EVENT_DISCONNECTED:
            break;
        case MQTT_EVENT_SUBSCRIBED:
            break;
        case MQTT_EVENT_UNSUBSCRIBED:
            break;
        case MQTT_EVENT_PUBLISHED:
            break;
        case MQTT_EVENT_DATA:
            break;
        case MQTT_EVENT_BEFORE_CONNECT:
            break;
        case MQTT_EVENT_DELETED:
            break;
        case MQTT_USER_EVENT:
            break;
        default:
            ESP_LOGI(TAG, "Other event id:%d", event->event_id);
            break;
    }
}

void mqtt5_init(void) {
    esp_mqtt5_connection_property_config_t connect_property = {
            .session_expiry_interval = 10,
            .maximum_packet_size = 1024,
            .receive_maximum = 65535,
            .topic_alias_maximum = 2,
            .request_resp_info = true,
            .request_problem_info = true,
            .will_delay_interval = 10,
            .payload_format_indicator = true,
            .message_expiry_interval = 10,
    };

    esp_mqtt_client_config_t mqtt5_cfg = {
            .broker.address.uri = CONFIG_ESP_BROKER_URL,
            .session.protocol_ver = MQTT_PROTOCOL_V_5,
            .network.disable_auto_reconnect = true,
            .credentials.username = CONFIG_ESP_MQTT_USERNAME,
            .credentials.authentication.password = CONFIG_ESP_MQTT_PASSWORD,
            .session.last_will.topic = "/topic/will",
            .session.last_will.msg = "i will leave",
            .session.last_will.msg_len = 12,
            .session.last_will.qos = 1,
            .session.last_will.retain = true,
    };

    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt5_cfg);

    /* Set connection properties and user properties */
    esp_mqtt5_client_set_user_property(&connect_property.user_property, user_property_arr, USE_PROPERTY_ARR_SIZE);
    esp_mqtt5_client_set_user_property(&connect_property.will_user_property, user_property_arr, USE_PROPERTY_ARR_SIZE);
    esp_mqtt5_client_set_connect_property(client, &connect_property);

    /* If you call esp_mqtt5_client_set_user_property to set user properties, DO NOT forget to delete them.
     * esp_mqtt5_client_set_connect_property will malloc buffer to store the user_property, and you can delete it after
     */
    esp_mqtt5_client_delete_user_property(connect_property.user_property);
    esp_mqtt5_client_delete_user_property(connect_property.will_user_property);

    /* The last argument may be used to pass data to the event handler, in this example mqtt_event_handler */
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, event_handler, NULL);
    esp_mqtt_client_start(client);
}