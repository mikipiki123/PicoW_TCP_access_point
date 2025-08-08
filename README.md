This infrastructure designed for various porposes using Pico W Infineon CYW43439 (2.4 GHz).
The user can easily connect to pico W via access point uses TCP/IP protocol, and sends any commands wireless.

## How it works?

The system uses a single control structure called TCP_LOCAL:  
TCP_LOCAL - handles TCP,DHCP,DNS structs and and provides a single generic callback function *fn.

### The module has 3 functions:
•	ap_tcp_init – Receives the AP name and password from the user and initializes the TCP/IP stack, including DHCP and DNS.  
•	ap_tcp_open – Opens the access point and waits for client connections and responses.  
•	ap_tcp_shutdown – Shuts down all services used by the access point.


## How to use?

After initializing the TCP/IP settings with ap_tcp_init, the function returns a TCP_LOCAL handler.  
Before starting the access point, you must declare the *fn callback function.  
This function is triggered when the server receives data from the client (via the tcp_server_recv callback).  
You can build a full CLI application using *fn, without needing to dive deeply into the TCP/IP implementation.

### Some recommendations:
1) Take a look in CMake file, i mentioned there cmake command that's worked for me to compile.
2) In CMake i also allow usb port serial communication, so you can see responses in serial monitor.
3) When client connects the program starts counting time, if user isn't responded for more then minute (definition), connection closed.
4) I download shell app on IOS, you can easily connect with the command: nc 192.168.4.1 9999
