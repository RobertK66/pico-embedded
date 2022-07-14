add_executable(cli
    cli_main.c
    cli.c
    )

 add_compile_definitions(
    TUD_OPT_RP2040_USB_DEVICE_ENUMERATION_FIX=1         # This is needed on some of my windows 11 dev pcs!
    PICO_STDIO_USB_ENABLE_RESET_VIA_VENDOR_INTERFACE=0  # this removes the RESET-Device, which would need extra driver in wndows....
    PICO_STDIO_USB_ENABLE_RESET_VIA_BAUD_RATE=0         # mit 0 verhindert man den BOOTSEL reset wenn das COM device auf 1200 Baud gesetzt wird!
)   

# pull in common dependencies
target_link_libraries(cli pico_stdlib pico_multicore)

pico_enable_stdio_usb(cli 1)        # usb depends on host machine (USB timimgs?) if the above workaround succeeeds
pico_enable_stdio_uart(cli 1)       

# create map/bin/hex file etc.
pico_add_extra_outputs(cli)

# add url via pico_set_program_url
#example_auto_set_url(blink)
