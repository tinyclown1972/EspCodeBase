/* OTA example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include "s_ota.h"
#include "argtable3/argtable3.h"
#include "esp_console.h"
#include <regex.h>
#include <stdio.h>

#ifdef CONFIG_UTILS_EN
#include "Utils.h"
#endif

#ifdef CONFIG_OTA_EN
static char *ota_addr = "";
static const char *TAG = "OTA";

esp_err_t _http_event_handler(esp_http_client_event_t *evt)
{
    switch (evt->event_id)
    {
    case HTTP_EVENT_ERROR:
        ESP_LOGD(TAG, "HTTP_EVENT_ERROR");
        break;
    case HTTP_EVENT_ON_CONNECTED:
        ESP_LOGD(TAG, "HTTP_EVENT_ON_CONNECTED");
        break;
    case HTTP_EVENT_HEADER_SENT:
        ESP_LOGD(TAG, "HTTP_EVENT_HEADER_SENT");
        break;
    case HTTP_EVENT_ON_HEADER:
        ESP_LOGD(TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key,
                 evt->header_value);
        break;
    case HTTP_EVENT_ON_DATA:
        ESP_LOGD(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
        break;
    case HTTP_EVENT_ON_FINISH:
        ESP_LOGD(TAG, "HTTP_EVENT_ON_FINISH");
        break;
    case HTTP_EVENT_DISCONNECTED:
        ESP_LOGD(TAG, "HTTP_EVENT_DISCONNECTED");
        break;
    }
    return ESP_OK;
}

void simple_ota_example_task(void *pvParameter)
{
    ESP_LOGI(TAG, "Starting OTA");
    esp_http_client_config_t config = {
        .url = ota_addr,
        //.cert_pem = (char *)server_cert_pem_start,
        .event_handler = _http_event_handler,
        .keep_alive_enable = true,
    };

    esp_err_t ret = esp_https_ota(&config);
    if (ret == ESP_OK)
    {
        ESP_LOGI(TAG, "Firmware upgrade succeed, restrt now");
        vTaskDelay(200);
        SystemRestart();
    }
    else
    {
        ESP_LOGE(TAG, "Firmware upgrade failed");
    }
    while (1)
    {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

static void print_sha256(const uint8_t *image_hash, const char *label)
{
    char hash_print[HASH_LEN * 2 + 1];
    hash_print[HASH_LEN * 2] = 0;
    for (int i = 0; i < HASH_LEN; ++i)
    {
        sprintf(&hash_print[i * 2], "%02x", image_hash[i]);
    }
    ESP_LOGI(TAG, "%s %s", label, hash_print);
}

static void get_sha256_of_partitions(void)
{
    uint8_t sha_256[HASH_LEN] = {0};
    esp_partition_t partition;

    // get sha256 digest for bootloader
    partition.address = ESP_BOOTLOADER_OFFSET;
    partition.size = ESP_PARTITION_TABLE_OFFSET;
    partition.type = ESP_PARTITION_TYPE_APP;
    esp_partition_get_sha256(&partition, sha_256);
    print_sha256(sha_256, "SHA-256 for bootloader: ");

    // get sha256 digest for running partition
    esp_partition_get_sha256(esp_ota_get_running_partition(), sha_256);
    print_sha256(sha_256, "SHA-256 for current firmware: ");
}

void OTA_main(const char *addr)
{
    ota_addr = addr;
    get_sha256_of_partitions();

    esp_wifi_set_ps(WIFI_PS_NONE);

    xTaskCreate(&simple_ota_example_task, "ota_example_task", 8192, NULL, 5, NULL);
}

#if 0
/** Arguments used by 'join' function */
static struct
{
    struct arg_str *upgradeUrl;
    struct arg_end *end;
} OTA_join_args;

int OTACli(int argc, char **argv)
{
    int retVal = ESP_OK;
    int nerrors = arg_parse(argc, argv, (void **)&OTA_join_args);
    if (nerrors != 0)
    {
        arg_print_errors(stderr, OTA_join_args.end, argv[0]);
        return 1;
    }
    regex_t regex;
    char *pattern =
        "http://[0-9]+\\.[0-9]+\\.[0-9]+\\.[0-9]+:[0-9]+/[^[:space:]]+\\.bin";

    if (regcomp(&regex, pattern, REG_EXTENDED) != 0)
    {
        printf("Failed to compile regex\n");
        return 1;
    }

    if (regexec(&regex, OTA_join_args.upgradeUrl->sval[0], 0, NULL, 0) == 0)
    {
        printf("URL matches the pattern with .bin file format\n");
        OTA_main((OTA_join_args.upgradeUrl->sval[0]));
        retVal = ESP_OK;
    }
    else
    {
        printf("URL does not match the pattern with .bin file format\n");
        retVal = ESP_FAIL;
    }

    regfree(&regex);
    return retVal;
}

void register_OTA(void)
{
    OTA_join_args.upgradeUrl =
        arg_str1("u", "url", "<u>",
                 "url to get bin file: http://192.168.31.166:8070/main.bin");
    OTA_join_args.end = arg_end(1);

    const esp_console_cmd_t join_cmd = {.command = "ota",
                                        .help = "ota the firmware",
                                        .hint = NULL,
                                        .func = &OTACli,
                                        .argtable = &OTA_join_args};

    ESP_ERROR_CHECK(esp_console_cmd_register(&join_cmd));
}
#endif
#endif