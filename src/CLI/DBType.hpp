#ifndef PROJECT_DBTYPE_HPP
#define PROJECT_DBTYPE_HPP
#include <string>

enum DBTypes {
    INTEGER,
    FLOAT,
    STRING,
    OBJECT,
    VECTOR_INT,
    VECTOR_FLOAT,
    VECTOR_STRING,
    INVALID
};

DBTypes GetType(std::string& type);




#endif //PROJECT_DBTYPE_HPP