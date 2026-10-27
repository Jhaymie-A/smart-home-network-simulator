#include "light.h"

/**
 * Starts the LightService worker thread to handle incoming commands.
 */
void LightService::start() {
    std::thread([this]() {
        while (running) {
            std::unique_lock<std::mutex> lock(mtx);

            // Wait for a new command or service shutdown
            cv.wait(lock, [this]() { return !commandQueue.empty() || !running; });

            // Process all queued commands
            while (!commandQueue.empty()) {
                auto pair = commandQueue.front();
                std::string cmd = pair.first;
                std::function<void(std::string)> callback = pair.second;
                commandQueue.pop(); // Remove the command
                lock.unlock(); // Unlock during execution

                std::string result;

                if (cmd == "on") {
                    result = isOn ? "[Light] Already ON\n" : (isOn = true, "[Light] Turned ON\n");
                }
                else if (cmd == "off") {
                    result = !isOn ? "[Light] Already OFF\n" : (isOn = false, "[Light] Turned OFF\n");
                }
                else if (cmd == "status") {
                    result = isOn ? "[Light] currently ON\n" : "[Light] currently OFF\n";
                }
                else {
                    result = "[Light] Unknown command\n";
                }

                // Send response back to the client
                if (callback) callback(result);

                lock.lock(); // Re-lock for next iteration
            }
        }
        }).detach(); // Detach thread so it runs independently
}
