file(GLOB_RECURSE DEMO_SOURCES DemoProc/src/*.c)
file(GLOB_RECURSE GUI_SOURCES GUI/src/*.c)
file(GLOB_RECURSE HMI_SOURCES HMI/src/*.c)


idf_component_register(SRCS ${DEMO_SOURCES} ${GUI_SOURCES} ${HMI_SOURCES} 
                    INCLUDE_DIRS . DemoProc/inc GUI/inc HMI/inc
                    REQUIRES GUI ssd1306)
