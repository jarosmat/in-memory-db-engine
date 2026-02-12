#ifndef ENTRY_TPP
#define ENTRY_TPP

#include "EntryClass.hpp"

template<typename F, typename D, typename K>
SwissTable<F, D, K>::Entry::Entry(const SwissTable<F, D, K> &table, const LookupRes res): table(table), result(res)  {
}

template<typename F, typename D, typename K>
LookupRes SwissTable<F, D, K>::Entry::GetResult() {
}

#endif // ENTRY_TPP
