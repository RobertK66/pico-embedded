# Pull in SDK (must be before project)
#include(../pico_sdk_import.cmake)

add_executable(cli
    cli_main.c
    cli.c
    #i2c_slave.c
    #MyI2cSlave.cpp
    srs_sim.c
)

target_compile_definitions(cli PRIVATE
    TUD_OPT_RP2040_USB_DEVICE_ENUMERATION_FIX=1         # This is needed on some of my windows 11 dev pcs!
    PICO_STDIO_USB_ENABLE_RESET_VIA_VENDOR_INTERFACE=0  # this removes the RESET-Device, which would need extra driver in wndows....
    PICO_STDIO_USB_ENABLE_RESET_VIA_BAUD_RATE=0         # mit 0 verhindert man den BOOTSEL reset wenn das COM device auf 1200 Baud gesetzt wird!
)


# add_compile_definitions(
#     TUD_OPT_RP2040_USB_DEVICE_ENUMERATION_FIX=1         # This is needed on some of my windows 11 dev pcs!
#     PICO_STDIO_USB_ENABLE_RESET_VIA_VENDOR_INTERFACE=0  # this removes the RESET-Device, which would need extra driver in wndows....
#     PICO_STDIO_USB_ENABLE_RESET_VIA_BAUD_RATE=0         # mit 0 verhindert man den BOOTSEL reset wenn das COM device auf 1200 Baud gesetzt wird!
# )   

# pull in common dependencies
target_link_libraries(cli pico_stdlib pico_multicore hardware_i2c)

pico_enable_stdio_usb(cli 0)                            # usb depends on host machine (USB timimgs?) if the above workaround succeeeds
pico_enable_stdio_uart(cli 1)       



# create map/bin/hex file etc.
pico_add_extra_outputs(cli)

# add url via pico_set_program_url
#example_auto_set_url(blink)
