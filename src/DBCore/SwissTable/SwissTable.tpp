#ifndef PROJECT_SWISSTABLE_TPP
#define PROJECT_SWISSTABLE_TPP
#include "EntryClass.hpp"
#include "SwissTableClass.hpp"

template<typename F, typename D, typename K>
size_t SwissTable<F, D, K>::GetNextGroup(const size_t group) const {
    const auto res = group + 1;
    if (res < metadata.Count() ) {
        return res;
    }
    return 0;
}

template<typename F, typename D, typename K>
std::pair<bool, size_t> SwissTable<F, D, K>::CheckExistence(const lookUpRes &arr, K &key, const size_t group) {
    for (auto &&a: arr.second) {
        if (data[group * groupSize + a] == key) {
            return {true, a};
        }
    }

    return {false, 0};
}

template<typename F, typename D, typename K>
SwissTable<F, D, K>::SwissTable() : metadata(initialGroupCount), data(initialGroupCount * groupSize) {
}

template<typename F, typename D, typename K>
SwissTable<F, D, K>::Entry SwissTable<F, D, K>::LookUp(K key) {
    const hash hKey = F(key);
    const hash h1 = hKey >> h1ShiftCoef;
    const hash h2 = hKey & h2Mask;

    const size_t groupCount = metadata.Count();
    const size_t lookUpGroup = h1 % groupCount;


    auto lookUpRes = metadata[lookUpGroup].LookUp(h2);
    while (lookUpRes.first != EMPTY) {
        if (lookUpRes.first == USED) {
            if (auto checkRes = CheckExistence(lookUpRes, lookUpGroup); checkRes.first) {
                return Entry(this, lookUpRes.first, lookUpGroup, checkRes.second);
            }
        }

        lookUpGroup = GetNextGroup(lookUpGroup);
        lookUpRes = metadata[lookUpGroup].LookUp(h2);
    }

    return Entry(this, lookUpRes.first, lookUpGroup, lookUpRes.second[0]);
}

#endif //PROJECT_SWISSTABLE_TPP
