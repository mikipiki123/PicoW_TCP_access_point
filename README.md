This infrastructure designed for various porposes using Pico W Infineon CYW43439 (2.4 GHz).
The user can easily connect to pico W via access point uses TCP/IP protocol, and sends any commands wireless.

How it works?

Using single control structure:
TCP_LOCAL - handles TCP,DHCP,DNS structs and single generic callback function *fn.

The module has 3 functions:

ap_tcp_init - getting from user AP name and password and initialize TCP/IP settings, including DHCP and DNS.
ap_tcp_open - open AP and waiting for client connection and responses.
ap_tcp_shutdown - closes all Services used by AP.


How to use?

After initializing TCP/IP settings, the ap_tcp_init returns TCP_LOCAL handler, before starting AP you must declare the *fn callback function.
This function awakes when the server recieve data from user (tcp_server_recv callback).
You can make any CLI application with *fn without deep diving into TCP/IP settings.

Some recommendations:
1) Take a look in CMake file, i mentioned there cmake command that's worked for me to compile.
2) In CMake i also allow usb port serial communication, so you can see responses in serial monitor.
3) When client connects the program starts counting time, if user isn't responded for more then minute (definition), connection closed.
4) I download shell app on IOS, you can easily connect with the command: nc 192.168.4.1 9999
