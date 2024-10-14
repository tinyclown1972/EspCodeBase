idf_component_register(SRCS "MyWeb.c"
                    INCLUDE_DIRS .
                    REQUIRES esp_http_server protocol_examples_common RTE Pump MyOTA json Utils
                    EMBED_TXTFILES "web_page.html")
