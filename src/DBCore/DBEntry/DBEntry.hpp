#ifndef PROJECT_DBENTRY_HPP
#define PROJECT_DBENTRY_HPP
#include "ValueTypes.hpp"
#include "SwissTable.hpp"

enum DBTypes {
    INTEGER,
    FLOAT,
    STRING,
    OBJECT,
    VECTOR_INT,
    VECTOR_FLOAT,
    VECTOR_STRING
};

template<typename E>
class DBEntry {
    E payload;

public:
    explicit DBEntry(std::remove_reference_t<E> &&payload) : payload(std::move(payload)) {
    }

    std::remove_reference_t<E> &GetRef() {
        return payload;
    }

    void SetVal(const std::remove_reference_t<E> &val) {
        payload = val;
    }

    void SetVal(std::remove_reference_t<E> &&val) {
        payload = std::move(val);
    }
};

template<typename K, typename E, typename F>



#endif //PROJECT_DBENTRY_HPP