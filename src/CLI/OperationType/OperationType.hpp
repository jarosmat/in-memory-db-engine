#ifndef PROJECT_OPERATIONTYPE_HPP
#define PROJECT_OPERATIONTYPE_HPP
#include <functional>
#include <ranges>
#include <vector>
#include "DBType.hpp"
#include "Vector.hpp"
#include <fstream>
#include <queue>
#include "QueueEntry.hpp"


using ttlQueueType = std::priority_queue<std::unique_ptr<QueueEntry>, std::vector<std::unique_ptr<QueueEntry> >,
    CompareQueueEntry>;

template<typename T>
struct vector_t {
    using type = T;
};

template<typename T>
struct vector_t<Vector<T> > {
    using type = T;
};

enum class OPType {
    INSERT,
    GET,
    DELETE,
    TERMINATE,
    ADD,
    SUBTRACT,
    DIVIDE,
    MULTIPLY,
    FADD,
    FSUBTRACT,
    FDIVIDE,
    FMULTIPLY,
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
    OGET,
    OSET,
    INVALID
};



std::size_t GetArgCount(OPType op);

using view_t = std::ranges::subrange<std::vector<std::string>::iterator>;

OPType GetOPType(view_t &args);

bool ValidateIntOp(OPType op);

bool ValidateFloatOp(OPType op);

bool ValidateStringOp(OPType op);

bool ValidateVectorOP(OPType op);

bool ValidateVectorIntOP(OPType op);

bool ValidateOPType(OPType op, DBTypes dbType);


#endif //PROJECT_OPERATIONTYPE_HPP
