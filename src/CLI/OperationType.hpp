#ifndef PROJECT_OPERATIONTYPE_HPP
#define PROJECT_OPERATIONTYPE_HPP
#include <functional>
#include <ranges>
#include <vector>
#include "Nodes.hpp"
#include "DBType.hpp"
#include "Vector.hpp"
#include "../DBCore/DBEntry/ValueTypes.hpp"
#include <fstream>
#include <filesystem>
#include <queue>
#include <random>
#include <stop_token>
#include <thread>

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
    INVALID
};

enum class ArgsType {
    CONSTANT,
    KEY_FROM_FILE,
    KEY_FROM_CONSTANT,
    EMPTY
};

enum class BytesFromFileRes {
    VALID,
    COULD_NOT_OPEN,
    COULD_NOT_READ
};

using err_t = std::vector<std::string>;

err_t CouldNotOpenFileErr(const std::string &file);

err_t CouldNotReadFileErr(const std::string &file);

err_t InvalidArgs();

err_t InvalidArgsCount(std::size_t expected, std::size_t actual);

err_t InvalidOperation();

using read_from_file_r = std::pair<BytesFromFileRes, std::vector<uint8_t> >;

read_from_file_r readFileBytes(const std::string &filePath);

std::vector<uint8_t> stringBytes(std::string &&str);

using view_t = std::ranges::subrange<std::vector<std::string>::iterator>;

OPType GetOPType(view_t &args);

bool ValidateIntOp(OPType op);

bool ValidateFloatOp(OPType op);

bool ValidateStringOp(OPType op);

bool ValidateVectorOP(OPType op);

bool ValidateVectorIntOP(OPType op);

bool ValidateOPType(OPType op, DBTypes dbType);

using args_parsed = std::pair<ArgsType, std::string>;

args_parsed ParseArg(view_t &args);

template<typename T>
using NodePtr = std::unique_ptr<Node<T> >;

enum class GetArgsReturnTypes {
    VALID,
    INVALID
};

template<typename T>
using get_arg_res = std::pair<GetArgsReturnTypes, NodePtr<T> >;


template<typename T>
using parse_res = std::pair<bool, T>;

parse_res<String> ParseString(std::string_view input);

template<typename DB>
    requires (!std::is_reference_v<DB>)
std::pair<GetArgsReturnTypes, StringPtr> getString(const DBTypes dbType, DB &db, args_parsed &&args) {
    auto [type, val] = std::move(args);
    auto stringVal = String(std::move(val));
    if (type == ArgsType::CONSTANT) {
        auto res = std::make_unique<ConstantNode<String> >(std::move(stringVal));
        return {GetArgsReturnTypes::VALID, std::move(res)};
    }
    if constexpr (get_db_type<DB>::value == DBTypes::STRING) {
        if (type == ArgsType::KEY_FROM_CONSTANT) {
            auto key = stringBytes(std::move(val));
            auto res = std::make_unique<FromDB<String> >(db, std::move(key));
            return {GetArgsReturnTypes::VALID, std::move(res)};
        }
        if (type == ArgsType::KEY_FROM_FILE) {
            auto [res, key] = readFileBytes(val);
            if (res == BytesFromFileRes::VALID) {
                auto resPtr = std::make_unique<FromDB<String> >(db, std::move(key));
                return {GetArgsReturnTypes::VALID, std::move(resPtr)};
            }
            return {GetArgsReturnTypes::INVALID, {}};
        }
    }
    return {GetArgsReturnTypes::INVALID, {}};
}

parse_res<Integer> ParseInt(std::string_view input);

template<typename DB>
    requires (!std::is_reference_v<DB>)
std::pair<GetArgsReturnTypes, NumberPtr<Integer> > getInteger(const DBTypes dbType, DB &db, args_parsed &&args) {
    auto [type, val] = std::move(args);
    if (type == ArgsType::EMPTY) {
        return {GetArgsReturnTypes::INVALID, {}};
    }
    if (type == ArgsType::CONSTANT) {
        auto [res, valNum] = ParseInt(std::string_view(val.begin(), val.end()));
        if (res) {
            auto resPtr = std::make_unique<ConstantNode<Integer> >(valNum);
            return {GetArgsReturnTypes::VALID, std::move(resPtr)};
        }
        return {GetArgsReturnTypes::INVALID, {}};
    }
    if constexpr (get_db_type<DB>::value == DBTypes::INTEGER) {
        if (type == ArgsType::KEY_FROM_CONSTANT) {
            auto key = stringBytes(std::move(val));
            auto res = std::make_unique<FromDB<Integer> >(db, std::move(key));
            return {GetArgsReturnTypes::VALID, std::move(res)};
        }
        if (type == ArgsType::KEY_FROM_FILE) {
            auto [res, key] = readFileBytes(val);
            if (res == BytesFromFileRes::VALID) {
                auto resPtr = std::make_unique<FromDB<Integer> >(db, std::move(key));
                return {GetArgsReturnTypes::VALID, std::move(resPtr)};
            }
            return {GetArgsReturnTypes::INVALID, {}};
        }
    }

    return {GetArgsReturnTypes::INVALID, {}};
}

parse_res<Float> ParseFloat(std::string_view input);

template<typename DB>
    requires (!std::is_reference_v<DB>)
std::pair<GetArgsReturnTypes, NumberPtr<Float> > getFloat(DBTypes dbType, DB &db, args_parsed &&args) {
    auto [type, val] = std::move(args);
    if (type == ArgsType::EMPTY) {
        return {GetArgsReturnTypes::INVALID, {}};
    }
    if (type == ArgsType::CONSTANT) {
        auto [res, valNum] = ParseFloat(std::string_view(val.begin(), val.end()));
        if (res) {
            auto resPtr = std::make_unique<ConstantNode<Float> >(valNum);
            return {GetArgsReturnTypes::VALID, std::move(resPtr)};
        }
        return {GetArgsReturnTypes::INVALID, {}};
    }
    if constexpr (get_db_type<DB>::value == DBTypes::FLOAT) {
        if (type == ArgsType::KEY_FROM_CONSTANT) {
            auto key = stringBytes(std::move(val));
            auto res = std::make_unique<FromDB<Float> >(db, std::move(key));
            return {GetArgsReturnTypes::VALID, std::move(res)};
        }
        if (type == ArgsType::KEY_FROM_FILE) {
            auto [res, key] = readFileBytes(val);
            if (res == BytesFromFileRes::VALID) {
                auto resPtr = std::make_unique<FromDB<Float> >(db, std::move(key));
                return {GetArgsReturnTypes::VALID, std::move(resPtr)};
            }
            return {GetArgsReturnTypes::INVALID, {}};
        }
    }

    return {GetArgsReturnTypes::INVALID, {}};
}

