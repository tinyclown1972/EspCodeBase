file(GLOB_RECURSE SOURCE_SOURCES AdcKey.c)

idf_component_register(SRCS ${SOURCE_SOURCES}
                    INCLUDE_DIRS .
                    REQUIRES ADC_Key GUI)
