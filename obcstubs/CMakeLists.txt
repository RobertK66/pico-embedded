add_executable( obcstubs
    main.c
    ../common/MainClass.cpp
    ../common/Module.cpp
    ../common/CliModule.cpp
    ../common/UartBase.cpp
    mod/LedControl.cpp
    usb/usb-descriptors.c
    mod/Usb.cpp 
    mod/UartProxy.cpp
)

target_include_directories(obcstubs PUBLIC
    ./usb       # this is needed to make our usb/tusb_config.h overwrite the (suported/needed) one in the right place!...
    ../../pico-sdk/pico_sdk-src/lib/tinyusb/src
)


target_compile_definitions(obcstubs PRIVATE
    #TUD_OPT_RP2040_USB_DEVICE_ENUMERATION_FIX=1         # This is needed on some of my windows 11 dev pcs!                             19.12.2023: seems not to be needed any more....
    #PICO_STDIO_USB_ENABLE_RESET_VIA_VENDOR_INTERFACE=0  # this removes the RESET-Device, which would need extra driver in windows ...  19.12.2023: reset driver looks ok in GeräteManager
                                                         #                                                              when used with OUR 3 CDC config, this device does not appear at all ...
    PICO_STDIO_USB_ENABLE_RESET_VIA_BAUD_RATE=0          # mit 0 verhindert man den BOOTSEL reset wenn das COM device auf 1200 Baud gesetzt wird!
)

# pull in common dependencies
#target_link_libraries( obcstubs pico_stdlib pico_multicore hardware_i2c tinyusb_device hardware_flash)  // hardware_flash seems not to be needed (any more) USB config finds the Flash UID without it!?
target_link_libraries( obcstubs pico_stdlib pico_multicore hardware_i2c tinyusb_device)

pico_enable_stdio_usb(obcstubs 1)                        # enable the stdio stream to USB device - serial COM driver (Microsoft standard driver)
pico_enable_stdio_uart(obcstubs 0)                       # disable the uart to be used for stdio. (We use both UART as Proxies connected to CDC devices.)

# create map/bin/hex file etc.
pico_add_extra_outputs(obcstubs)
