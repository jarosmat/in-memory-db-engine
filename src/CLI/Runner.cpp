#include "Runner.hpp"

void readThread(const std::stop_token &stoken, std::istream &stream, std::mutex &queueMutex,
    std::queue<std::string> &dataQueue) {
    std::string line;
    while (!stoken.stop_requested()) {
        if (std::getline(stream, line)) {
            std::lock_guard<std::mutex> lock(queueMutex);
            dataQueue.push(line);
        }
    }
}
