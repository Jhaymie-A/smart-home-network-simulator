#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream>
#include <thread>
#include <sstream>      // For parsing incoming commands
#include <winsock2.h>   // WinSock2 for socket communication
#include <ws2tcpip.h>   // Extended TCP/IP functions
#include <functional>    // For std::function

#pragma comment(lib, "ws2_32.lib") // Link with WinSock library

#include "light.h"
#include "thermostat.h"
#include "security.h"
#include "Router.h"

// Instantiate smart home device services
LightService light;
ThermostatService thermostat;
SecurityService security;

// Instantiate the Router object
SmartHome::Router router;

/**
 * Handles an individual client connection in a separate thread.
 */
void handle_client(SOCKET client_socket) {
    char buffer[1024]; // Buffer for incoming request

    sockaddr_in client_info;
    int client_info_len = sizeof(client_info);
    getpeername(client_socket, (sockaddr*)&client_info, &client_info_len);
    std::string client_ip = inet_ntoa(client_info.sin_addr);
    unsigned short client_port = ntohs(client_info.sin_port);

    // Log user connection
    std::cout << "[Server] User connected: IP " << client_ip << " | Port " << client_port << "\n";

    while (true) {
        // Receive data from the client
        int bytesRead = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
        if (bytesRead <= 0) break; // Exit if connection lost or error occurred

        buffer[bytesRead] = '\0'; // Null-terminate to create C-string
        std::string request(buffer);

        std::cout << "[Server] Received: " << request << "\n";

        // Split the request into METHOD and PATH using stringstream
        std::istringstream iss(request);
        std::string method, path;
        iss >> method >> path;

        // Lambda to send responses back to the client
        auto respond = [&](const std::string& response) {
            send(client_socket, response.c_str(), response.length(), 0);
            };

        // Route SET commands (state changes)
        if (method == "SET") {
            if (path == "/light/on") {
                light.sendCommand("on", respond);
            }
            else if (path == "/light/off") {
                light.sendCommand("off", respond);
            }
            else if (path == "/security/on") {
                security.sendCommand("on", respond);
            }
            else if (path == "/security/off") {
                security.sendCommand("off", respond);
            }
            else if (path.rfind("/thermostat/set/", 0) == 0) {
                // Extract temperature from the path
                std::string value = path.substr(std::string("/thermostat/set/").length());
                thermostat.sendCommand("set " + value, respond);
            }
            else {
                respond("[Server] Unknown SET path\n");
            }
        }
        // Route GET commands (info/status retrieval)
        else if (method == "GET") {
            if (path == "/thermostat/status") {
                thermostat.sendCommand("status", respond);
            }
            else if (path == "/security/status") {
                security.sendCommand("status", respond);
            }
            else if (path == "/light/status") {
                light.sendCommand("status", respond);
            }
            else {
                respond("[Server] Unknown GET path\n");
            }
        }
        // Invalid HTTP-like method
        else {
            respond("[Server] Invalid method. Use GET or SET.\n");
        }
    }

    // Clean up client connection and log disconnection
    std::cout << "[Server] User disconnected: IP " << client_ip << " | Port " << client_port << "\n";
    closesocket(client_socket);
}


/**
 * Entry point for the server program.
 * Initializes networking, starts services, and accepts clients.
 */
int main() {
    // Step 1: Initialize Winsock
    WSAData wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed.\n";
        return 1;
    }

    // Step 2: Start smart device threads
    light.start();
    thermostat.start();
    security.start();

    // Step 3: Create TCP socket for server
    SOCKET server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed.\n";
        WSACleanup();
        return 1;
    }

    // Step 4: Set up server address info
    sockaddr_in server_addr = {};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);          // Listen on port 8080
    server_addr.sin_addr.s_addr = INADDR_ANY;    // Accept connections from any IP

    // Step 5: Bind the socket to the address
    if (bind(server_socket, (sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        std::cerr << "Bind failed.\n";
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    // Step 6: Begin listening for incoming connections
    listen(server_socket, 5); // Allow up to 5 queued clients
    std::cout << "[Server] Listening on port 8080...\n";

    // Step 7: Add device connections to the router
    router.addConnection(1, "192.168.1.2", 24, &light);       // Connect light with IP
    router.addConnection(2, "192.168.1.3", 24, &thermostat); // Connect thermostat with IP
    router.addConnection(3, "192.168.1.4", 24, &security);   // Connect security camera with IP

    // Step 8: Print routing table for debugging
    router.printRoutingTable();

    // Step 9: Accept clients in an infinite loop
    while (true) {
        SOCKET client_socket = accept(server_socket, nullptr, nullptr);
        if (client_socket != INVALID_SOCKET) {
            std::thread(handle_client, client_socket).detach(); // Handle each client in a separate thread
        }
    }

    // Final Cleanup (not reached in this design)
    closesocket(server_socket);
    WSACleanup();
    return 0;
}
