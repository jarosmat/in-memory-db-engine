#include "QueueEntry.hpp"

QueueEntry::QueueEntry(const std::chrono::milliseconds ttl, const std::vector<uint8_t> &key) : ttl(ttl), key(key) {
}

bool QueueEntry::Expired() const {
    return timepoint.Elapsed(ttl) || invalid;
}

std::chrono::milliseconds QueueEntry::RemainingTime() const {
    return timepoint.RemainingTime(ttl);
}

bool QueueEntry::Invalid() const {
    return invalid;
}

bool CompareQueueEntry::operator()(const std::unique_ptr<QueueEntry> &a, const std::unique_ptr<QueueEntry> &b) const {
    return a->RemainingTime() > b->RemainingTime();
}