enum class VectorParsingRes {
    VALID,
    INVALID_VECT,
    INVALID_ITEM
};

template<typename T>
using vector_parse_t = std::pair<VectorParsingRes, Vector<T> >;

template<typename T>
vector_parse_t<T> ParseVect(std::string_view input, std::function<std::pair<bool, T>(std::string_view)> converter) {
    if (input.empty() || input.front() != '{' || input.back() != '}') {
        return {VectorParsingRes::INVALID_VECT, Vector<T>()};
    }

    auto it = input.begin() + 1;
    const auto end_boundary = input.end() - 1;

    Vector<T> result;

    while (it < end_boundary) {
        while (it < end_boundary && std::isspace(static_cast<unsigned char>(*it))) {
            ++it;
        }

        if (it == end_boundary && result.Count() == 0) {
            break;
        }
        if (it == end_boundary) {
            return {VectorParsingRes::INVALID_VECT, Vector<T>()};
        }

        auto token_end = it;
        while (token_end < end_boundary && *token_end != ',') {
            ++token_end;
        }

        if (token_end == it) {
            return {VectorParsingRes::INVALID_VECT, Vector<T>()};
        }

        auto actual_end = token_end;
        while (actual_end > it && std::isspace(static_cast<unsigned char>(*(actual_end - 1)))) {
            --actual_end;
        }

        std::string_view token(it, actual_end);

        auto [isValid, value] = converter(token);
        if (!isValid) {
            return {VectorParsingRes::INVALID_ITEM, Vector<T>()};
        }

        result.Append(value);

        it = token_end + 1;
    }

    return {VectorParsingRes::VALID, result};
}

template<typename T>
struct DBVectType {
    static constexpr auto type = DBTypes::INVALID;
};

template<>
struct DBVectType<String> {
    static constexpr auto type = DBTypes::VECTOR_STRING;
};

template<>
struct DBVectType<Integer> {
    static constexpr auto type = DBTypes::VECTOR_INT;
};

template<>
struct DBVectType<Float> {
    static constexpr auto type = DBTypes::VECTOR_FLOAT;
};

template<typename T>
using DBVectType_t = DBVectType<T>::type;

template<typename T, typename DB>
    requires (!std::is_reference_v<DB>)
std::pair<GetArgsReturnTypes, VectPtr<T> > getVector(DBTypes dbType, DB &db, args_parsed &&args,
                                                     std::function<std::pair<bool, T>(std::string_view)>
                                                     converter) {
    auto [type, val] = std::move(args);
    if (type == ArgsType::EMPTY) {
        return {GetArgsReturnTypes::INVALID, {}};
    }
    if (type == ArgsType::CONSTANT) {
        auto [res, valVect] = ParseVect<T>(std::string_view(val.begin(), val.end()), converter);
        if (res == VectorParsingRes::VALID) {
            auto resPtr = std::make_unique<ConstantNode<Vector<T> > >(std::move(valVect));
            return {GetArgsReturnTypes::VALID, std::move(resPtr)};
        }
        return {GetArgsReturnTypes::INVALID, {}};
    }

    if constexpr (get_db_type<DB>::value == get_db_type<Vector<T> >::value) {
        if (type == ArgsType::KEY_FROM_CONSTANT) {
            auto key = stringBytes(std::move(val));
            auto res = std::make_unique<FromDB<Vector<T> > >(db, std::move(key));
        }

        if (type == ArgsType::KEY_FROM_FILE) {
            auto [res, key] = readFileBytes(val);
            if (res == BytesFromFileRes::VALID) {
                auto resPtr = std::make_unique<FromDB<Vector<T> > >(db, std::move(key));
                return {GetArgsReturnTypes::VALID, std::move(resPtr)};
            }
            return {GetArgsReturnTypes::INVALID, {}};
        }
    }

    return {GetArgsReturnTypes::INVALID, {}};
}

template<typename T, typename DB>
    requires (!std::is_reference_v<DB>)
std::pair<GetArgsReturnTypes, VectPtr<T> > getVectorFromDB(DBTypes dbType, DB &db, args_parsed &&args) {
    auto [type, val] = std::move(args);
    if (type == ArgsType::EMPTY) {
        return {GetArgsReturnTypes::INVALID, {}};
    }
    if (type == ArgsType::CONSTANT) {
        return {GetArgsReturnTypes::INVALID, {}};
    }

    if constexpr (get_db_type<DB>::value == get_db_type<Vector<T> >::value) {
        if (type == ArgsType::KEY_FROM_CONSTANT) {
            auto key = stringBytes(std::move(val));
            auto res = std::make_unique<FromDB<Vector<T> > >(db, std::move(key));
        }

        if (type == ArgsType::KEY_FROM_FILE) {
            auto [res, key] = readFileBytes(val);
            if (res == BytesFromFileRes::VALID) {
                auto resPtr = std::make_unique<FromDB<Vector<T> > >(db, std::move(key));
                return {GetArgsReturnTypes::VALID, std::move(resPtr)};
            }
            return {GetArgsReturnTypes::INVALID, {}};
        }
    }

    return {GetArgsReturnTypes::INVALID, {}};
}

enum class ParseOperationRes {
    VALID,
    INVALID_ARG_C,
    INVALID_ARGS
};


template<typename N>
using parse_op_r = std::pair<ParseOperationRes, std::unique_ptr<N> >;

template<typename DB, typename N>
parse_op_r<N> ParseIntNode(view_t &view, const DBTypes dbType, DB &db) {
    if (view.size() != 2) {
        return {ParseOperationRes::INVALID_ARG_C, {}};
    }
    auto parse1 = ParseArg(view);
    auto [type1, ptr1] = getInteger<DB>(dbType, db, std::move(parse1));
    auto parse2 = ParseArg(view);
    auto [type2, ptr2] = getInteger<DB>(dbType, db, std::move(parse2));


    if (GetArgsReturnTypes::INVALID == type1 || GetArgsReturnTypes::INVALID == type2) {
        return {ParseOperationRes::INVALID_ARGS, {}};
    }
    auto res = std::make_unique<N>(std::move(ptr1), std::move(ptr2));
    return {ParseOperationRes::VALID, std::move(res)};
}

template<typename DB, typename N>
parse_op_r<N> ParseFloatNode(view_t &view, const DBTypes dbType, DB &db) {
    if (view.size() != 2) {
        return {ParseOperationRes::INVALID_ARG_C, {}};
    }
    auto parse1 = ParseArg(view);
    auto [type1, ptr1] = getFloat<DB>(dbType, db, std::move(parse1));
    auto parse2 = ParseArg(view);
    auto [type2, ptr2] = getFloat<DB>(dbType, db, std::move(parse2));


    if (GetArgsReturnTypes::INVALID == type1 || GetArgsReturnTypes::INVALID == type2) {
        return {ParseOperationRes::INVALID_ARGS, {}};
    }
    auto res = std::make_unique<N>(std::move(ptr1), std::move(ptr2));
    return {ParseOperationRes::VALID, std::move(res)};
}

