#include "DBType.hpp"


auto GetType(view_t &view) -> DBTypes {
    if (view.empty()) {
        return DBTypes::INVALID;
    }

    const auto val = view.front();
    view.advance(1);
    if (val == "int") {
        return DBTypes::INTEGER;
    }
    if (val == "float") {
        return DBTypes::FLOAT;
    }
    if (val == "string") {
        return DBTypes::STRING;
    }
    if (val == "object") {
        return DBTypes::OBJECT;
    }
    if (val == "vect-int") {
        return DBTypes::VECTOR_INT;
    }
    if (val == "vect-float") {
        return DBTypes::VECTOR_FLOAT;
    }
    if (val == "vect-string") {
        return DBTypes::VECTOR_STRING;
    }
    return DBTypes::INVALID;
}
