idf_component_register(SRCS "s_ota.c"
                    INCLUDE_DIRS .
                    REQUIRES console RTE esp_http_client esp_https_ota app_update nvs_flash protocol_examples_common)

set(EXTRA_COMPONENT_DIRS $ENV{IDF_PATH}/examples/common_components/protocol_examples_common)