template<typename DB, typename N>
parse_op_r<N> ParseStringOpStringNum(view_t &view, const DBTypes dbType, DB &db) {
    if (view.size() != 2) {
        return {ParseOperationRes::INVALID_ARG_C, {}};
    }
    auto parse1 = ParseArg(view);
    auto [typeS, ptrS] = getString<DB>(dbType, db, std::move(parse1));
    auto parse2 = ParseArg(view);
    auto [typeI, ptrI] = getInteger<DB>(dbType, db, std::move(parse2));

    if (GetArgsReturnTypes::INVALID == typeS || GetArgsReturnTypes::INVALID == typeI) {
        return {ParseOperationRes::INVALID_ARGS, {}};
    }

    auto res = std::make_unique<N>(std::move(ptrS), std::move(ptrI));

    return {ParseOperationRes::VALID, std::move(res)};
}

template<typename DB, typename N>
parse_op_r<N> ParseStringOpStringString(view_t &view, const DBTypes dbType, DB &db) {
    if (view.size() != 2) {
        return {ParseOperationRes::INVALID_ARG_C, {}};
    }
    auto parse1 = ParseArg(view);
    auto [type1, ptr1] = getString<DB>(dbType, db, std::move(parse1));
    auto parse2 = ParseArg(view);
    auto [type2, ptr2] = getString<DB>(dbType, db, std::move(parse2));

    if (GetArgsReturnTypes::INVALID == type1 || GetArgsReturnTypes::INVALID == type2) {
        return {ParseOperationRes::INVALID_ARGS, {}};
    }

    auto res = std::make_unique<N>(std::move(ptr1), std::move(ptr2));

    return {ParseOperationRes::VALID, std::move(res)};
}

template<typename DB, typename OP, typename T>
parse_op_r<OP> ParseVectOpVectVal(view_t &view, DBTypes dbType, DB &db) {
    if (view.size() != 2) {
        return {ParseOperationRes::INVALID_ARG_C, {}};
    }

    if constexpr (get_db_type<T>::value == DBTypes::VECTOR_FLOAT) {
        auto parse = ParseArg(view);
        auto [type, ptr] = getVectorFromDB<Float, DB>(dbType, db, std::move(parse), ParseFloat);
        auto parseVal = ParseArg(view);
        auto [typeVal, ptrVal] = getFloat<DB>(dbType, db, std::move(parseVal));

        if (type == GetArgsReturnTypes::INVALID || typeVal == GetArgsReturnTypes::INVALID) {
            return {ParseOperationRes::INVALID_ARGS, {}};
        }

        auto res = std::make_unique<OP>(std::move(ptr), std::move(ptrVal));
        return {ParseOperationRes::VALID, std::move(res)};
    }
    if constexpr (get_db_type<T>::value == DBTypes::VECTOR_INT) {
        auto parse = ParseArg(view);
        auto [type, ptr] = getVectorFromDB<Integer, DB>(dbType, db, std::move(parse), ParseInt);
        auto parseVal = ParseArg(view);
        auto [typeVal, ptrVal] = getInteger<DB>(dbType, db, std::move(parseVal));

        if (type == GetArgsReturnTypes::INVALID || typeVal == GetArgsReturnTypes::INVALID) {
            return {ParseOperationRes::INVALID_ARGS, {}};
        }

        auto res = std::make_unique<OP>(std::move(ptr), std::move(ptrVal));
        return {ParseOperationRes::VALID, std::move(res)};
    }
    if constexpr (get_db_type<T>::value == DBTypes::VECTOR_STRING) {
        auto parse = ParseArg(view);
        auto [type, ptr] = getVectorFromDB<String, DB>(dbType, db, std::move(parse), ParseString);
        auto parseVal = ParseArg(view);
        auto [typeVal, ptrVal] = getString<DB>(dbType, db, std::move(parseVal));

        if (type == GetArgsReturnTypes::INVALID || typeVal == GetArgsReturnTypes::INVALID) {
            return {ParseOperationRes::INVALID_ARGS, {}};
        }

        auto res = std::make_unique<OP>(std::move(ptr), std::move(ptrVal));
        return {ParseOperationRes::VALID, std::move(res)};
    }
    return {ParseOperationRes::INVALID_ARGS, {}};
}

template<typename DB, typename OP, typename T>
parse_op_r<OP> ParseVectOpVect(view_t &view, DBTypes dbType, DB &db) {
    if (view.size() != 1) {
        return {ParseOperationRes::INVALID_ARG_C, {}};
    }

    if constexpr (get_db_type<Vector<T> >::value == DBTypes::VECTOR_FLOAT) {
        auto parse = ParseArg(view);
        auto [type, ptr] = getVectorFromDB<T, DB>(dbType, db, std::move(parse), ParseFloat);

        if (type == GetArgsReturnTypes::INVALID) {
            return {ParseOperationRes::INVALID_ARGS, {}};
        }

        auto res = std::make_unique<OP>(std::move(ptr));
        return {ParseOperationRes::VALID, std::move(res)};
    }
    if constexpr (get_db_type<Vector<T> >::value == DBTypes::VECTOR_INT) {
        auto parse = ParseArg(view);
        auto [type, ptr] = getVectorFromDB<T, DB>(dbType, db, std::move(parse), ParseInt);

        if (type == GetArgsReturnTypes::INVALID) {
            return {ParseOperationRes::INVALID_ARGS, {}};
        }

        auto res = std::make_unique<OP>(std::move(ptr));
        return {ParseOperationRes::VALID, std::move(res)};
    }
    if constexpr (get_db_type<Vector<T> >::value == DBTypes::VECTOR_STRING) {
        auto parse = ParseArg(view);
        auto [type, ptr] = getVectorFromDB<T, DB>(dbType, db, std::move(parse), ParseString);

        if (type == GetArgsReturnTypes::INVALID) {
            return {ParseOperationRes::INVALID_ARGS, {}};
        }

        auto res = std::make_unique<OP>(std::move(ptr));
        return {ParseOperationRes::VALID, std::move(res)};
    }
    return {ParseOperationRes::INVALID_ARGS, {}};
}

