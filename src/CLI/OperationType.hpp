#ifndef PROJECT_OPERATIONTYPE_HPP
#define PROJECT_OPERATIONTYPE_HPP
#include <ranges>
#include <vector>

enum class OPType {
    INSERT,
    GET,
    DELETE,
    TERMINATE,
    ADD,
    SUBTRACT,
    DIVIDE,
    MULTIPLY,
    CONCAT,
    SUFFIX,
    PREFIX,
    AT,
    FIND,
    APPEND,
    POP,
    VINSERT,
    REMOVE,
    SET,
    VGET,
    SIZE,
    INVALID
};

using view_t = std::ranges::subrange<std::vector<std::string>::iterator>;

OPType GetOPType(view_t &args);



#endif //PROJECT_OPERATIONTYPE_HPP