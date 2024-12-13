add_executable(can_txrx can.c)

pico_set_program_name(can_txrx "can_txrx")
pico_set_program_version(can_txrx "0.1")

# Modify the below lines to enable/disable output over UART/USB.
# These are mutually exclusive, only set one.
pico_enable_stdio_uart(can_txrx 0)
pico_enable_stdio_usb(can_txrx 1)

# Add the libraries to the build
target_link_libraries(can_txrx PRIVATE
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
    target_link_libraries(can_txrx PRIVATE
        pico_cyw43_arch_none
    )
endif()

# Another pico macro which generates the uf2 file in addition to the elf
pico_add_extra_outputs(can_txrx)

add_custom_target(flash_can_txrx picotool load -f $<TARGET_FILE:can_txrx> DEPENDS can_txrx)