template<typename DB, typename OP, typename T>
parse_op_r<OP> ParseVectOpVectNumVal(view_t &view, DBTypes dbType, DB &db) {
    if (view.size() != 3) {
        return {ParseOperationRes::INVALID_ARG_C, {}};
    }

    if constexpr (get_db_type<T>::value == DBTypes::VECTOR_FLOAT) {
        auto parse = ParseArg(view);
        auto [type, ptr] = getVectorFromDB<Float, DB>(dbType, db, std::move(parse), ParseFloat);
        auto parseNum = ParseArg(view);
        auto [typeNum, ptrNum] = getInteger(dbType, db, std::move(parseNum));
        auto parseVal = ParseArg(view);
        auto [typeVal, ptrVal] = getFloat<DB>(dbType, db, std::move(parseVal));

        if (type == GetArgsReturnTypes::INVALID || typeVal == GetArgsReturnTypes::INVALID || typeNum ==
            GetArgsReturnTypes::INVALID) {
            return {ParseOperationRes::INVALID_ARGS, {}};
        }

        auto res = std::make_unique<OP>(std::move(ptr), std::move(ptrNum), std::move(ptrVal));
        return {ParseOperationRes::VALID, std::move(res)};
    }
    if constexpr (get_db_type<T>::value == DBTypes::VECTOR_INT) {
        auto parse = ParseArg(view);
        auto [type, ptr] = getVectorFromDB<Integer, DB>(dbType, db, std::move(parse), ParseInt);
        auto parseNum = ParseArg(view);
        auto [typeNum, ptrNum] = getInteger(dbType, db, std::move(parseNum));
        auto parseVal = ParseArg(view);
        auto [typeVal, ptrVal] = getInteger<DB>(dbType, db, std::move(parseVal));

        if (type == GetArgsReturnTypes::INVALID || typeVal == GetArgsReturnTypes::INVALID || typeNum ==
            GetArgsReturnTypes::INVALID) {
            return {ParseOperationRes::INVALID_ARGS, {}};
        }

        auto res = std::make_unique<OP>(std::move(ptr), std::move(ptrNum), std::move(ptrVal));
        return {ParseOperationRes::VALID, std::move(res)};
    }
    if constexpr (get_db_type<T>::value == DBTypes::VECTOR_STRING) {
        auto parse = ParseArg(view);
        auto [type, ptr] = getVectorFromDB<String, DB>(dbType, db, std::move(parse), ParseString);
        auto parseNum = ParseArg(view);
        auto [typeNum, ptrNum] = getInteger(dbType, db, std::move(parseNum));
        auto parseVal = ParseArg(view);
        auto [typeVal, ptrVal] = getString<DB>(dbType, db, std::move(parseVal));

        if (type == GetArgsReturnTypes::INVALID || typeVal == GetArgsReturnTypes::INVALID || typeNum ==
            GetArgsReturnTypes::INVALID) {
            return {ParseOperationRes::INVALID_ARGS, {}};
        }

        auto res = std::make_unique<OP>(std::move(ptr), std::move(ptrNum), std::move(ptrVal));
        return {ParseOperationRes::VALID, std::move(res)};
    }
    return {ParseOperationRes::INVALID_ARGS, {}};
}

template<typename DB, typename OP, typename T>
parse_op_r<OP> ParseVectOpVectNum(view_t &view, DBTypes dbType, DB &db) {
    if (view.size() != 2) {
        return {ParseOperationRes::INVALID_ARG_C, {}};
    }

    if constexpr (get_db_type<T>::value == DBTypes::VECTOR_FLOAT) {
        auto parse = ParseArg(view);
        auto [type, ptr] = getVectorFromDB<Float, DB>(dbType, db, std::move(parse), ParseFloat);
        auto parseNum = ParseArg(view);
        auto [typeNum, ptrNum] = getInteger(dbType, db, std::move(parseNum));

        if (type == GetArgsReturnTypes::INVALID || typeNum == GetArgsReturnTypes::INVALID) {
            return {ParseOperationRes::INVALID_ARGS, {}};
        }

        auto res = std::make_unique<OP>(std::move(ptr), std::move(ptrNum));
        return {ParseOperationRes::VALID, std::move(res)};
    }
    if constexpr (get_db_type<T>::value == DBTypes::VECTOR_INT) {
        auto parse = ParseArg(view);
        auto [type, ptr] = getVectorFromDB<Integer, DB>(dbType, db, std::move(parse), ParseInt);
        auto parseNum = ParseArg(view);
        auto [typeNum, ptrNum] = getInteger(dbType, db, std::move(parseNum));

        if (type == GetArgsReturnTypes::INVALID || typeNum == GetArgsReturnTypes::INVALID) {
            return {ParseOperationRes::INVALID_ARGS, {}};
        }

        auto res = std::make_unique<OP>(std::move(ptr), std::move(ptrNum));
        return {ParseOperationRes::VALID, std::move(res)};
    }
    if constexpr (get_db_type<T>::value == DBTypes::VECTOR_STRING) {
        auto parse = ParseArg(view);
        auto [type, ptr] = getVectorFromDB<String, DB>(dbType, db, std::move(parse), ParseString);
        auto parseNum = ParseArg(view);
        auto [typeNum, ptrNum] = getInteger(dbType, db, std::move(parseNum));

        if (type == GetArgsReturnTypes::INVALID || typeNum == GetArgsReturnTypes::INVALID) {
            return {ParseOperationRes::INVALID_ARGS, {}};
        }

        auto res = std::make_unique<OP>(std::move(ptr), std::move(ptrNum));
        return {ParseOperationRes::VALID, std::move(res)};
    }
    return {ParseOperationRes::INVALID_ARGS, {}};
}

enum class KeyType {
    INVALID,
    CONSTANT,
    FILE_PATH
};

using key_parse = std::pair<KeyType, std::string>;

key_parse ParseKey(view_t &view);

enum class TargetType {
    INVALID,
    FILE,
    KEY
};

using target_parse = std::pair<TargetType, std::string>;

target_parse ParseTarget(view_t &view);

ttl_t ParseTTL(view_t &view, ttl_t defaultTTL);

template<typename T, typename N>
parse_op_r<DBopsNode<T> > ParseOneKeyOP(view_t &view, std::ostream &stream, DBType<T> &db) {
    if (view.size() != 1) {
        auto res = std::make_unique<ErrorNode<T> >(InvalidArgsCount(1, view.size()));
        return {ParseOperationRes::INVALID_ARG_C, std::move(res)};
    }

    auto [type, val] = ParseKey(view);

    if (type == KeyType::INVALID) {
        auto res = std::make_unique<ErrorNode<T> >(InvalidArgs());
        return {ParseOperationRes::INVALID_ARGS, std::move(res)};
    }

    std::unique_ptr<DBopsNode<T> > res;
    if (type == KeyType::CONSTANT) {
        std::vector<uint8_t> bytes = stringBytes(std::move(val));
        res = std::make_unique<N>(db, std::move(bytes), stream);
    } else if (type == KeyType::FILE_PATH) {
        auto [msg, bytes] = readFileBytes(val);
        if (msg == BytesFromFileRes::COULD_NOT_OPEN) {
            res = std::make_unique<ErrorNode<T> >(CouldNotOpenFileErr(val));
        } else if (msg == BytesFromFileRes::COULD_NOT_READ) {
            res = std::make_unique<ErrorNode<T> >(CouldNotReadFileErr(val));
        } else {
            res = std::make_unique<N>(db, std::move(bytes), stream);
        }
    }
    return {ParseOperationRes::VALID, std::move(res)};
}

