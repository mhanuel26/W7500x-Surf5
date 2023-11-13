add_library(LOOPBACK_FILES STATIC)

target_sources(LOOPBACK_FILES PUBLIC
        ${CMAKE_SOURCE_DIR}/Libraries/ioLibrary/Application/loopback/loopback.c
        )

target_include_directories(LOOPBACK_FILES PUBLIC
        ${CMAKE_SOURCE_DIR}/Libraries/ioLibrary/Application/loopback
        )

target_link_libraries(LOOPBACK_FILES PUBLIC
        SOCKET_FILES    
        WIZCHIP_CONF_FILES
        WZTOE_FILES
        )