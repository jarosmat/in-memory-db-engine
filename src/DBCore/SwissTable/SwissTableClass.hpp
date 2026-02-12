#ifndef PROJECT_SWISSTABLECLASS_HPP
#define PROJECT_SWISSTABLECLASS_HPP
#include "Vector.hpp"
#include "Group.hpp"

using hash = size_t;

template <typename F, typename D, typename K>
class SwissTable {
    static constexpr size_t groupSize = 8;
    static constexpr size_t initialGroupCount = 1;
    static constexpr size_t resizeCoef = 2;
    static constexpr size_t h1ShiftCoef = 7; // number of shifts to get upper 57 bits
    static constexpr size_t h2Mask = 0x7F; // Mask for getting 7 lowest bits
    Vector<Group> metadata;
    Vector<D> data;

    void Rehash();

    void Delete(size_t pos);

    void SetValue(size_t pos, const D& data);

    void SetValue(size_t pos, D&& data);

    D& GetValue(size_t pos);

    size_t GetNextGroup(size_t);

    std::pair<bool, size_t> CheckExistence(const lookUpRes& arr, K& key, size_t group);

public:
    class Entry;

    SwissTable();

    Entry LookUp(K key);

};

#endif //PROJECT_SWISSTABLECLASS_HPP
