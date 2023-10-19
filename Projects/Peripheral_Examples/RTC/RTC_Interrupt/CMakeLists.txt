# Build the executable based on the source files
set(TARGET_NAME RTC_Interrupt)
add_executable(${TARGET_NAME}
  ${CMAKE_SOURCE_DIR}/Projects/Peripheral_Examples/RTC/RTC_Interrupt/main.c 
  ${CMAKE_SOURCE_DIR}/Projects/Peripheral_Examples/RTC/RTC_Interrupt/retarget.c
  ${CMAKE_SOURCE_DIR}/Projects/Peripheral_Examples/RTC/RTC_Interrupt/W7500x_it.c
  ${CMAKE_SOURCE_DIR}/Libraries/Startup/startup_W7500.s
  )
target_include_directories(${TARGET_NAME} PUBLIC
  ${CMAKE_SOURCE_DIR}/Projects/Peripheral_Examples/RTC/RTC_Interrupt
  )

target_link_libraries(${TARGET_NAME} PRIVATE
    RTC_FILES
    UART_FILES
    GPIO_FILES
    SYSTEM_W7500X_FILES
    )

complie_link_option(${TARGET_NAME})
create_binary_file(${TARGET_NAME})

