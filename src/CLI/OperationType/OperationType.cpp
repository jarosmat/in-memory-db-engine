#include "OperationType.hpp"


std::size_t GetArgCount(const OPType op) {
    switch (op) {
        case OPType::INSERT:
            return 2;
        case OPType::GET:
            return 1;
        case OPType::DELETE:
            return 1;
        case OPType::TERMINATE:
            return 0;
        case OPType::ADD:
            return 2;
        case OPType::SUBTRACT:
            return 2;
        case OPType::DIVIDE:
            return 2;
        case OPType::MULTIPLY:
            return 2;
        case OPType::FADD:
            return 2;
        case OPType::FSUBTRACT:
            return 2;
        case OPType::FDIVIDE:
            return 2;
        case OPType::FMULTIPLY:
            return 2;
        case OPType::CONCAT:
            return 2;
        case OPType::SUFFIX:
            return 2;
        case OPType::PREFIX:
            return 2;
        case OPType::AT:
            return 2;
        case OPType::FIND:
            return 2;
        case OPType::APPEND:
            return 2;
        case OPType::POP:
            return 1;
        case OPType::VINSERT:
            return 3;
        case OPType::REMOVE:
            return 2;
        case OPType::SET:
            return 3;
        case OPType::VGET:
            2;
        case OPType::SIZE:
            return 1;
        default:
            return 0;
    }
}

OPType GetOPType(view_t &args) {
    if (args.empty()) {
        return OPType::INVALID;
    }

    auto val = args[0];
    args.advance(1);

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
    if (val == "FADD") {
        return OPType::FADD;
    }
    if (val == "FSUBTRACT") {
        return OPType::FSUBTRACT;
    }
    if (val == "FDIVIDE") {
        return OPType::FDIVIDE;
    }
    if (val == "FMULTIPLY") {
        return OPType::FMULTIPLY;
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
    if (val == "OGET") {
        return OPType::OGET;
    }
    if (val == "OSET") {
        return OPType::OSET;
    }

    return OPType::INVALID;
}

bool ValidateIntOp(const OPType op) {
    return (op == OPType::ADD || op == OPType::SUBTRACT || op == OPType::DIVIDE || op == OPType::MULTIPLY || op ==
            OPType::SIZE);
}

bool ValidateFloatOp(const OPType op) {
    return (op == OPType::FADD || op == OPType::FSUBTRACT || op == OPType::FDIVIDE || op == OPType::FMULTIPLY);
}

bool ValidateStringOp(const OPType op) {
    return (op == OPType::CONCAT || op == OPType::SUFFIX || op == OPType::PREFIX || op == OPType::AT);
}

bool ValidateVectorOP(const OPType op) {
    return (op == OPType::APPEND || op == OPType::POP || op == OPType::VINSERT || op == OPType::REMOVE || op ==
            OPType::SET || op == OPType::VGET);
}

bool ValidateVectorIntOP(const OPType op) {
    return ValidateVectorOP(op) || op == OPType::FIND;
}

bool ValidateOPType(const OPType op, const DBTypes dbType) {
    if (dbType == DBTypes::INVALID) {
        return false;
    }
    if (dbType == DBTypes::INTEGER) {
        return ValidateIntOp(op);
    }
    if (dbType == DBTypes::FLOAT) {
        return ValidateFloatOp(op);
    }
    if (dbType == DBTypes::STRING) {
        return ValidateStringOp(op);
    }
    if (dbType == DBTypes::VECTOR_INT) {
        return ValidateVectorIntOP(op);
    }
    return ValidateVectorOP(op);
}

