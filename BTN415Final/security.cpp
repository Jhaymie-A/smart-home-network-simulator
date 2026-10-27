#include "security.h"

/**
 * Starts the SecurityService thread which listens for and processes
 * commands such as turning the system on/off or checking status.
 */
void SecurityService::start() {
    std::thread([this]() {
        while (running) {
            std::unique_lock<std::mutex> lock(mtx);

            // Wait until there is a command in the queue or a shutdown is requested
            cv.wait(lock, [this]() { return !commandQueue.empty() || !running; });

            // Process all commands currently in the queue
            while (!commandQueue.empty()) {
                auto pair = commandQueue.front(); // Get the next command and callback
                std::string cmd = pair.first;
                std::function<void(std::string)> callback = pair.second;
                commandQueue.pop(); // Remove from queue
                lock.unlock(); // Unlock while processing to avoid blocking other operations

                std::string result;

                // Handle supported commands
                if (cmd == "on") {
                    result = isOn ? "[Security] System Already ACTIVE\n"
                        : (isOn = true, "[Security] System Turned ON\n");
                }
                else if (cmd == "off") {
                    result = !isOn ? "[Security] System Already DEACTIVATED\n"
                        : (isOn = false, "[Security] System Turned OFF\n");
                }
                else if (cmd == "status") {
                    result = isOn ? "[Security] System is ACTIVE\n"
                        : "[Security] System currently OFF\n";
                }
                else {
                    result = "[Security] Unknown command\n"; // Corrected from "[Light]"
                }

                // Send the result back via the callback function
                if (callback) callback(result);

                // Re-acquire the lock before checking for the next command
                lock.lock();
            }
        }
        }).detach(); // Run the service thread independently
}
