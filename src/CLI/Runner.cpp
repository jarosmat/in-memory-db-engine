#include "Runner.hpp"

void readThread(const std::stop_token &stoken, std::istream &stream, std::mutex &queueMutex,
    std::queue<std::string> &dataQueue) {
    std::string line;
    bool not_terminated = true;
    while (!stoken.stop_requested() && not_terminated) {
        if (stream.eof()) {
            not_terminated = false;
            std::lock_guard lock(queueMutex);
            dataQueue.push(API_CONSTANTS::TERMINATE);
        }
        else if (std::getline(stream, line)) {
            if (line == API_CONSTANTS::TERMINATE) {
                not_terminated = false;
            }
            std::lock_guard lock(queueMutex);
            dataQueue.push(line);
        }
    }
}
