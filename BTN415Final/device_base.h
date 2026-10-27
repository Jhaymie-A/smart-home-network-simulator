#ifndef DEVICE_BASE_H
#define DEVICE_BASE_H

#include <string>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <iostream>
#include <functional>

// Base class for all smart home devices (e.g., light, thermostat, security)
class DeviceService {
protected:
    // Queue holds pairs of command strings and their callback handlers
    std::queue<std::pair<std::string, std::function<void(std::string)>>> commandQueue;

    // Mutex for safe concurrent access to the command queue
    std::mutex mtx;

    // Condition variable to notify worker thread when new command arrives
    std::condition_variable cv;

    // Flag used to stop the service gracefully
    bool running = true;

public:
    virtual ~DeviceService() {}

    // Each subclass must implement its own start logic (typically runs a worker thread)
    virtual void start() = 0;

    // Stop the service and notify waiting threads to exit
    virtual void stop() {
        running = false;
        cv.notify_all();
    }

    // Called by the server to send a command to the device
    void sendCommand(const std::string& cmd, std::function<void(std::string)> callback) {
        std::lock_guard<std::mutex> lock(mtx); // Ensure thread-safe access
        commandQueue.push({ cmd, callback });   // Add command + callback to the queue
        cv.notify_one();                        // Wake up the device's worker thread
    }
};

#endif // DEVICE_BASE_H