template<typename T>
parse_op_r<DBopsNode<T> > ParseInsertWithParser(view_t &view, DBType<T> &db, DBTypes dbType,
                                                std::function<std::pair<GetArgsReturnTypes, NodePtr<T> >(
                                                    DBTypes, DBType<T> &, args_parsed &&)> parser,
                                                ttl_t ttl) {
    if (view.size() != 2) {
        auto res = std::make_unique<ErrorNode<T> >(InvalidArgsCount(2, view.size()));
        return {ParseOperationRes::INVALID_ARG_C, std::move(res)};
    }

    auto [keyType, key] = ParseKey(view);

    if (keyType == KeyType::INVALID) {
        auto resNode = std::make_unique<ErrorNode<T> >(InvalidArgs());
        return {ParseOperationRes::INVALID_ARGS, std::move(resNode)};
    }

    auto arg = ParseArg(view);
    auto [res, ptr] = parser(dbType, db, std::move(arg));

    if (res == GetArgsReturnTypes::INVALID) {
        auto resNode = std::make_unique<ErrorNode<T> >(InvalidArgs());
        return {ParseOperationRes::INVALID_ARGS, std::move(resNode)};
    }

    std::vector<uint8_t> keyBytes;

    if (keyType == KeyType::CONSTANT) {
        keyBytes = stringBytes(std::move(key));
    }

    if (keyType == KeyType::FILE_PATH) {
        auto [fileRead, value] = readFileBytes(key);
        if (fileRead == BytesFromFileRes::COULD_NOT_READ) {
            auto resNode = std::make_unique<ErrorNode<T> >(CouldNotReadFileErr(key));
            return {ParseOperationRes::INVALID_ARGS, std::move(resNode)};
        }
        if (fileRead == BytesFromFileRes::COULD_NOT_OPEN) {
            auto resNode = std::make_unique<ErrorNode<T> >(CouldNotOpenFileErr(key));
            return {ParseOperationRes::INVALID_ARGS, std::move(resNode)};
        }
        keyBytes = std::move(value);
    }


    if (ttl.second == TTLTypes::IMMORTAL) {
        auto resNode = std::make_unique<InsertNode<T, ImmortalSentinel> >(
            db, std::move(keyBytes), std::move(ptr), std::move(ttl.first));
        return {ParseOperationRes::VALID, std::move(resNode)};
    }
    if (ttl.second == TTLTypes::MILLISECONDS) {
        auto resNode = std::make_unique<InsertNode<T, std::chrono::milliseconds> >(
            db, std::move(keyBytes), std::move(ptr), std::move(ttl.first));
        return {ParseOperationRes::VALID, std::move(resNode)};
    }
    if (ttl.second == TTLTypes::SECONDS) {
        auto resNode = std::make_unique<InsertNode<T, std::chrono::seconds> >(
            db, std::move(keyBytes), std::move(ptr), std::move(ttl.first));
        return {ParseOperationRes::VALID, std::move(resNode)};
    }
    if (ttl.second == TTLTypes::MINUTES) {
        auto resNode = std::make_unique<InsertNode<T, std::chrono::minutes> >(
            db, std::move(keyBytes), std::move(ptr), std::move(ttl.first));
        return {ParseOperationRes::VALID, std::move(resNode)};
    }

    auto resNode = std::make_unique<ErrorNode<T> >(InvalidArgs());
    return {ParseOperationRes::INVALID_ARGS, std::move(resNode)};
}

template<typename T>
parse_op_r<DBopsNode<T> > ParseInsert(view_t &view, DBType<T> &db, DBTypes dbType) {
    if constexpr (get_db_type<T>::value == DBTypes::FLOAT) {
        return ParseInsertWithParser<T>(view, db, dbType, getFloat<DBType<T> >);
    }
    if constexpr (get_db_type<T>::value == DBTypes::INTEGER) {
        return ParseInsertWithParser<T>(view, db, dbType, getInteger<DBType<T> >);
    }
    if constexpr (get_db_type<T>::value == DBTypes::STRING) {
        return ParseInsertWithParser<T>(view, db, dbType, getString<DBType<T> >);
    }
    if constexpr (get_db_type<T>::value == DBTypes::VECTOR_FLOAT) {
        return ParseInsertWithParser<T>(view, db, dbType, [](DBTypes Type, DBType<T> &dbRef, args_parsed &&args) {
            return getVector<T, DBType<T> >(Type, dbRef, std::move(args), ParseFloat);
        });
    }
    if constexpr (get_db_type<T>::value == DBTypes::VECTOR_INT) {
        return ParseInsertWithParser<T>(view, db, dbType, [](DBTypes Type, DBType<T> &dbRef, args_parsed &&args) {
            return getVector<T, DBType<T> >(Type, dbRef, std::move(args), ParseInt);
        });
    }
    if constexpr (get_db_type<T>::value == DBTypes::VECTOR_STRING) {
        return ParseInsertWithParser<T>(view, db, dbType, [](DBTypes Type, DBType<T> &dbRef, args_parsed &&args) {
            return getVector<T, DBType<T> >(Type, dbRef, std::move(args), ParseString);
        });
    } else {
        auto resNode = std::make_unique<ErrorNode<T> >(InvalidArgs());
        return {ParseOperationRes::INVALID_ARGS, std::move(resNode)};
    }
}

std::pair<uint64_t, uint64_t> sampleKey(uint64_t seed);


struct QueueEntry {
    Timepoint timepoint{};
    std::chrono::milliseconds ttl;
    std::vector<uint8_t> key;
    bool invalid = false;


    QueueEntry(const std::chrono::milliseconds ttl, const std::vector<uint8_t> &key) : ttl(ttl), key(key) {
    }

    [[nodiscard]] bool Expired() const {
        return timepoint.Elapsed(ttl) || invalid;
    }

    [[nodiscard]] std::chrono::milliseconds RemainingTime() const {
        return timepoint.RemainingTime(ttl);
    }

    [[nodiscard]] bool Invalid() const {
        return invalid;
    }
};

