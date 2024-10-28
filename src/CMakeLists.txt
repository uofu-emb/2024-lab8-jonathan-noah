foreach(PNAME transmit_can receive_can)

    add_executable(${PNAME} ${PNAME}.c can.c)

    pico_set_program_name(${PNAME} "${PNAME}")
    pico_set_program_version(${PNAME} "0.1")

    # Modify the below lines to enable/disable output over UART/USB.
    # These are mutually exclusive, only set one.
    pico_enable_stdio_uart(${PNAME} 0)
    pico_enable_stdio_usb(${PNAME} 1)

    # Add the libraries to the build
    target_link_libraries(${PNAME} PRIVATE
        pico_stdlib
        pico_multicore
        pico_async_context_freertos
        FreeRTOS-Kernel-Heap4
        can2040
    )

    # The CYW43 is the Wifi/Bluetooth module. If the board is set to pico_w, this
    # variable will be set and the wireless libraries added.
    if(PICO_CYW43_SUPPORTED)
        # For led support on pico_w
        target_link_libraries(${PNAME} PRIVATE
            pico_cyw43_arch_none
        )
    endif()

    # Another pico macro which generates the uf2 file in addition to the elf
    pico_add_extra_outputs(${PNAME})

    add_custom_target(flash_${PNAME} picotool load -f $<TARGET_FILE:${PNAME}> DEPENDS ${PNAME})

endforeach()

target_compile_definitions(receive_can PUBLIC CAN_RECEIVER)
