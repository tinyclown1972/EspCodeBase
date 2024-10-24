/* Simple HTTP Server Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <esp_event.h>
#include <esp_log.h>
#include "esp_netif.h"
#include "protocol_examples_common.h"
#include <esp_http_server.h>
#include "cJSON.h"
#ifdef CONFIG_RTE_EN
#include "pump.h"
#endif
#ifdef CONFIG_OTA_EN
#include "s_ota.h"
#endif

/* A simple example that demonstrates how to create GET and POST
 * handlers for the web server.
 */

static const char *TAG = "MyWeb";

/* An HTTP GET handler */
static esp_err_t home_get_handler(httpd_req_t *req)
{
    httpd_resp_set_type(req, "text/html"); // 设置响应类型为HTML
    httpd_resp_set_hdr(req, "Content-Encoding", "utf-8"); // 设置内容编码为UTF-8
    extern const unsigned char html_page_start[] asm("_binary_web_page_html_start");
    extern const unsigned char html_page_end[] asm("_binary_web_page_html_end");
    httpd_resp_sendstr(req, (const char *)html_page_start);

    return ESP_OK;
}

static const httpd_uri_t hello = {
    .uri       = "/",
    .method    = HTTP_GET,
    .handler   = home_get_handler,
    .user_ctx  = NULL
};

static esp_err_t api_request_handler(const char *pParam)
{
    esp_err_t error = ESP_OK;

    if(0 == strcmp("Reboot", pParam))
    {
        /* Reboot */
        ESP_LOGI(TAG, "Reboot request");
        vTaskDelay(1000/portTICK_PERIOD_MS);
        SystemRestart();
    }
    else if(0 == strcmp("PumpOn", pParam))
    {
        /* PumpOn */
        ESP_LOGI(TAG, "request to add water");
        RTESetgePumpStateMachine(PUMP_RUN_MANUAL);
    }
    else if(0 == strcmp("PumpOff", pParam))
    {
        /* PumpOff */
        ESP_LOGI(TAG, "request to stop");
        RTESetgePumpStateMachine(PUMP_INIT);
    }
    else if(0 == strcmp("NightTime", pParam))
    {
        /* PumpOff */
        ESP_LOGI(TAG, "request to stop in night");
        RTESetgePumpStateMachine(PUMP_END);
    }
    else if(0 == strcmp("DayTime", pParam))
    {
        /* PumpOff */
        ESP_LOGI(TAG, "request to start in day time");
        RTESetgePumpStateMachine(PUMP_INIT);
    }
    else
    {
        /* Unknown */
        ESP_LOGI(TAG, "Unknown api request");
        error = ESP_FAIL;
    }

    return error;
}

/* An HTTP GET handler */
static esp_err_t api_get_handler(httpd_req_t *req)
{
    char*  buf;
    size_t buf_len;

    /* Read URL query string length and allocate memory for length + 1,
     * extra byte for null termination */
    buf_len = httpd_req_get_url_query_len(req) + 1;
    if (buf_len > 1) {
        buf = malloc(buf_len);
        if (httpd_req_get_url_query_str(req, buf, buf_len) == ESP_OK) {
            char param[32];
            /* Get value of expected key from query string */
            if (httpd_query_key_value(buf, "cmd", param, sizeof(param)) == ESP_OK) {
                // ESP_LOGI(TAG, "Found URL cmd => %s", param);
                if(0 == strcmp("waterLevel", param))
                {
                    /* return uint8_t current water level via RTEGetWaterLevel() */
                    uint8_t waterLevel = RTEGetWaterLevel();
                    char resp[64];
                    snprintf(resp, sizeof(resp), "{\"waterLevel\": %d}", waterLevel);
                    httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
                }
                else if(0 == strcmp("waterLevelHigh", param))
                {
                    /* return uint8_t current water level via RTEGetWaterLevel() */
                    char resp[64];
                    snprintf(resp, sizeof(resp), "{\"waterLevel\": %d}", RTEGetHighThreshold());
                    httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
                }
                else if(0 == strcmp("waterLevelLow", param))
                {
                    /* return uint8_t current water level via RTEGetWaterLevel() */
                    char resp[64];
                    snprintf(resp, sizeof(resp), "{\"waterLevel\": %d}", RTEGetLowThreshold());
                    httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
                }
                else
                {
                    if(ESP_OK != api_request_handler(param))
                    {
                        char resp[64];
                        snprintf(resp, sizeof(resp), "Failed to request");
                        httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
                        ESP_LOGE(TAG, "Request Api failed");
                    }
                    else
                    {
                        char resp[64];
                        snprintf(resp, sizeof(resp), "Succeed to request");
                        httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
                    }
                }
            }
        }
        free(buf);
    }

    return ESP_OK;
}

static const httpd_uri_t api = {
    .uri       = "/api",
    .method    = HTTP_GET,
    .handler   = api_get_handler,
    /* Let's pass response string in user
     * context to demonstrate it's usage */
    .user_ctx  = "Api interface"
};

#ifdef CONFIG_OTA_EN
char *urlDecode(const char *encoded_url)
{
    int len = strlen(encoded_url);
    char *decoded_url = (char *)malloc((len + 1) * sizeof(char));
    int j = 0;

    for (int i = 0; i < len; ++i)
    {
        if (encoded_url[i] == '%' && i + 2 < len)
        {
            char hex[3] = {encoded_url[i + 1], encoded_url[i + 2], '\0'};
            long hex_value = strtol(hex, NULL, 16);
            decoded_url[j++] = (char)hex_value;
            i += 2;
        }
        else if (encoded_url[i] == '+')
        {
            decoded_url[j++] = ' ';
        }
        else
        {
            decoded_url[j++] = encoded_url[i];
        }
    }

    decoded_url[j] = '\0';
    return decoded_url;
}

