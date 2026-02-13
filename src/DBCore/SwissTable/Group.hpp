#ifndef PROJECT_GROUPCLASS_HPP
#define PROJECT_GROUPCLASS_HPP
#include "Vector.hpp"
#include <vector>

enum LookupRes {
    USED,
    EMPTY,
    NOT_FOUND,
    FULL
};

using pos = size_t;
using posArr = std::vector<pos>;
using lookUpRes = std::pair<LookupRes, posArr>;
using hash2Type = uint8_t;
using controlWord = std::vector<hash2Type>;

class Group {
    size_t start;
    size_t end; // contains index just after last value of this group
    controlWord metadata;
    constexpr static uint8_t EMPTY = 0x80;
    constexpr static uint8_t DELETED = 0xFE;
    constexpr static uint8_t USED_MASK = 0x7F;
    inline const static std::string invalidHashError = "Invalid h2";
    inline const static std::string outOfBoundsError = "Position out of bound of group size";

    static bool isValidHash2(hash2Type hash);

public:
    Group();

    Group(size_t start, size_t end, controlWord&& metadata);

    Group(size_t start, size_t end);

    lookUpRes LookUp(hash2Type hash2) const; // tries to find specified hash in control word and returns corresponding Index

    void SetVal(hash2Type hash, pos pos);

    void Delete(pos pos);

    bool Used(pos pos) const;
};
// EMPTY   1 0 0 0 0 0 0 0
// USED    0 x
// DELETED 1 1 1 1 1 1 1 0

#endif //PROJECT_GROUPCLASS_HPP