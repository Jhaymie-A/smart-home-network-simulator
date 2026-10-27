# Smart Home Network Simulator

A C++ client/server program that simulates a smart home over TCP sockets. The server handles several clients at once and routes GET/SET commands to light, thermostat and security services. It also includes IPv4, subnet-mask and network-address calculations.

## Tech

- C++ (Visual Studio)
- WinSock
- Threads, mutexes and condition variables

## Build

Open `BTN415Final.sln` in Visual Studio, build, run the server, then run the client.
