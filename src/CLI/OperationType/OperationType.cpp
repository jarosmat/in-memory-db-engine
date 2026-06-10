#include "OperationType.hpp"
#include "Constants.hpp"

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
            return 2;
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

