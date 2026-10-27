#include "router.h"
#include <sstream>
#include <vector>
#include <stdexcept>

namespace SmartHome {

    // Adds a connection entry to the router for a specific device
    void Router::addConnection(int deviceId, const std::string& ipAddress, int cidr, DeviceService* service) {
        // Convert the IP address string to its integer representation
        uint32_t ip = ipToInt(ipAddress);

        // Get the subnet mask based on the CIDR value
        uint32_t subnetMask = getSubnetMask(cidr);

        // Calculate the network address by applying the subnet mask to the IP address
        uint32_t network = getNetworkAddress(ip, subnetMask);

        // Store the connection information in the connections map
        connections[deviceId] = ConnectionInfo{ ipAddress, cidr, service };

        // Print out the details of the newly added connection
        std::cout << "[Router] Device " << deviceId
            << " connected with IP " << ipAddress
            << " (Network: " << intToIp(network) << "/" << cidr << ")\n";
    }

    // Routes a message to the device specified by its ID
    void Router::routeMessage(int deviceId, const std::string& message) {
        // Find the device by its ID in the connections map
        auto it = connections.find(deviceId);

        // If the device isn't found, print an error and return
        if (it == connections.end()) {
            std::cerr << "[Router] Device ID " << deviceId << " not found.\n";
            return;
        }

        // Get the device associated with the connection
        auto* device = it->second.device;

        // If the device exists, send the message to it; otherwise, print an error
        if (device) {
            device->sendCommand(message, [](const std::string& result) {
                std::cout << "[Router Response] " << result;
                });
        }
        else {
            std::cerr << "[Router] No device attached for ID " << deviceId << "\n";
        }
    }

    // Prints out the current routing table, displaying all device connections
    void Router::printRoutingTable() const {
        std::cout << "\n[Routing Table]\n";
        // Iterate over all connections stored in the router
        for (const auto& entry : connections) {
            int id = entry.first;                    // Device ID
            const ConnectionInfo& info = entry.second; // Connection information

            // Print out the device ID, IP address, and subnet mask (in CIDR format)
            std::cout << "Device ID: " << id
                << " | IP: " << info.ipAddress
                << " | Subnet: /" << info.subnetMask << "\n";
        }
    }

    // Prints the currently active connections in the router
    void Router::printActiveConnections() const {
        std::cout << "\n[Active Connections]\n";
        // Iterate over all connections stored in the router
        for (const auto& entry : connections) {
            int id = entry.first;                    // Device ID
            const ConnectionInfo& info = entry.second; // Connection information

            // Print out the device ID and its associated IP address
            std::cout << "Device ID: " << id << " - IP: " << info.ipAddress << "\n";
        }
    }

    // Converts an IP address from string format (e.g., "192.168.1.1") to a 32-bit integer
    uint32_t Router::ipToInt(const std::string& ip) const {
        std::stringstream ss(ip);
        std::string item;
        uint32_t result = 0;

        // Split the IP address into its four parts (octets) and convert each to its integer representation
        for (int i = 0; i < 4; ++i) {
            std::getline(ss, item, '.');
            result |= (std::stoi(item) << (24 - 8 * i)); // Shift the part into the correct position
        }
        return result;
    }

    // Converts a 32-bit integer into an IP address string
    std::string Router::intToIp(uint32_t ip) const {
        std::ostringstream oss;

        // Break the integer into its four parts (octets) and format them into the IP address string
        for (int i = 3; i >= 0; --i) {
            oss << ((ip >> (8 * i)) & 0xFF); // Shift the correct byte into place
            if (i > 0) oss << '.'; // Add dots between octets
        }
        return oss.str();
    }

    // Given a CIDR notation (e.g., 24), return the corresponding subnet mask as a 32-bit integer
    uint32_t Router::getSubnetMask(int cidr) const {
        // Generate the subnet mask by left-shifting 1s and filling the rest with 0s
        return cidr == 0 ? 0 : (~0U << (32 - cidr));
    }

    // Given an IP address and subnet mask, calculate the network address
    uint32_t Router::getNetworkAddress(uint32_t ip, uint32_t subnetMask) const {
        return ip & subnetMask; // Perform a bitwise AND operation to extract the network part
    }

} // namespace SmartHome
