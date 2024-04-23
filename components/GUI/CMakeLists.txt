file(GLOB_RECURSE DEMO_SOURCES src/*.c)

idf_component_register(SRCS ${DEMO_SOURCES}
                    INCLUDE_DIRS inc
                    REQUIRES ssd1306_x ADC_Key)
