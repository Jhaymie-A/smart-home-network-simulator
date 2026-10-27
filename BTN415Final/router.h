#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <iostream>
#include <memory>
#include "device_base.h" // So we can use DeviceService*

namespace SmartHome {

    // Router class for managing device connections in the smart home network
    class Router {
    public:
        // Adds a device connection to the router's routing table
        void addConnection(int deviceId, const std::string& ipAddress, int cidr, DeviceService* service);

        // Routes a message to the device specified by deviceId
        void routeMessage(int deviceId, const std::string& message);

        // Prints the routing table with all connected devices and their info
        void printRoutingTable() const;

        // Prints a list of currently active connections
        void printActiveConnections() const;

    private:
        // Structure to hold the connection info for a device
        struct ConnectionInfo {
            std::string ipAddress; // The IP address of the device
            int subnetMask;        // The subnet mask in CIDR format
            DeviceService* device; // Pointer to the actual device service
        };

        // Unordered map storing connections by device ID
        std::unordered_map<int, ConnectionInfo> connections;

        // Converts an IP address string (e.g., "192.168.1.1") to its 32-bit integer representation
        uint32_t ipToInt(const std::string& ip) const;

        // Converts a 32-bit integer back to an IP address string (e.g., 3232235776 -> "192.168.1.1")
        std::string intToIp(uint32_t ip) const;

        // Calculates the subnet mask for a given CIDR value
        uint32_t getSubnetMask(int cidr) const;

        // Computes the network address given an IP address and subnet mask
        uint32_t getNetworkAddress(uint32_t ip, uint32_t subnetMask) const;
    };

} // namespace SmartHome