esp_err_t ota_post_handler(httpd_req_t *req)
{
    static char input_buffer[256];
    static char output_buffer[256];
    size_t recv_size = MIN(req->content_len, 255);
    httpd_req_recv(req, input_buffer, recv_size);
    input_buffer[recv_size] = '\0'; // 确保输入缓冲区以空字符结尾

    printf("ota input : %s\r\n", input_buffer);

    httpd_resp_set_type(req, "text/html");
    strcpy(output_buffer, "Ok"); // 返回空响应
    httpd_resp_send(req, output_buffer, strlen(output_buffer));

    char *input = urlDecode(input_buffer);

    OTA_main(input);
    return ESP_OK;
}

httpd_uri_t ota_post_uri = {
    .uri = "/ota",
    .method = HTTP_POST,
    .handler = ota_post_handler,
    .user_ctx = NULL
};
#endif

esp_err_t waterlevel_post_handler(httpd_req_t *req)
{
    static char input_buffer[256];
    static char output_buffer[256];
    size_t recv_size = MIN(req->content_len, 255);
    httpd_req_recv(req, input_buffer, recv_size);
    input_buffer[recv_size] = '\0'; // 确保输入缓冲区以空字符结尾

    // 使用cJSON解析JSON数据
    cJSON *json = cJSON_Parse(input_buffer);
    if (json == NULL) {
        ESP_LOGE(TAG, "Error before: [%s]\r\n", cJSON_GetErrorPtr());
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }

    // 提取High和Low的值
    int high_value = 0;
    int low_value = 0;
    if (cJSON_HasObjectItem(json, "High")) {
        cJSON *high_item = cJSON_GetObjectItem(json, "High");
        if (high_item->type == cJSON_Number) {
            high_value = high_item->valueint;
        }
    }
    if (cJSON_HasObjectItem(json, "Low")) {
        cJSON *low_item = cJSON_GetObjectItem(json, "Low");
        if (low_item->type == cJSON_Number) {
            low_value = low_item->valueint;
        }
    }

    if((high_value <= 0) || (low_value <= 0))
    {
        ESP_LOGE(TAG, "Invalid Low or High water Level");
        strcpy(output_buffer, "Invalid Low or High water Level");
    }
    else
    {
        RTESetHighThreshold(high_value);
        RTESetLowThreshold(low_value);        
        ESP_LOGI(TAG, "Set High and Low water Level to %d %d", high_value, low_value);
        strcpy(output_buffer, "Ok");
    }

    // 清理cJSON对象
    cJSON_Delete(json);

    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, output_buffer, strlen(output_buffer));

    return ESP_OK;
}

httpd_uri_t waterlevel_post_uri = {
    .uri = "/waterlevel",
    .method = HTTP_POST,
    .handler = waterlevel_post_handler,
    .user_ctx = NULL
};

/* This handler allows the custom error handling functionality to be
 * tested from client side. For that, when a PUT request 0 is sent to
 * URI /ctrl, the /hello and /echo URIs are unregistered and following
 * custom error handler http_404_error_handler() is registered.
 * Afterwards, when /hello or /echo is requested, this custom error
 * handler is invoked which, after sending an error message to client,
 * either closes the underlying socket (when requested URI is /echo)
 * or keeps it open (when requested URI is /hello). This allows the
 * client to infer if the custom error handler is functioning as expected
 * by observing the socket state.
 */
esp_err_t http_404_error_handler(httpd_req_t *req, httpd_err_code_t err)
{
    if (strcmp("/hello", req->uri) == 0) {
        httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "/hello URI is not available");
        /* Return ESP_OK to keep underlying socket open */
        return ESP_OK;
    } else if (strcmp("/api", req->uri) == 0) {
        httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "/api URI is not available");
        /* Return ESP_FAIL to close underlying socket */
        return ESP_FAIL;
    }
#ifdef CONFIG_OTA_EN
    else if (strcmp("/ota", req->uri) == 0) {
        httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "/ota URI is not available");
        /* Return ESP_FAIL to close underlying socket */
        return ESP_FAIL;
    }
#endif
    /* For any other URI send 404 and close socket */
    httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "Some 404 error message");
    return ESP_FAIL;
}

static httpd_handle_t start_webserver(void)
{
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.lru_purge_enable = true;

    // Start the httpd server
    ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);
    if (httpd_start(&server, &config) == ESP_OK) {
        // Set URI handlers
        ESP_LOGI(TAG, "Registering URI handlers");
        httpd_register_uri_handler(server, &hello);
        httpd_register_uri_handler(server, &api);
        httpd_register_uri_handler(server, &waterlevel_post_uri);
#ifdef CONFIG_OTA_EN
        httpd_register_uri_handler(server, &ota_post_uri);
#endif
        return server;
    }

    ESP_LOGI(TAG, "Error starting server!");
    return NULL;
}

static void stop_webserver(httpd_handle_t server)
{
    // Stop the httpd server
    httpd_stop(server);
}

static void disconnect_handler(void* arg, esp_event_base_t event_base,
                               int32_t event_id, void* event_data)
{
    httpd_handle_t* server = (httpd_handle_t*) arg;
    if (*server) {
        ESP_LOGI(TAG, "Stopping webserver");
        stop_webserver(*server);
        *server = NULL;
    }
}

static void connect_handler(void* arg, esp_event_base_t event_base,
                            int32_t event_id, void* event_data)
{
    httpd_handle_t* server = (httpd_handle_t*) arg;
    if (*server == NULL) {
        ESP_LOGI(TAG, "Starting webserver");
        *server = start_webserver();
    }
}


void MyWebInit(void)
{
    static httpd_handle_t server = NULL;
    /* Start the server for the first time */
    server = start_webserver();
}
