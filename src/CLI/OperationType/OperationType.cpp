#include "OperationType.hpp"
#include "Constants.hpp"

std::size_t GetArgCount(const OPType op) {
    switch (op) {
        case OPType::INSERT:
            return ARG_COUNT_CONSTS::INSERT;
        case OPType::GET:
            return ARG_COUNT_CONSTS::GET;
        case OPType::DELETE:
            return ARG_COUNT_CONSTS::DELETE;
        case OPType::TERMINATE:
            return ARG_COUNT_CONSTS::TERMINATE;
        case OPType::ADD:
            return ARG_COUNT_CONSTS::ADD;
        case OPType::SUBTRACT:
            return ARG_COUNT_CONSTS::SUBTRACT;
        case OPType::DIVIDE:
            return ARG_COUNT_CONSTS::DIVIDE;
        case OPType::MULTIPLY:
            return ARG_COUNT_CONSTS::MULTIPLY;
        case OPType::FADD:
            return ARG_COUNT_CONSTS::FADD;
        case OPType::FSUBTRACT:
            return ARG_COUNT_CONSTS::FSUBTRACT;
        case OPType::FDIVIDE:
            return ARG_COUNT_CONSTS::FDIVIDE;
        case OPType::FMULTIPLY:
            return ARG_COUNT_CONSTS::FMULTIPLY;
        case OPType::CONCAT:
            return ARG_COUNT_CONSTS::CONCAT;
        case OPType::SUFFIX:
            return ARG_COUNT_CONSTS::SUFFIX;
        case OPType::PREFIX:
            return ARG_COUNT_CONSTS::PREFIX;
        case OPType::AT:
            return ARG_COUNT_CONSTS::AT;
        case OPType::FIND:
            return ARG_COUNT_CONSTS::FIND;
        case OPType::APPEND:
            return ARG_COUNT_CONSTS::APPEND;
        case OPType::POP:
            return ARG_COUNT_CONSTS::POP;
        case OPType::VINSERT:
            return ARG_COUNT_CONSTS::VINSERT;
        case OPType::REMOVE:
            return ARG_COUNT_CONSTS::REMOVE;
        case OPType::SET:
            return ARG_COUNT_CONSTS::SET;
        case OPType::VGET:
            return ARG_COUNT_CONSTS::VGET;
        case OPType::SIZE:
            return ARG_COUNT_CONSTS::SIZE;
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

    if (val == API_CONSTANTS::INSERT) {
        return OPType::INSERT;
    }
    if (val == API_CONSTANTS::GET) {
        return OPType::GET;
    }
    if (val == API_CONSTANTS::DELETE) {
        return OPType::DELETE;
    }
    if (val == API_CONSTANTS::TERMINATE) {
        return OPType::TERMINATE;
    }
    if (val == API_CONSTANTS::ADD) {
        return OPType::ADD;
    }
    if (val == API_CONSTANTS::SUBTRACT) {
        return OPType::SUBTRACT;
    }
    if (val == API_CONSTANTS::DIVIDE) {
        return OPType::DIVIDE;
    }
    if (val == API_CONSTANTS::MULTIPLY) {
        return OPType::MULTIPLY;
    }
    if (val == API_CONSTANTS::FADD) {
        return OPType::FADD;
    }
    if (val == API_CONSTANTS::FSUBTRACT) {
        return OPType::FSUBTRACT;
    }
    if (val == API_CONSTANTS::FDIVIDE) {
        return OPType::FDIVIDE;
    }
    if (val == API_CONSTANTS::FMULTIPLY) {
        return OPType::FMULTIPLY;
    }
    if (val == API_CONSTANTS::CONCAT) {
        return OPType::CONCAT;
    }
    if (val == API_CONSTANTS::SUFFIX) {
        return OPType::SUFFIX;
    }
    if (val == API_CONSTANTS::PREFIX) {
        return OPType::PREFIX;
    }
    if (val == API_CONSTANTS::AT) {
        return OPType::AT;
    }
    if (val == API_CONSTANTS::FIND) {
        return OPType::FIND;
    }
    if (val == API_CONSTANTS::APPEND) {
        return OPType::APPEND;
    }
    if (val == API_CONSTANTS::POP) {
        return OPType::POP;
    }
    if (val == API_CONSTANTS::VINSERT) {
        return OPType::VINSERT;
    }
    if (val == API_CONSTANTS::REMOVE) {
        return OPType::REMOVE;
    }
    if (val == API_CONSTANTS::SET) {
        return OPType::SET;
    }
    if (val == API_CONSTANTS::VGET) {
        return OPType::VGET;
    }
    if (val == API_CONSTANTS::SIZE) {
        return OPType::SIZE;
    }
    if (val == API_CONSTANTS::OGET) {
        return OPType::OGET;
    }
    if (val == API_CONSTANTS::OSET) {
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

