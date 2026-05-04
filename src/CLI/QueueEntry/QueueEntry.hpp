#ifndef PROJECT_QUEUEENTRY_HPP
#define PROJECT_QUEUEENTRY_HPP
#include "Timepoint.hpp"

struct QueueEntry {
    Timepoint timepoint{};
    std::chrono::milliseconds ttl;
    std::vector<uint8_t> key;
    bool invalid = false;


    QueueEntry(std::chrono::milliseconds ttl, const std::vector<uint8_t> &key);

    [[nodiscard]] bool Expired() const;

    [[nodiscard]] std::chrono::milliseconds RemainingTime() const;

    [[nodiscard]] bool Invalid() const;
};

struct CompareQueueEntry {
    bool operator()(const std::unique_ptr<QueueEntry> &a, const std::unique_ptr<QueueEntry> &b) const;
};

#endif //PROJECT_QUEUEENTRY_HPP
