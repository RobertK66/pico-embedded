
add_compile_definitions(
    TUD_OPT_RP2040_USB_DEVICE_ENUMERATION_FIX=1 # usb unter windows 11 does not work ?
)   


add_executable(cli
    cli.c
    )


# pull in common dependencies
target_link_libraries(cli pico_stdlib)


pico_enable_stdio_usb(cli 0)        # usb unter windows 11 does not work ?
pico_enable_stdio_uart(cli 1)

# create map/bin/hex file etc.
pico_add_extra_outputs(cli)

# add url via pico_set_program_url
#example_auto_set_url(blink)
