file(GLOB_RECURSE DEMO_SOURCES example/*.c)
file(GLOB_RECURSE INTERFACE_SOURCES interface/*.c)
file(GLOB_RECURSE SOURCE_SOURCES src/*.c)

idf_component_register(SRCS ${DEMO_SOURCES} ${INTERFACE_SOURCES} ${SOURCE_SOURCES}
                    INCLUDE_DIRS example interface src
                    REQUIRES ssd1306_x)
