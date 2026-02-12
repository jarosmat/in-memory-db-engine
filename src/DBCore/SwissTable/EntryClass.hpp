#ifndef PROJECT_ENTRYCLASS_HPP
#define PROJECT_ENTRYCLASS_HPP
#include "SwissTable.hpp"

template<typename F, typename D, typename K>
class SwissTable<F,D,K>::Entry {
    const SwissTable<F,D,K>& table;
    const LookupRes result;
public:
    Entry(const SwissTable<F,D,K>& table, LookupRes res);

    LookupRes GetResult() const;

    void DeleteEntry();

    void SetValue(const D& data);

    void SetValue(D&& data);

    D& GetValue();

};


#endif //PROJECT_ENTRYCLASS_HPP