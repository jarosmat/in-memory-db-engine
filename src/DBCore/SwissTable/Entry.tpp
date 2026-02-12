#ifndef ENTRY_TPP
#define ENTRY_TPP

#include "EntryClass.hpp"

template<typename F, typename D, typename K>
SwissTable<F, D, K>::Entry::Entry(const SwissTable<F, D, K> &table, const LookupRes res, const size_t group,
                                  const size_t pos) : table(table), result(res), group(group), pos(pos) {
}

template<typename F, typename D, typename K>
LookupRes SwissTable<F, D, K>::Entry::GetResult() const {
    return result;
}

template<typename F, typename D, typename K>
void SwissTable<F, D, K>::Entry::DeleteEntry() {
    table.Delete(pos);
}

template<typename F, typename D, typename K>
void SwissTable<F, D, K>::Entry::SetValue(const D &val) {
    table.SetValue(pos, val);
}

template<typename F, typename D, typename K>
void SwissTable<F, D, K>::Entry::SetValue(D &&val) {
    table.SetValue(pos, std::move(val));
}

template<typename F, typename D, typename K>
D &SwissTable<F, D, K>::Entry::GetValue() const {
    return table.GetValue(pos);
}

#endif // ENTRY_TPP
