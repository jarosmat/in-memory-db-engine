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
};

template<typename K, typename E, typename F>



#endif //PROJECT_DBENTRY_HPP