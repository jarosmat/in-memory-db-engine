#include "OperationType.hpp"

OPType GetOPType(view_t &args) {
    if (args.empty()) {
        return OPType::INVALID;
    }

    auto val = args[0];

    if (val == "INSERT") {
        return OPType::INSERT;
    }
    if (val == "GET") {
        return OPType::GET;
    }
    if (val == "DELETE") {
        return OPType::DELETE;
    }
    if (val == "TERMINATE") {
        return OPType::TERMINATE;
    }
    if (val == "ADD") {
        return OPType::ADD;
    }
    if (val == "SUBTRACT") {
        return OPType::SUBTRACT;
    }
    if (val == "DIVIDE") {
        return OPType::DIVIDE;
    }
    if (val == "MULTIPLY") {
        return OPType::MULTIPLY;
    }
    if (val == "CONCAT") {
        return OPType::CONCAT;
    }
    if (val == "SUFFIX") {
        return OPType::SUFFIX;
    }
    if (val == "PREFIX") {
        return OPType::PREFIX;
    }
    if (val == "AT") {
        return OPType::AT;
    }
    if (val == "FIND") {
        return OPType::FIND;
    }
    if (val == "APPEND") {
        return OPType::APPEND;
    }
    if (val == "POP") {
        return OPType::POP;
    }
    if (val == "VINSERT") {
        return OPType::VINSERT;
    }
    if (val == "REMOVE") {
        return OPType::REMOVE;
    }
    if (val == "SET") {
        return OPType::SET;
    }
    if (val == "VGET") {
        return OPType::VGET;
    }
    if (val == "SIZE") {
        return OPType::SIZE;
    }

    return OPType::INVALID;
}