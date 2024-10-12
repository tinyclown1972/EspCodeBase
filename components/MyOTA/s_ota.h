#ifndef S_OTA_H
#define S_OTA_H

#include "esp_event.h"
#include "esp_http_client.h"
#include "esp_https_ota.h"
#include "esp_log.h"
#include "esp_ota_ops.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "protocol_examples_common.h"
#include "string.h"
#include <sys/socket.h>

#define HASH_LEN 32

#define CONFIG_EXAMPLE_FIRMWARE_UPGRADE_URL                                    \
    "http://192.168.31.166:8070/main.bin"

#define OTA_URL_SIZE 256

esp_err_t _http_event_handler(esp_http_client_event_t *evt);

void simple_ota_example_task(void *pvParameter);

void OTA_main(const char *addr);

void register_OTA(void);

#endif
