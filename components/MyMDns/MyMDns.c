/* MDNS-SD Query and advertise Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "mdns.h"
#include "netdb.h"

#ifdef CONFIG_M_DNS_EN

static const char * TAG = "MymDns";

/** Generate host name based on sdkconfig, optionally adding a portion of MAC address to it.
 *  @return host name string allocated from the heap
 */
// static char* generate_hostname(const char *pHostName)
// {
//     uint8_t mac[6];
//     char   *hostname;
//     esp_read_mac(mac, ESP_MAC_WIFI_STA);
//     if (-1 == asprintf(&hostname, "%s-%02X%02X%02X", pHostName, mac[3], mac[4], mac[5])) {
//         abort();
//     }
//     return hostname;
// }

static void initialise_mdns(const char *pHostName)
{
    //initialize mDNS
    ESP_ERROR_CHECK( mdns_init() );
    //set mDNS hostname (required if you want to advertise services)
    ESP_ERROR_CHECK( mdns_hostname_set(pHostName) );
    ESP_LOGI(TAG, "mdns hostname set to: [%s.local]", pHostName);
    //set default mDNS instance name
    ESP_ERROR_CHECK( mdns_instance_name_set(pHostName) );

    //structure with TXT records
    mdns_txt_item_t serviceTxtData[3] = {
        {"board", "esp32"},
        {"u", "user"},
        {"p", "password"}
    };

    //initialize service
    ESP_ERROR_CHECK( mdns_service_add("ESP32-WebServer", "_http", "_tcp", 80, serviceTxtData, 3) );
    ESP_ERROR_CHECK( mdns_service_add("ESP32-WebServer", "_http", "_udp", 80, serviceTxtData, 3) );
}

void MyMDnsInit(const char *pHostName)
{
    initialise_mdns(pHostName);
}

#endif