struct CompareQueueEntry {
    bool operator()(const std::unique_ptr<QueueEntry> &a, const std::unique_ptr<QueueEntry> &b) const;
};

template<typename T>
std::unique_ptr<ErrorNode<T> > GetError(const ParseOperationRes res, const std::size_t expected,
                                        const std::size_t actual) {
    if (res == ParseOperationRes::INVALID_ARG_C) {
        auto node = std::make_unique<ErrorNode<T> >(std::move(InvalidArgsCount(expected, actual)));
        return node;
    }
    if (res == ParseOperationRes::INVALID_ARGS) {
        auto node = std::make_unique<ErrorNode<T> >(std::move(InvalidArgs()));
        return node;
    }
    return {};
}

std::pair<bool, std::string> parseQuote(std::string_view &view);

std::string parseUnquote(std::string_view &view);

void SkipSpaces(std::string_view &view);

std::pair<bool, std::vector<std::string> > parseLine(const std::string &line);

void readThread(std::stop_token stoken, std::istream &stream, std::mutex &queueMutex,
                std::queue<std::string> &dataQueue);

bool HasTarget(OPType op);


template<typename T>
void RunDB(ttl_t &&defaultTTL, DBTypes dbType, std::istream &instream, std::ostream &outstream) {
    const auto seed = static_cast<uint64_t>(std::time(nullptr));
    auto [k1,k2] = sampleKey(seed);
    SwissTable<SipHash, DBEntry<T>, std::vector<uint8_t> > db(SipHash(k1, k2, 2, 4));
    std::priority_queue<std::unique_ptr<QueueEntry>, std::vector<std::unique_ptr<QueueEntry> >, CompareQueueEntry>
            ttlQueue;
    std::queue<std::string> dataQueue;
    std::mutex queueMutex;

    std::jthread reader([&instream, &dataQueue, &queueMutex](const std::stop_token &stoken) {
        readThread(stoken, instream, queueMutex, dataQueue);
    });


    bool run = true;


    while (run) {
        if (!ttlQueue.empty()) {
            if (ttlQueue.top()->Invalid()) {
                ttlQueue.pop();
            }
            if (ttlQueue.top()->Expired()) {
                auto key = ttlQueue.top()->key;
                auto entry = db.LookUp(std::move(key));
                entry.DeleteEntry();
            }
        }

        if (!dataQueue.empty()) {
            std::lock_guard<std::mutex> lock(queueMutex);
            if (dataQueue.empty()) {
                continue;
            }
            std::string line = std::move(dataQueue.front());
            dataQueue.pop();

            auto [res, lineParsed] = parseLine(line);
            if (!res) {
                outstream << "Invalid input: " << line << std::endl;
                continue;
            }
            view_t view(lineParsed.begin(), lineParsed.end());

            OPType type = GetOPType(view);


            if (type == OPType::TERMINATE) {
                run = false;
                reader.request_stop();
                continue;
            }

            auto ttl = ParseTTL(view, defaultTTL);

            std::unique_ptr<Node<T> > toExecute;
            if (type == OPType::GET) {
                auto [msg, ptr] = ParseOneKeyOP<T, GetNode<T> >(view, outstream, db);
                ptr.get()->Execute();
                continue;
            }
            if (type == OPType::DELETE) {
                auto [msg, ptr] = ParseOneKeyOP<T, DeleteNode<T> >(view, outstream, db);
                ptr.get()->Execute();
                continue;
            }
            if (type == OPType::INSERT) {
                auto [msg, ptr] = ParseInsert<T>(view, db, ttl);
                ptr.get()->Execute();
                continue;
            }

            auto [targetType, targetValue] = ParseTarget(view);
            if (targetType == TargetType::INVALID) {
                outstream << "Invalid target" << std::endl;
            }
            if constexpr (get_db_type<T>::value == DBTypes::INTEGER) {
                if (type == OPType::ADD) {
                    auto [msg, ptr] = ParseIntNode<DBType<T>, AddOp<Integer> >(view, dbType, db);
                    if (msg == ParseOperationRes::VALID) {
                        toExecute = std::move(ptr);
                    } else {
                        toExecute = GetError<T>(msg, 2, view.size());
                    }
                } else if (type == OPType::SUBTRACT) {
                    auto [msg, ptr] = ParseIntNode<DBType<T>, SubtractOp<Integer> >(view, dbType, db);
                    if (msg == ParseOperationRes::VALID) {
                        toExecute = std::move(ptr);
                    } else {
                        toExecute = GetError<T>(msg, 2, view.size());
                    }
                } else if (type == OPType::MULTIPLY) {
                    auto [msg, ptr] = ParseIntNode<DBType<T>, MultiplyOp<Integer> >(view, dbType, db);
                    if (msg == ParseOperationRes::VALID) {
                        toExecute = std::move(ptr);
                    } else {
                        toExecute = GetError<T>(msg, 2, view.size());
                    }
                } else if (type == OPType::DIVIDE) {
                    auto [msg, ptr] = ParseIntNode<DBType<T>, DivideOp<Integer> >(view, dbType, db);
                    if (msg == ParseOperationRes::VALID) {
                        toExecute = std::move(ptr);
                    } else {
                        toExecute = GetError<T>(msg, 2, view.size());
                    }
                }
            }

            if constexpr (get_db_type<T>::value == DBTypes::FLOAT) {
                if (type == OPType::FADD) {
                    auto [msg, ptr] = ParseIntNode<DBType<T>, AddOp<Float> >(view, dbType, db);
                    if (msg == ParseOperationRes::VALID) {
                        toExecute = std::move(ptr);
                    } else {
                        toExecute = GetError<T>(msg, 2, view.size());
                    }
                } else if (type == OPType::FSUBTRACT) {
                    auto [msg, ptr] = ParseIntNode<DBType<T>, SubtractOp<Float> >(view, dbType, db);
                    if (msg == ParseOperationRes::VALID) {
                        toExecute = std::move(ptr);
                    } else {
                        toExecute = GetError<T>(msg, 2, view.size());
                    }
                } else if (type == OPType::FMULTIPLY) {
                    auto [msg, ptr] = ParseIntNode<DBType<T>, MultiplyOp<Float> >(view, dbType, db);
                    if (msg == ParseOperationRes::VALID) {
                        toExecute = std::move(ptr);
                    } else {
                        toExecute = GetError<T>(msg, 2, view.size());
                    }
                } else if (type == OPType::FDIVIDE) {
                    auto [msg, ptr] = ParseIntNode<DBType<T>, DivideOp<Float> >(view, dbType, db);
                    if (msg == ParseOperationRes::VALID) {
                        toExecute = std::move(ptr);
                    } else {
                        toExecute = GetError<T>(msg, 2, view.size());
                    }
                }
            }

            if constexpr (get_db_type<T>::value == DBTypes::STRING) {
                if (type == OPType::CONCAT) {
                    auto [msg, ptr] = ParseStringOpStringString<DBType<T>, ConcatStrings>(view, dbType, db);
                    if (msg == ParseOperationRes::VALID) {
                        toExecute = std::move(ptr);
                    } else {
                        toExecute = GetError<T>(msg, 2, view.size());
                    }
                } else if (type == OPType::SUFFIX) {
                    auto [msg, ptr] = ParseStringOpStringNum<DBType<T>, SuffixNode>(view, dbType, db);
                    if (msg == ParseOperationRes::VALID) {
                        toExecute = std::move(ptr);
                    } else {
                        toExecute = GetError<T>(msg, 2, view.size());
                    }
                } else if (type == OPType::PREFIX) {
                    auto [msg, ptr] = ParseStringOpStringNum<DBType<T>, PrefixNode>(view, dbType, db);
                    if (msg == ParseOperationRes::VALID) {
                        toExecute = std::move(ptr);
                    } else {
                        toExecute = GetError<T>(msg, 2, view.size());
                    }
                } else if (type == OPType::AT) {
                    auto [msg, ptr] = ParseStringOpStringNum<DBType<T>, AtNode>(view, dbType, db);
                    if (msg == ParseOperationRes::VALID) {
                        toExecute = std::move(ptr);
                    } else {
                        toExecute = GetError<T>(msg, 2, view.size());
                    }
                } else if (type == OPType::FIND) {
                }
            }


            if (type == OPType::APPEND) {
                constexpr size_t expectedArgs = 2;
                if constexpr (get_db_type<T>::value == DBTypes::VECTOR_FLOAT) {
                    auto [msg, ptr] = ParseVectOpVectVal<DBType<T>, AppendNode<Float>, Vector<
                        Float> >(view, dbType, db);
                    if (msg == ParseOperationRes::VALID) {
                        toExecute = std::move(ptr);
                    } else {
                        toExecute = GetError<T>(msg, expectedArgs, view.size());
                    }
                }
                if constexpr (get_db_type<T>::value == DBTypes::VECTOR_INT) {
                    auto [msg, ptr] = ParseVectOpVectVal<DBType<T>, AppendNode<Integer>, Vector<Integer> >(
                        view, dbType, db);
                    if (msg == ParseOperationRes::VALID) {
                        toExecute = std::move(ptr);
                    } else {
                        toExecute = GetError<T>(msg, expectedArgs, view.size());
                    }
                }
                if constexpr (get_db_type<T>::value == DBTypes::VECTOR_STRING) {
                    auto [msg, ptr] = ParseVectOpVectVal<DBType<T>, AppendNode<String>, Vector<String> >(
                        view, dbType, db);
                    if (msg == ParseOperationRes::VALID) {
                        toExecute = std::move(ptr);
                    } else {
                        toExecute = GetError<T>(msg, expectedArgs, view.size());
                    }
                }
            } else if (type == OPType::POP) {
                constexpr size_t expectedArgs = 1;
                if constexpr (get_db_type<T>::value == DBTypes::VECTOR_FLOAT) {
                    auto [msg, ptr] = ParseVectOpVect<DBType<T>, PopNode<Float>, Vector<
                        Float> >(view, dbType, db);
                    if (msg == ParseOperationRes::VALID) {
                        toExecute = std::move(ptr);
                    } else {
                        toExecute = GetError<T>(msg, expectedArgs, view.size());
                    }
                }
                if constexpr (get_db_type<T>::value == DBTypes::VECTOR_INT) {
                    auto [msg, ptr] = ParseVectOpVect<DBType<T>, PopNode<Integer>, Vector<Integer> >(
                        view, dbType, db);
                    if (msg == ParseOperationRes::VALID) {
                        toExecute = std::move(ptr);
                    } else {
                        toExecute = GetError<T>(msg, expectedArgs, view.size());
                    }
                }
                if constexpr (get_db_type<T>::value == DBTypes::VECTOR_STRING) {
                    auto [msg, ptr] = ParseVectOpVect<DBType<T>, PopNode<String>, Vector<String> >(
                        view, dbType, db);
                    if (msg == ParseOperationRes::VALID) {
                        toExecute = std::move(ptr);
                    } else {
                        toExecute = GetError<T>(msg, expectedArgs, view.size());
                    }
                }
            } else if (type == OPType::VINSERT) {
                constexpr size_t expectedArgs = 3;
                if constexpr (get_db_type<T>::value == DBTypes::VECTOR_FLOAT) {
                    auto [msg, ptr] = ParseVectOpVectNumVal<DBType<T>, VInsertNode<Float>, Vector<
                        Float> >(view, dbType, db);
                    if (msg == ParseOperationRes::VALID) {
                        toExecute = std::move(ptr);
                    } else {
                        toExecute = GetError<T>(msg, expectedArgs, view.size());
                    }
                }
                if constexpr (get_db_type<T>::value == DBTypes::VECTOR_INT) {
                    auto [msg, ptr] = ParseVectOpVectNumVal<DBType<T>, VInsertNode<Integer>, Vector<Integer> >(
                        view, dbType, db);
                    if (msg == ParseOperationRes::VALID) {
                        toExecute = std::move(ptr);
                    } else {
                        toExecute = GetError<T>(msg, expectedArgs, view.size());
                    }
                }
                if constexpr (get_db_type<T>::value == DBTypes::VECTOR_STRING) {
                    auto [msg, ptr] = ParseVectOpVectNumVal<DBType<T>, VInsertNode<String>, Vector<String> >(
                        view, dbType, db);
                    if (msg == ParseOperationRes::VALID) {
                        toExecute = std::move(ptr);
                    } else {
                        toExecute = GetError<T>(msg, expectedArgs, view.size());
                    }
                }
            } else if (type == OPType::REMOVE) {
                constexpr size_t expectedArgs = 2;
                if constexpr (get_db_type<T>::value == DBTypes::VECTOR_FLOAT) {
                    auto [msg, ptr] = ParseVectOpVectNum<DBType<T>, RemoveNode<Float>, Vector<
                        Float> >(view, dbType, db);
                    if (msg == ParseOperationRes::VALID) {
                        toExecute = std::move(ptr);
                    } else {
                        toExecute = GetError<T>(msg, expectedArgs, view.size());
                    }
                }
                if constexpr (get_db_type<T>::value == DBTypes::VECTOR_INT) {
                    auto [msg, ptr] = ParseVectOpVectNum<DBType<T>, RemoveNode<Integer>, Vector<Integer> >(
                        view, dbType, db);
                    if (msg == ParseOperationRes::VALID) {
                        toExecute = std::move(ptr);
                    } else {
                        toExecute = GetError<T>(msg, expectedArgs, view.size());
                    }
                }
                if constexpr (get_db_type<T>::value == DBTypes::VECTOR_STRING) {
                    auto [msg, ptr] = ParseVectOpVectNum<DBType<T>, RemoveNode<String>, Vector<String> >(
                        view, dbType, db);
                    if (msg == ParseOperationRes::VALID) {
                        toExecute = std::move(ptr);
                    } else {
                        toExecute = GetError<T>(msg, expectedArgs, view.size());
                    }
                }
            } else if (type == OPType::SET) {
                constexpr size_t expectedArgs = 3;
                if constexpr (get_db_type<T>::value == DBTypes::VECTOR_FLOAT) {
                    auto [msg, ptr] = ParseVectOpVectNumVal<DBType<T>, SetNode<Float>, Vector<
                        Float> >(view, dbType, db);
                    if (msg == ParseOperationRes::VALID) {
                        toExecute = std::move(ptr);
                    } else {
                        toExecute = GetError<T>(msg, expectedArgs, view.size());
                    }
                }
                if constexpr (get_db_type<T>::value == DBTypes::VECTOR_INT) {
                    auto [msg, ptr] = ParseVectOpVectNumVal<DBType<T>, SetNode<Integer>, Vector<Integer> >(
                        view, dbType, db);
                    if (msg == ParseOperationRes::VALID) {
                        toExecute = std::move(ptr);
                    } else {
                        toExecute = GetError<T>(msg, expectedArgs, view.size());
                    }
                }
                if constexpr (get_db_type<T>::value == DBTypes::VECTOR_STRING) {
                    auto [msg, ptr] = ParseVectOpVectNumVal<DBType<T>, SetNode<String>, Vector<String> >(
                        view, dbType, db);
                    if (msg == ParseOperationRes::VALID) {
                        toExecute = std::move(ptr);
                    } else {
                        toExecute = GetError<T>(msg, expectedArgs, view.size());
                    }
                }
            } else if (type == OPType::VGET) {
                constexpr size_t expectedArgs = 2;
                if constexpr (get_db_type<T>::value == DBTypes::VECTOR_FLOAT) {
                    auto [msg, ptr] = ParseVectOpVectNum<DBType<T>, VgetNode<Float>, Vector<
                        Float> >(view, dbType, db);
                    if (msg == ParseOperationRes::VALID) {
                        toExecute = std::move(ptr);
                    } else {
                        toExecute = GetError<T>(msg, expectedArgs, view.size());
                    }
                }
                if constexpr (get_db_type<T>::value == DBTypes::VECTOR_INT) {
                    auto [msg, ptr] = ParseVectOpVectNum<DBType<T>, VgetNode<Integer>, Vector<Integer> >(
                        view, dbType, db);
                    if (msg == ParseOperationRes::VALID) {
                        toExecute = std::move(ptr);
                    } else {
                        toExecute = GetError<T>(msg, expectedArgs, view.size());
                    }
                }
                if constexpr (get_db_type<T>::value == DBTypes::VECTOR_STRING) {
                    auto [msg, ptr] = ParseVectOpVectNum<DBType<T>, VgetNode<String>, Vector<String> >(
                        view, dbType, db);
                    if (msg == ParseOperationRes::VALID) {
                        toExecute = std::move(ptr);
                    } else {
                        toExecute = GetError<T>(msg, expectedArgs, view.size());
                    }
                }
            } else if (type == OPType::SIZE) {
                constexpr size_t expectedArgs = 1;
                if constexpr (get_db_type<T>::value == DBTypes::VECTOR_FLOAT) {
                    auto [msg, ptr] = ParseVectOpVect<DBType<T>, SizeNode<Float>, Vector<
                        Float> >(view, dbType, db);
                    if (msg == ParseOperationRes::VALID) {
                        toExecute = std::move(ptr);
                    } else {
                        toExecute = GetError<T>(msg, expectedArgs, view.size());
                    }
                }
                if constexpr (get_db_type<T>::value == DBTypes::VECTOR_INT) {
                    auto [msg, ptr] = ParseVectOpVect<DBType<T>, SizeNode<Integer>, Vector<Integer> >(
                        view, dbType, db);
                    if (msg == ParseOperationRes::VALID) {
                        toExecute = std::move(ptr);
                    } else {
                        toExecute = GetError<T>(msg, expectedArgs, view.size());
                    }
                }
                if constexpr (get_db_type<T>::value == DBTypes::VECTOR_STRING) {
                    auto [msg, ptr] = ParseVectOpVect<DBType<T>, SizeNode<String>, Vector<String> >(
                        view, dbType, db);
                    if (msg == ParseOperationRes::VALID) {
                        toExecute = std::move(ptr);
                    } else {
                        toExecute = GetError<T>(msg, expectedArgs, view.size());
                    }
                }
            } else {
                auto ptr = std::make_unique<ErrorNode<T> >(std::move(InvalidOperation()));
                toExecute = std::move(ptr);
            }

            std::unique_ptr<DBopsNode<T> > opNode = ErrorNode<T>(outstream, {"Error"});
            if (ValidateOPType(type, dbType) && targetType == TargetType::KEY) {
                auto key = stringBytes(std::move(targetValue));
                if (ttl.second == TTLTypes::MILLISECONDS) {
                    opNode = std::unique_ptr<InsertNode<T, std::chrono::milliseconds> >(
                        db, std::move(key), std::move(toExecute), ttl.first);
                } else if (ttl.second == TTLTypes::SECONDS) {
                    opNode = std::unique_ptr<InsertNode<T, std::chrono::seconds> >(
                        db, std::move(key), std::move(toExecute), ttl.first);
                } else if (ttl.second == TTLTypes::MINUTES) {
                    opNode = std::unique_ptr<InsertNode<T, std::chrono::minutes> >(
                        db, std::move(key), std::move(toExecute), ttl.first);
                } else {
                    opNode = std::unique_ptr<InsertNode<T, ImmortalSentinel> >(
                        db, std::move(key), std::move(toExecute), ttl.first);
                }
            } else if (targetType == TargetType::FILE) {
                std::ofstream outFile(targetValue);
                if (!outFile.is_open()) {
                    opNode = std::make_unique<ErrorNode<T> >({"Could not open file: ", targetValue});
                }
                opNode = std::make_unique<ToStreamNode<T, T> >(outFile, std::move(toExecute));
            } else {
                opNode = std::make_unique<ErrorNode<T> >(outstream, {"Error"});
            }
        }
    }
}
#endif //PROJECT_OPERATIONTYPE_HPP
