
# Build the executable based on the source files
set(TARGET_NAME src)

add_executable(${TARGET_NAME}

  ${CMAKE_SOURCE_DIR}/src/main.c 
  ${CMAKE_SOURCE_DIR}/src/W7500x_it.c
  ${CMAKE_SOURCE_DIR}/src/retarget.c
  ${CMAKE_SOURCE_DIR}/src/bsp_surf5_w7500.c
  ${CMAKE_SOURCE_DIR}/src/blinky.c
  ${CMAKE_SOURCE_DIR}/Libraries/sst_c/src/sst.c
  ${CMAKE_SOURCE_DIR}/Libraries/sst_c/ports/arm-cm/sst_port.c
  ${CMAKE_SOURCE_DIR}/Libraries/CMSIS/Device/WIZnet/W7500/Source/GCC/startup_w7500.s
  )

target_include_directories(${TARGET_NAME} PUBLIC
  ${CMAKE_SOURCE_DIR}/src
  ${CMAKE_SOURCE_DIR}/Libraries/sst_c/ports/arm-cm
  ${CMAKE_SOURCE_DIR}/Libraries/sst_c/include
  )

# target_link_libraries(${TARGET_NAME} PRIVATE
#     SYSTEM_W7500X_FILES  
#     GPIO_FILES
#     UART_FILES
#     ADC_FILES
#     CRG_FILES
#     DUALTIMER_FILES
#     EXTI_FILES
#     FLASH_FILES
#     MIIM_FILES
#     MISC_FILES
#     PWM_FILES
#     RNG_FILES
#     RTC_FILES
#     SSP_FILES
#     WDT_FILES
#     WZTOE_FILES   
#     WIZCHIP_CONF_FILES
#     LOOPBACK_FILES
# )

target_link_libraries(${TARGET_NAME} PRIVATE
    SYSTEM_W7500X_FILES  
    GPIO_FILES
    UART_FILES
    ADC_FILES
    CRG_FILES
    DUALTIMER_FILES
    EXTI_FILES
    FLASH_FILES
    MIIM_FILES
    MISC_FILES
    PWM_FILES
    RNG_FILES
    RTC_FILES
    SSP_FILES
    WDT_FILES
    DNS_FILES
    DHCP_FILES
    SOCKET_FILES
    LOOPBACK_FILES
    WIZCHIP_CONF_FILES
    WZTOE_FILES
)

complie_link_option(${TARGET_NAME})
create_binary_file(${TARGET_NAME})
create_elf_file(${TARGET_NAME})

