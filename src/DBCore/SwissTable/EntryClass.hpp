#ifndef PROJECT_ENTRYCLASS_HPP
#define PROJECT_ENTRYCLASS_HPP
#include "SwissTable.hpp"

template<typename F, typename D, typename K>
class SwissTable<F,D,K>::Entry {
    const SwissTable<F,D,K>& table;
    const LookupRes result;
    const size_t group;
    const size_t pos;
public:
    Entry(const SwissTable<F, D, K> &table, LookupRes res, size_t group, size_t pos);

    [[nodiscard]] LookupRes GetResult() const;

    void DeleteEntry();

    void SetValue(const D& val);

    void SetValue(D&& val);

    D& GetValue() const;

};


#endif //PROJECT_ENTRYCLASS_HPP