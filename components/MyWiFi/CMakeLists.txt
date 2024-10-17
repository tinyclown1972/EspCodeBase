idf_component_register(SRCS "MyWiFi.c"
                    INCLUDE_DIRS .
                    REQUIRES console esp_wifi nvs_flash Utils)
