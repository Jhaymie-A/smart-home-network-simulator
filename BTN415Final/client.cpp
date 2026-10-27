#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <iostream>
#include <string>
#include <thread>

#pragma comment(lib, "ws2_32.lib")

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    SOCKET clientSocket;
    struct sockaddr_in serverAddr;
    WSADATA wsaData;
    char buffer[BUFFER_SIZE];
    int bytesReceived;

    // 1. Initialize Winsock for TCP/IP networking
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cout << "Failed. Error Code: " << WSAGetLastError() << std::endl;
        return 1;
    }

    // 2. Create client-side socket
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        std::cout << "Socket creation failed. Error Code: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }

    // 3. Set server address (localhost on port 8080)
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serverAddr.sin_port = htons(PORT);

    // 4. Connect to server
    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cout << "Connection to server failed." << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    // Display usage instructions
    std::cout << "Connected to Smart Home Server.\nType commands like:\n"
        << "GET /light/status\nGET /thermostat/status\nGET /security/status\nSET /thermostat/set/21\n"
        << "Type 'exit' to quit.\n";

    std::string input;
    while (true) {
        std::cout << "\nEnter command: ";
        std::getline(std::cin, input);

        if (input == "exit") break;

        // Send typed command to the server
        if (send(clientSocket, input.c_str(), input.size(), 0) < 0) {
            std::cout << "Failed to send data to server." << std::endl;
            break;
        }

        // Receive server response
        bytesReceived = recv(clientSocket, buffer, BUFFER_SIZE - 1, 0);
        if (bytesReceived <= 0) {
            std::cout << "Server disconnected or error occurred.\n";
            break;
        }

        buffer[bytesReceived] = '\0'; // Null-terminate string
        std::cout << "[Server Response]: " << buffer;
    }

    // Clean up Winsock resources
    closesocket(clientSocket);
    WSACleanup();
    return 0;
}
