#ifndef PROJECT_SWISSTABLE_TPP
#define PROJECT_SWISSTABLE_TPP
#include "EntryClass.hpp"
#include "SwissTableClass.hpp"

template<typename F, typename D, typename K>
void SwissTable<F, D, K>::Rehash() {
    auto oldKeys = std::move(keys);
    auto oldData = std::move(data);
    const auto oldMetadata = std::move(metadata);

    keys = Vector<K>(oldKeys.Count() * resizeCoef);
    data = Vector<D>(oldData.Count() * resizeCoef);
    metadata = Vector<Group>(oldMetadata.Count() * resizeCoef);
    dataCount = 0;
    InitializeFields(oldMetadata.Count() * resizeCoef);


    size_t dataIndex = 0;
    for (size_t group = 0; group < oldMetadata.Count(); group++) {
        for (size_t i = 0; i < groupSize; i++, dataIndex++) {
            if (oldMetadata[group].Used(dataIndex)) {
                auto res = LookUp(oldKeys[dataIndex]);
                res.SetValue(oldData[dataIndex]);
            }
        }
    }
}

template<typename F, typename D, typename K>
bool SwissTable<F, D, K>::NeedRehash() {
    if (data.Count() == 0) {
        return false;
    }
    return (dataCount * 100) / data.Count() >= maxLoad;
}

template<typename F, typename D, typename K>
void SwissTable<F, D, K>::EnsureNotMaxload() {
    if (NeedRehash()) {
        Rehash();
    }
}

template<typename F, typename D, typename K>
void SwissTable<F, D, K>::Delete(const size_t pos) {
    const auto group = pos / groupSize;

    metadata[group].Delete(pos);
}

template<typename F, typename D, typename K>
void SwissTable<F, D, K>::SetValue(size_t pos, const D &val, const size_t hash, K &&key) {
    EnsureNotMaxload();
    const auto group = pos / groupSize;

    if (!metadata[group].Used(pos)) {
        dataCount++;
    }

    keys[pos] = std::move(key);
    data[pos] = val;
    metadata[group].SetVal(hash, pos);
}

template<typename F, typename D, typename K>
void SwissTable<F, D, K>::SetValue(size_t pos, D &&val, const size_t hash, K &&key) {
    EnsureNotMaxload();
    const auto group = pos / groupSize;

    if (!metadata[group].Used(pos)) {
        dataCount++;
    }

    keys[pos] = std::move(key);
    data[pos] = std::move(val);
    metadata[group].SetVal(hash, pos);
}

template<typename F, typename D, typename K>
D &SwissTable<F, D, K>::GetValue(size_t pos) {
    return data[pos];
}

template<typename F, typename D, typename K>
size_t SwissTable<F, D, K>::GetNextGroup(const size_t group) const {
    const auto res = group + 1;
    if (res < metadata.Count()) {
        return res;
    }
    return 0;
}

template<typename F, typename D, typename K>
std::pair<bool, size_t> SwissTable<F, D, K>::CheckExistence(const lookUpRes &arr, K &key, const size_t group) {
    for (auto &&a: arr.second) {
        if (keys[a] == key) {
            return {true, a};
        }
    }

    return {false, 0};
}

template<typename F, typename D, typename K>
void SwissTable<F, D, K>::InitializeFields(const size_t groupCount) {
    for (size_t i = 0; i < groupCount; i++) {
        metadata.Append(Group(i * groupSize, (i + 1) * groupSize));
    }

    const size_t dataSize = groupCount * groupSize;
    for (size_t i = 0; i < dataSize; i++) {
        data.Append(D());
        keys.Append(K());
    }
}

template<typename F, typename D, typename K>
SwissTable<F, D, K>::SwissTable(F hash_func) : dataCount(0), metadata(initialGroupCount), data(initialGroupCount * groupSize),
                                    keys(initialGroupCount * groupSize), hashFunc(hash_func) {
   InitializeFields();
}

template<typename F, typename D, typename K>
SwissTable<F, D, K>::Entry SwissTable<F, D, K>::LookUp(K key) {
    const hash hKey = hashFunc(key);
    const hash h1 = hKey >> h1ShiftCoef;
    const hash h2 = hKey & h2Mask;

    const size_t groupCount = metadata.Count();
    size_t lookUpGroup = h1 % groupCount;


    auto lookUpRes = metadata[lookUpGroup].LookUp(h2);
    while (lookUpRes.first != EMPTY) {
        if (lookUpRes.first == USED) {
            if (auto checkRes = CheckExistence(lookUpRes, key, lookUpGroup); checkRes.first) {
                return Entry(*this, lookUpRes.first, checkRes.second, key, h2);
            }
        }

        lookUpGroup = GetNextGroup(lookUpGroup);
        lookUpRes = metadata[lookUpGroup].LookUp(h2);
    }

    return Entry(*this, lookUpRes.first, lookUpRes.second[0], key, h2);
}

#endif //PROJECT_SWISSTABLE_TPP
