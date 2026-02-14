#include "Group.hpp"

using namespace std;

bool Group::isValidHash2(const hash2Type hash) {
    return hash <= USED_MASK;
}

Group::Group(): start(0), end(0) {
}

Group::Group(const size_t start, const size_t end, controlWord &&metadata) : start(start), end(end),
                                                                             metadata(std::move(metadata)) {
}

Group::Group(const size_t start, const size_t end) : start(start), end(end), metadata(controlWord(end - start, EMPTY)) {
}

lookUpRes Group::LookUp(const hash2Type hash2) const {
    if (!isValidHash2(hash2)) {
        throw invalidHashError;
    }
    // this tries to find entries with same hash, if it does not find them, then ot tries to return empty spaces in group,
    // if no empty spaces are found it just returns FULL
    posArr result;
    posArr resultEmpty;
    LookupRes lookUpResult = FULL;
    const size_t size = end - start;

    for (size_t i = 0; i < size; i++) {
        const auto currMetadata = metadata[i];
        if (hash2 == currMetadata) {
            result.push_back(i + start);
            lookUpResult = USED;
        } else if (currMetadata == EMPTY) {
            resultEmpty.push_back(i + start);
        }
    }

    if (lookUpResult == USED) {
        return {lookUpResult, result};
    }

    if (!resultEmpty.empty()) {
        lookUpResult = LookupRes::EMPTY;
    }
    return {lookUpResult, resultEmpty};
}

void Group::SetVal(const hash2Type hash, const pos pos) {
    if (pos < start || pos >= end) {
        throw outOfBoundsError;
    }

    if (!isValidHash2(hash)) {
        throw invalidHashError;
    }

    metadata[pos - start] = hash;
}

void Group::Delete(const pos pos) {
    if (pos < start || pos >= end) {
        auto exception = outOfBoundsError;
        throw exception;
    }

    metadata[pos - start] = DELETED;
}

bool Group::Used(const pos pos) const {
    if (pos < start || pos >= end) {
        auto exception = outOfBoundsError;
        throw exception;
    }

    return metadata[pos - start] != EMPTY && metadata[pos] != DELETED;
}
