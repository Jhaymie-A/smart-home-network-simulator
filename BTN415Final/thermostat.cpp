#include "thermostat.h"

/**
 * Starts the ThermostatService thread, which waits for commands like:
 * - "set <temp>": to change the temperature
 * - "status": to retrieve the current temperature
 */
void ThermostatService::start() {
    std::thread([this]() {
        while (running) {
            std::unique_lock<std::mutex> lock(mtx);

            // Wait until there is at least one command or termination is signaled
            cv.wait(lock, [this]() { return !commandQueue.empty() || !running; });

            // Process all pending commands
            while (!commandQueue.empty()) {
                auto pair = commandQueue.front(); // Get next command
                std::string cmd = pair.first;
                std::function<void(std::string)> callback = pair.second;
                commandQueue.pop(); // Remove from queue
                lock.unlock(); // Unlock while processing the command

                std::string result;

                // Handle "set <temperature>" command
                if (cmd.rfind("set ", 0) == 0) {
                    try {
                        int value = std::stoi(cmd.substr(4)); // Convert temperature string to int
                        temperature = value;
                        result = "[Thermostat] Temperature set to " + std::to_string(temperature) + " Celcius\n";
                    }
                    catch (...) {
                        result = "[Thermostat] Invalid temperature value\n";
                    }
                }
                // Handle "status" command
                else if (cmd == "status") {
                    result = "[Thermostat] Current temperature is " + std::to_string(temperature) + " Celcius\n";
                }
                // Unknown command
                else {
                    result = "[Thermostat] Unknown command\n";
                }

                // Return the result via callback
                if (callback) callback(result);

                lock.lock(); // Re-lock the mutex before next loop iteration
            }
        }
        }).detach(); // Detach thread to run independently
}
