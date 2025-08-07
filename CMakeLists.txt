cmake_minimum_required(VERSION 3.30)

#important!!!
#cmake -DPICO_BOARD=pico_w ..


include(
        pico_sdk_import.cmake
)

project(picow_ap_class C CXX ASM)

set(CMAKE_CXX_STANDARD 20)

pico_sdk_init()


add_executable(picow_ap_class main.c
        localtcp.c
        localtcp.h
        dhcpserver/dhcpserver.c
        dnsserver/dnsserver.c
)

target_include_directories(picow_ap_class PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/dhcpserver
        ${CMAKE_CURRENT_LIST_DIR}/dnsserver
)

target_link_libraries(picow_ap_class
        pico_stdlib
        pico_cyw43_arch_lwip_threadsafe_background
)

# You can change the address below to change the address of the access point
pico_configure_ip4_address(picow_ap_class PRIVATE
        CYW43_DEFAULT_IP_AP_ADDRESS 192.168.4.1
)

# Enable STDIO (optional, for USB serial prints)
pico_enable_stdio_usb(picow_ap_class 1)
pico_enable_stdio_uart(picow_ap_class 0)

pico_add_extra_outputs(picow_ap_class)


