#ifndef PROJECT_OPERATIONTYPE_HPP
#define PROJECT_OPERATIONTYPE_HPP
#include <functional>
#include <ranges>
#include <vector>
#include "Nodes.hpp"
#include "DBType.hpp"
#include "Vector.hpp"
#include "ValueTypes.hpp"
#include <fstream>
#include <filesystem>
#include <queue>
#include <random>
#include <stop_token>
#include <thread>
#include "QueueEntry.hpp"
#include "DBEntry.hpp"
#include "Error.hpp"
#include "Utils.hpp"


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

enum class ArgsType {
    CONSTANT,
    KEY_FROM_FILE,
    KEY_FROM_CONSTANT,
    EMPTY
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

enum class ObjectFieldTypes {
    INTEGER,
    FLOAT,
    STRING,
    INVALID_TYPE,
    INVALID_FIELD,
    VALID
};

using object_reference = std::unordered_map<std::string, ObjectFieldTypes>;

std::tuple<std::string, ObjectFieldTypes, bool> ParseFieldStartup(std::string_view view);


std::tuple<std::string, ObjectFieldTypes, std::string> ParseField(std::string_view view);


parse_res<String> ParseString(std::string_view input);

template<typename DB>
    requires (!std::is_reference_v<DB>)
std::pair<GetArgsReturnTypes, StringPtr> getString(DB &db, args_parsed &&args) {
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
            if (auto [res, key] = readFileBytes(val); res == BytesFromFileRes::VALID) {
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
std::pair<GetArgsReturnTypes, NumberPtr<Integer> > getInteger(DB &db, args_parsed &&args) {
    auto [type, val] = std::move(args);
    if (type == ArgsType::EMPTY) {
        return {GetArgsReturnTypes::INVALID, {}};
    }
    if (type == ArgsType::CONSTANT) {
        if (auto [res, valNum] = ParseInt(std::string_view(val.begin(), val.end())); res) {
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
            if (auto [res, key] = readFileBytes(val); res == BytesFromFileRes::VALID) {
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
std::pair<GetArgsReturnTypes, NumberPtr<Float> > getFloat(DB &db, args_parsed &&args) {
    auto [type, val] = std::move(args);
    if (type == ArgsType::EMPTY) {
        return {GetArgsReturnTypes::INVALID, {}};
    }
    if (type == ArgsType::CONSTANT) {
        if (auto [res, valNum] = ParseFloat(std::string_view(val.begin(), val.end())); res) {
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
            if (auto [res, key] = readFileBytes(val); res == BytesFromFileRes::VALID) {
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
vector_parse_t<typename vector_t<T>::type> ParseVect(const std::string_view input,
                                                     std::function<std::pair<bool, typename vector_t<T>::type>(
                                                         std::string_view)> converter) {
    if (input.empty() || input.front() != '{' || input.back() != '}') {
        return {VectorParsingRes::INVALID_VECT, Vector<typename vector_t<T>::type>()};
    }

    auto it = input.begin() + 1;
    const auto end_boundary = input.end() - 1;

    Vector<typename vector_t<T>::type> result;

    while (it < end_boundary) {
        while (it < end_boundary && std::isspace(static_cast<unsigned char>(*it))) {
            ++it;
        }

        if (it == end_boundary && result.Count() == 0) {
            break;
        }
        if (it == end_boundary) {
            return {VectorParsingRes::INVALID_VECT, Vector<typename vector_t<T>::type>()};
        }

        auto token_end = it;
        while (token_end < end_boundary && *token_end != ',') {
            ++token_end;
        }

        if (token_end == it) {
            return {VectorParsingRes::INVALID_VECT, Vector<typename vector_t<T>::type>()};
        }

        auto actual_end = token_end;
        while (actual_end > it && std::isspace(static_cast<unsigned char>(*(actual_end - 1)))) {
            --actual_end;
        }

        std::string_view token(it, actual_end);

        auto [isValid, value] = converter(token);
        if (!isValid) {
            return {VectorParsingRes::INVALID_ITEM, Vector<typename vector_t<T>::type>()};
        }

        result.Append(std::move(value));

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
std::pair<GetArgsReturnTypes, VectPtr<typename vector_t<T>::type> > getVector(
    DB &db, args_parsed &&args,
    std::function<std::pair<bool, typename vector_t<T>::type>(std::string_view)> converter) {
    auto [type, val] = std::move(args);
    if (type == ArgsType::EMPTY) {
        return {GetArgsReturnTypes::INVALID, {}};
    }
    if (type == ArgsType::CONSTANT) {
        auto [res, valVect] = ParseVect<T>(std::string_view(val.begin(), val.end()), converter);
        if (res == VectorParsingRes::VALID) {
            auto resPtr = std::make_unique<ConstantNode<Vector<typename vector_t<T>::type> > >(std::move(valVect));
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
            if (auto [res, key] = readFileBytes(val); res == BytesFromFileRes::VALID) {
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
std::pair<GetArgsReturnTypes, VectPtr<T> > getVectorFromDB(DB &db, args_parsed &&args) {
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
            return {GetArgsReturnTypes::VALID, std::move(res)};
        }

        if (type == ArgsType::KEY_FROM_FILE) {
            if (auto [res, key] = readFileBytes(val); res == BytesFromFileRes::VALID) {
                auto resPtr = std::make_unique<FromDB<Vector<T> > >(db, std::move(key));
                return {GetArgsReturnTypes::VALID, std::move(resPtr)};
            }
            return {GetArgsReturnTypes::INVALID, {}};
        }
    }

    return {GetArgsReturnTypes::INVALID, {}};
}

parse_res<Object> ParseObject(std::string_view input, Object referenceObject, const object_reference &reference);

template<typename DB>
    requires (!std::is_reference_v<DB>)
std::pair<GetArgsReturnTypes, NodePtr<Object> > getObject(DB &db, args_parsed &&args, const Object &referenceObject,
                                                          const object_reference &reference) {
    auto [type, val] = std::move(args);
    if (type == ArgsType::EMPTY) {
        return {GetArgsReturnTypes::INVALID, {}};
    }
    if (type == ArgsType::CONSTANT) {
        if (auto [res, valVect] = ParseObject(std::string_view(val.begin(), val.end()), referenceObject, reference);
            res) {
            auto resPtr = std::make_unique<ConstantNode<Object> >(std::move(valVect));
            return {GetArgsReturnTypes::VALID, std::move(resPtr)};
        }
        return {GetArgsReturnTypes::INVALID, {}};
    }

    if constexpr (get_db_type<DB>::value == DBTypes::OBJECT) {
        if (type == ArgsType::KEY_FROM_CONSTANT) {
            auto key = stringBytes(std::move(val));
            auto res = std::make_unique<FromDB<Object> >(db, std::move(key));
        }

        if (type == ArgsType::KEY_FROM_FILE) {
            if (auto [res, key] = readFileBytes(val); res == BytesFromFileRes::VALID) {
                auto resPtr = std::make_unique<FromDB<Object> >(db, std::move(key));
                return {GetArgsReturnTypes::VALID, std::move(resPtr)};
            }
            return {GetArgsReturnTypes::INVALID, {}};
        }
    }

    return {GetArgsReturnTypes::INVALID, {}};
}

template<typename DB>
    requires (!std::is_reference_v<DB>)
std::pair<GetArgsReturnTypes, NodePtr<Object> > getObjectFromDB(DB &db, args_parsed &&args) {
    auto [type, val] = std::move(args);
    if (type == ArgsType::EMPTY) {
        return {GetArgsReturnTypes::INVALID, {}};
    }
    if (type == ArgsType::CONSTANT) {
        return {GetArgsReturnTypes::INVALID, {}};
    }

    if constexpr (get_db_type<DB>::value == DBTypes::OBJECT) {
        if (type == ArgsType::KEY_FROM_CONSTANT) {
            auto key = stringBytes(std::move(val));
            auto res = std::make_unique<FromDB<Object> >(db, std::move(key));
            return {GetArgsReturnTypes::VALID, std::move(res)};
        }

        if (type == ArgsType::KEY_FROM_FILE) {
            if (auto [res, key] = readFileBytes(val); res == BytesFromFileRes::VALID) {
                auto resPtr = std::make_unique<FromDB<Object> >(db, std::move(key));
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
    INVALID_ARGS,
    COULD_NOT_READ_FILE,
    COULD_NOT_OPEN_FILE
};


template<typename N>
using parse_op_r = std::pair<ParseOperationRes, std::unique_ptr<N> >;

template<typename DB, typename N>
parse_op_r<N> ParseIntNode(view_t &view, DB &db) {
    if (view.size() != 2) {
        return {ParseOperationRes::INVALID_ARG_C, {}};
    }
    auto parse1 = ParseArg(view);
    auto [type1, ptr1] = getInteger<DB>(db, std::move(parse1));
    auto parse2 = ParseArg(view);
    auto [type2, ptr2] = getInteger<DB>(db, std::move(parse2));


    if (GetArgsReturnTypes::INVALID == type1 || GetArgsReturnTypes::INVALID == type2) {
        return {ParseOperationRes::INVALID_ARGS, {}};
    }
    auto res = std::make_unique<N>(std::move(ptr1), std::move(ptr2));
    return {ParseOperationRes::VALID, std::move(res)};
}

template<typename DB, typename N>
parse_op_r<N> ParseFloatNode(view_t &view, DB &db) {
    if (view.size() != 2) {
        return {ParseOperationRes::INVALID_ARG_C, {}};
    }
    auto parse1 = ParseArg(view);
    auto [type1, ptr1] = getFloat<DB>(db, std::move(parse1));
    auto parse2 = ParseArg(view);
    auto [type2, ptr2] = getFloat<DB>(db, std::move(parse2));


    if (GetArgsReturnTypes::INVALID == type1 || GetArgsReturnTypes::INVALID == type2) {
        return {ParseOperationRes::INVALID_ARGS, {}};
    }
    auto res = std::make_unique<N>(std::move(ptr1), std::move(ptr2));
    return {ParseOperationRes::VALID, std::move(res)};
}

template<typename DB, typename N>
parse_op_r<N> ParseStringOpStringNum(view_t &view, DB &db) {
    if (view.size() != 2) {
        return {ParseOperationRes::INVALID_ARG_C, {}};
    }
    auto parse1 = ParseArg(view);
    auto [typeS, ptrS] = getString<DB>(db, std::move(parse1));
    auto parse2 = ParseArg(view);
    auto [typeI, ptrI] = getInteger<DB>(db, std::move(parse2));

    if (GetArgsReturnTypes::INVALID == typeS || GetArgsReturnTypes::INVALID == typeI) {
        return {ParseOperationRes::INVALID_ARGS, {}};
    }

    auto res = std::make_unique<N>(std::move(ptrS), std::move(ptrI));

    return {ParseOperationRes::VALID, std::move(res)};
}

template<typename DB, typename N>
parse_op_r<N> ParseStringOpStringString(view_t &view, DB &db) {
    if (view.size() != 2) {
        return {ParseOperationRes::INVALID_ARG_C, {}};
    }
    auto parse1 = ParseArg(view);
    auto [type1, ptr1] = getString<DB>(db, std::move(parse1));
    auto parse2 = ParseArg(view);
    auto [type2, ptr2] = getString<DB>(db, std::move(parse2));

    if (GetArgsReturnTypes::INVALID == type1 || GetArgsReturnTypes::INVALID == type2) {
        return {ParseOperationRes::INVALID_ARGS, {}};
    }

    auto res = std::make_unique<N>(std::move(ptr1), std::move(ptr2));

    return {ParseOperationRes::VALID, std::move(res)};
}

template<typename DB, typename OP, typename T>
parse_op_r<OP> ParseVectOpVectVal(view_t &view, DB &db) {
    if (view.size() != 2) {
        return {ParseOperationRes::INVALID_ARG_C, {}};
    }

    if constexpr (get_db_type<T>::value == DBTypes::VECTOR_FLOAT) {
        auto parse = ParseArg(view);
        auto [type, ptr] = getVectorFromDB<Float, DB>(db, std::move(parse));
        auto parseVal = ParseArg(view);
        auto [typeVal, ptrVal] = getFloat<DB>(db, std::move(parseVal));

        if (type == GetArgsReturnTypes::INVALID || typeVal == GetArgsReturnTypes::INVALID) {
            return {ParseOperationRes::INVALID_ARGS, {}};
        }

        auto res = std::make_unique<OP>(std::move(ptr), std::move(ptrVal));
        return {ParseOperationRes::VALID, std::move(res)};
    }
    if constexpr (get_db_type<T>::value == DBTypes::VECTOR_INT) {
        auto parse = ParseArg(view);
        auto [type, ptr] = getVectorFromDB<Integer, DB>(db, std::move(parse));
        auto parseVal = ParseArg(view);
        auto [typeVal, ptrVal] = getInteger<DB>(db, std::move(parseVal));

        if (type == GetArgsReturnTypes::INVALID || typeVal == GetArgsReturnTypes::INVALID) {
            return {ParseOperationRes::INVALID_ARGS, {}};
        }

        auto res = std::make_unique<OP>(std::move(ptr), std::move(ptrVal));
        return {ParseOperationRes::VALID, std::move(res)};
    }
    if constexpr (get_db_type<T>::value == DBTypes::VECTOR_STRING) {
        auto parse = ParseArg(view);
        auto [type, ptr] = getVectorFromDB<String, DB>(db, std::move(parse));
        auto parseVal = ParseArg(view);
        auto [typeVal, ptrVal] = getString<DB>(db, std::move(parseVal));

        if (type == GetArgsReturnTypes::INVALID || typeVal == GetArgsReturnTypes::INVALID) {
            return {ParseOperationRes::INVALID_ARGS, {}};
        }

        auto res = std::make_unique<OP>(std::move(ptr), std::move(ptrVal));
        return {ParseOperationRes::VALID, std::move(res)};
    }
    return {ParseOperationRes::INVALID_ARGS, {}};
}

template<typename DB, typename OP, typename T>
parse_op_r<OP> ParseVectOpVect(view_t &view, DB &db) {
    if (view.size() != 1) {
        return {ParseOperationRes::INVALID_ARG_C, {}};
    }

    if constexpr (get_db_type<T>::value == DBTypes::VECTOR_FLOAT) {
        auto parse = ParseArg(view);
        auto [type, ptr] = getVectorFromDB<Float, DB>(db, std::move(parse));

        if (type == GetArgsReturnTypes::INVALID) {
            return {ParseOperationRes::INVALID_ARGS, {}};
        }

        auto res = std::make_unique<OP>(std::move(ptr));
        return {ParseOperationRes::VALID, std::move(res)};
    }
    if constexpr (get_db_type<T>::value == DBTypes::VECTOR_INT) {
        auto parse = ParseArg(view);
        auto [type, ptr] = getVectorFromDB<Integer, DB>(db, std::move(parse));

        if (type == GetArgsReturnTypes::INVALID) {
            return {ParseOperationRes::INVALID_ARGS, {}};
        }

        auto res = std::make_unique<OP>(std::move(ptr));
        return {ParseOperationRes::VALID, std::move(res)};
    }
    if constexpr (get_db_type<T>::value == DBTypes::VECTOR_STRING) {
        auto parse = ParseArg(view);
        auto [type, ptr] = getVectorFromDB<String, DB>(db, std::move(parse));

        if (type == GetArgsReturnTypes::INVALID) {
            return {ParseOperationRes::INVALID_ARGS, {}};
        }

        auto res = std::make_unique<OP>(std::move(ptr));
        return {ParseOperationRes::VALID, std::move(res)};
    }
    return {ParseOperationRes::INVALID_ARGS, {}};
}

template<typename DB, typename OP, typename T>
parse_op_r<OP> ParseVectOpVectNumVal(view_t &view, DB &db) {
    if (view.size() != 3) {
        return {ParseOperationRes::INVALID_ARG_C, {}};
    }

    if constexpr (get_db_type<T>::value == DBTypes::VECTOR_FLOAT) {
        auto parse = ParseArg(view);
        auto [type, ptr] = getVectorFromDB<Float, DB>(db, std::move(parse));
        auto parseNum = ParseArg(view);
        auto [typeNum, ptrNum] = getInteger(db, std::move(parseNum));
        auto parseVal = ParseArg(view);
        auto [typeVal, ptrVal] = getFloat<DB>(db, std::move(parseVal));

        if (type == GetArgsReturnTypes::INVALID || typeVal == GetArgsReturnTypes::INVALID || typeNum ==
            GetArgsReturnTypes::INVALID) {
            return {ParseOperationRes::INVALID_ARGS, {}};
        }

        auto res = std::make_unique<OP>(std::move(ptr), std::move(ptrNum), std::move(ptrVal));
        return {ParseOperationRes::VALID, std::move(res)};
    }
    if constexpr (get_db_type<T>::value == DBTypes::VECTOR_INT) {
        auto parse = ParseArg(view);
        auto [type, ptr] = getVectorFromDB<Integer, DB>(db, std::move(parse));
        auto parseNum = ParseArg(view);
        auto [typeNum, ptrNum] = getInteger(db, std::move(parseNum));
        auto parseVal = ParseArg(view);
        auto [typeVal, ptrVal] = getInteger<DB>(db, std::move(parseVal));

        if (type == GetArgsReturnTypes::INVALID || typeVal == GetArgsReturnTypes::INVALID || typeNum ==
            GetArgsReturnTypes::INVALID) {
            return {ParseOperationRes::INVALID_ARGS, {}};
        }

        auto res = std::make_unique<OP>(std::move(ptr), std::move(ptrNum), std::move(ptrVal));
        return {ParseOperationRes::VALID, std::move(res)};
    }
    if constexpr (get_db_type<T>::value == DBTypes::VECTOR_STRING) {
        auto parse = ParseArg(view);
        auto [type, ptr] = getVectorFromDB<String, DB>(db, std::move(parse));
        auto parseNum = ParseArg(view);
        auto [typeNum, ptrNum] = getInteger(db, std::move(parseNum));
        auto parseVal = ParseArg(view);
        auto [typeVal, ptrVal] = getString<DB>(db, std::move(parseVal));

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
parse_op_r<OP> ParseVectOpVectNum(view_t &view, DB &db) {
    if (view.size() != 2) {
        return {ParseOperationRes::INVALID_ARG_C, {}};
    }

    if constexpr (get_db_type<T>::value == DBTypes::VECTOR_FLOAT) {
        auto parse = ParseArg(view);
        auto [type, ptr] = getVectorFromDB<Float, DB>(db, std::move(parse));
        auto parseNum = ParseArg(view);
        auto [typeNum, ptrNum] = getInteger(db, std::move(parseNum));

        if (type == GetArgsReturnTypes::INVALID || typeNum == GetArgsReturnTypes::INVALID) {
            return {ParseOperationRes::INVALID_ARGS, {}};
        }

        auto res = std::make_unique<OP>(std::move(ptr), std::move(ptrNum));
        return {ParseOperationRes::VALID, std::move(res)};
    }
    if constexpr (get_db_type<T>::value == DBTypes::VECTOR_INT) {
        auto parse = ParseArg(view);
        auto [type, ptr] = getVectorFromDB<Integer, DB>(db, std::move(parse));
        auto parseNum = ParseArg(view);
        auto [typeNum, ptrNum] = getInteger(db, std::move(parseNum));

        if (type == GetArgsReturnTypes::INVALID || typeNum == GetArgsReturnTypes::INVALID) {
            return {ParseOperationRes::INVALID_ARGS, {}};
        }

        auto res = std::make_unique<OP>(std::move(ptr), std::move(ptrNum));
        return {ParseOperationRes::VALID, std::move(res)};
    }
    if constexpr (get_db_type<T>::value == DBTypes::VECTOR_STRING) {
        auto parse = ParseArg(view);
        auto [type, ptr] = getVectorFromDB<String, DB>(db, std::move(parse));
        auto parseNum = ParseArg(view);
        auto [typeNum, ptrNum] = getInteger(db, std::move(parseNum));

        if (type == GetArgsReturnTypes::INVALID || typeNum == GetArgsReturnTypes::INVALID) {
            return {ParseOperationRes::INVALID_ARGS, {}};
        }

        auto res = std::make_unique<OP>(std::move(ptr), std::move(ptrNum));
        return {ParseOperationRes::VALID, std::move(res)};
    }
    return {ParseOperationRes::INVALID_ARGS, {}};
}

template<typename DB>
std::pair<ParseOperationRes, std::unique_ptr<DBopsNodeBase> > ParseObjectGet(
    view_t &view, DB &db, object_reference &reference, std::ostream &outstream) {
    if (view.size() != 2) {
        auto ptr = std::make_unique<ErrorNode>(outstream, InvalidArgsCount(2, view.size()));
        return {ParseOperationRes::INVALID_ARG_C, std::move(ptr)};
    }
    auto parse = ParseArg(view);
    auto [type, ptr] = getObjectFromDB(db, std::move(parse));
    auto parseField = ParseArg(view);
    auto [typeField, ptrField] = getString(db, std::move(parseField));
    const auto fieldName = static_cast<std::string>(ptrField.get()->Execute());

    auto findRes = reference.find(fieldName);

    if (findRes == reference.end()) {
        auto errorPtr = std::make_unique<ErrorNode>(outstream, InvalidField(fieldName));
        return {ParseOperationRes::INVALID_ARGS, std::move(errorPtr)};
    }

    if (findRes->second == ObjectFieldTypes::INTEGER) {
        auto ptrEval = std::make_unique<ObjectGetNode<Integer> >(std::move(ptr), fieldName);
        auto ptrOp = std::make_unique<ToStreamNode<Object, Integer> >(
            outstream, std::move(ptrEval), db);
        return {ParseOperationRes::VALID, std::move(ptrOp)};
    }


    if (findRes->second == ObjectFieldTypes::FLOAT) {
        auto ptrEval = std::make_unique<ObjectGetNode<Float> >(std::move(ptr), fieldName);
        auto ptrOp = std::make_unique<ToStreamNode<Object, Float> >(
            outstream, std::move(ptrEval), db);
        return {ParseOperationRes::VALID, std::move(ptrOp)};
    }

    if (findRes->second == ObjectFieldTypes::STRING) {
        auto ptrEval = std::make_unique<ObjectGetNode<String> >(std::move(ptr), fieldName);
        auto ptrOp = std::make_unique<ToStreamNode<Object, String> >(
            outstream, std::move(ptrEval), db);
        return {ParseOperationRes::VALID, std::move(ptrOp)};
    }

    return {ParseOperationRes::INVALID_ARGS, {}};
}

template<typename DB>
std::tuple<ParseOperationRes, NodePtr<Object>, ObjectFieldTypes, std::string> ParseObjectSet(
    view_t &view, DB &db, object_reference &reference) {
    if (view.size() != 3) {
        std::unique_ptr<Node<Object> > ptr;
        return {ParseOperationRes::INVALID_ARG_C, std::move(ptr), ObjectFieldTypes::INVALID_FIELD, std::string()};
    }
    auto parse = ParseArg(view);
    auto [type, ptr] = getObjectFromDB(db, std::move(parse));
    auto parseField = ParseArg(view);
    auto [typeField, ptrField] = getString(db, std::move(parseField));
    const auto fieldName = static_cast<std::string>(ptrField.get()->Execute());

    auto findRes = reference.find(fieldName);

    if (findRes == reference.end()) {
        return {ParseOperationRes::INVALID_ARGS, std::move(ptr), ObjectFieldTypes::INVALID_FIELD, std::string()};
    }

    return {ParseOperationRes::VALID, std::move(ptr), findRes->second, fieldName};
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
        return {ParseOperationRes::INVALID_ARG_C, {}};
    }

    auto [type, val] = ParseKey(view);

    if (type == KeyType::INVALID) {
        return {ParseOperationRes::INVALID_ARGS, {}};
    }

    std::unique_ptr<DBopsNode<T> > res;
    auto resMsg = ParseOperationRes::VALID;
    if (type == KeyType::CONSTANT) {
        std::vector<uint8_t> bytes = stringBytes(std::move(val));
        res = std::make_unique<N>(db, std::move(bytes), stream);
    } else if (type == KeyType::FILE_PATH) {
        if (auto [msg, bytes] = readFileBytes(val); msg == BytesFromFileRes::COULD_NOT_OPEN) {
            resMsg = ParseOperationRes::COULD_NOT_OPEN_FILE;
        } else if (msg == BytesFromFileRes::COULD_NOT_READ) {
            resMsg = ParseOperationRes::COULD_NOT_READ_FILE;
        } else {
            res = std::make_unique<N>(db, std::move(bytes), stream);
        }
    }
    return {resMsg, std::move(res)};
}

template<typename T>
parse_op_r<DBopsNode<T> > ParseDelete(view_t &view, DBType<T> &db) {
    if (view.size() != 1) {
        return {ParseOperationRes::INVALID_ARG_C, {}};
    }

    auto [type, val] = ParseKey(view);

    if (type == KeyType::INVALID) {
        return {ParseOperationRes::INVALID_ARGS, {}};
    }

    std::unique_ptr<DBopsNode<T> > res;
    auto resMsg = ParseOperationRes::VALID;
    if (type == KeyType::CONSTANT) {
        std::vector<uint8_t> bytes = stringBytes(std::move(val));
        res = std::make_unique<DeleteNode<T> >(db, std::move(bytes));
    } else if (type == KeyType::FILE_PATH) {
        if (auto [msg, bytes] = readFileBytes(val); msg == BytesFromFileRes::COULD_NOT_OPEN) {
            resMsg = ParseOperationRes::COULD_NOT_OPEN_FILE;
        } else if (msg == BytesFromFileRes::COULD_NOT_READ) {
            resMsg = ParseOperationRes::COULD_NOT_READ_FILE;
        } else {
            res = std::make_unique<DeleteNode<T> >(db, std::move(bytes));
        }
    }
    return {resMsg, std::move(res)};
}

template<typename T>
parse_op_r<DBopsNode<T> > ParseInsertWithParser(view_t &view, DBType<T> &db,
                                                std::function<std::pair<GetArgsReturnTypes, NodePtr<T> >(
                                                    DBType<T> &, args_parsed &&)> parser, ttl_t ttl,
                                                ttlQueueType &queue) {
    if (view.size() != 2) {
        return {ParseOperationRes::INVALID_ARG_C, {}};
    }

    auto [keyType, key] = ParseKey(view);

    if (keyType == KeyType::INVALID) {
        return {ParseOperationRes::INVALID_ARGS, {}};
    }

    auto arg = ParseArg(view);
    auto [res, ptr] = parser(db, std::move(arg));

    if (res == GetArgsReturnTypes::INVALID) {
        return {ParseOperationRes::INVALID_ARGS, {}};
    }

    std::vector<uint8_t> keyBytes;

    if (keyType == KeyType::CONSTANT) {
        keyBytes = stringBytes(std::move(key));
    }

    if (keyType == KeyType::FILE_PATH) {
        auto [fileRead, value] = readFileBytes(key);
        if (fileRead == BytesFromFileRes::COULD_NOT_READ) {
            return {ParseOperationRes::COULD_NOT_READ_FILE, {}};
        }
        if (fileRead == BytesFromFileRes::COULD_NOT_OPEN) {
            return {ParseOperationRes::COULD_NOT_OPEN_FILE, {}};
        }
        keyBytes = std::move(value);
    }


    if (ttl.second == TTLTypes::IMMORTAL) {
        auto resNode = std::make_unique<InsertNode<T, ImmortalSentinel, ttlQueueType> >(
            db, std::move(keyBytes), std::move(ptr), std::move(ImmortalSentinel()), queue);
        return {ParseOperationRes::VALID, std::move(resNode)};
    }
    if (ttl.second == TTLTypes::MILLISECONDS) {
        auto resNode = std::make_unique<InsertNode<T, std::chrono::milliseconds, ttlQueueType> >(
            db, std::move(keyBytes), std::move(ptr), std::move(std::get<std::chrono::milliseconds>(ttl.first)), queue);
        return {ParseOperationRes::VALID, std::move(resNode)};
    }
    if (ttl.second == TTLTypes::SECONDS) {
        auto resNode = std::make_unique<InsertNode<T, std::chrono::seconds, ttlQueueType> >(
            db, std::move(keyBytes), std::move(ptr), std::move(std::get<std::chrono::seconds>(ttl.first)), queue);
        return {ParseOperationRes::VALID, std::move(resNode)};
    }
    if (ttl.second == TTLTypes::MINUTES) {
        auto resNode = std::make_unique<InsertNode<T, std::chrono::minutes, ttlQueueType> >(
            db, std::move(keyBytes), std::move(ptr), std::move(std::get<std::chrono::minutes>(ttl.first)), queue);
        return {ParseOperationRes::VALID, std::move(resNode)};
    }

    return {ParseOperationRes::INVALID_ARGS, {}};
}

template<typename T>
parse_op_r<DBopsNode<T> > ParseInsert(view_t &view, DBType<T> &db, ttl_t ttl,
                                      ttlQueueType &queue) {
    if constexpr (get_db_type<T>::value == DBTypes::FLOAT) {
        return ParseInsertWithParser<T>(view, db, getFloat<DBType<T> >, ttl, queue);
    }
    if constexpr (get_db_type<T>::value == DBTypes::INTEGER) {
        return ParseInsertWithParser<T>(view, db, getInteger<DBType<T> >, ttl, queue);
    }
    if constexpr (get_db_type<T>::value == DBTypes::STRING) {
        return ParseInsertWithParser<T>(view, db, getString<DBType<T> >, ttl, queue);
    }
    if constexpr (get_db_type<T>::value == DBTypes::VECTOR_FLOAT) {
        return ParseInsertWithParser<T>(view, db, [](DBType<T> &dbRef, args_parsed &&args) {
            return getVector<T, DBType<T> >(dbRef, std::move(args), ParseFloat);
        }, ttl, queue);
    }
    if constexpr (get_db_type<T>::value == DBTypes::VECTOR_INT) {
        return ParseInsertWithParser<T>(view, db, [](DBType<T> &dbRef, args_parsed &&args) {
            return getVector<T, DBType<T> >(dbRef, std::move(args), ParseInt);
        }, ttl, queue);
    }
    if constexpr (get_db_type<T>::value == DBTypes::VECTOR_STRING) {
        return ParseInsertWithParser<T>(view, db, [](DBType<T> &dbRef, args_parsed &&args) {
            return getVector<T, DBType<T> >(dbRef, std::move(args), ParseString);
        }, ttl, queue);
    } else {
        return {ParseOperationRes::INVALID_ARGS, {}};
    }
}

parse_op_r<DBopsNode<Object> > ParseInsert(view_t &view, DBType<Object> &db, ttl_t ttl, ttlQueueType &queue,
                                           const object_reference &reference, const Object &referenceObject);

std::unique_ptr<ErrorNode> GetError(ParseOperationRes res, std::size_t expected, std::size_t actual,
                                    std::ostream &outstream, const std::string &file);

void readThread(std::stop_token stoken, std::istream &stream, std::mutex &queueMutex,
                std::queue<std::string> &dataQueue);

bool HasTarget(OPType op);


template<typename T>
void RunDB(ttl_t &&defaultTTL, std::istream &instream, std::ostream &outstream, Object &referenceObject,
           object_reference &reference) {
    constexpr DBTypes dbType = get_db_type<T>::value;
    const auto seed = static_cast<uint64_t>(std::time(nullptr));
    auto [k1,k2] = sampleKey(seed);
    SwissTable<SipHash, DBEntry<T>, std::vector<uint8_t> > db(SipHash(k1, k2, 2, 4));
    ttlQueueType ttlQueue;
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
            } else if (ttlQueue.top()->Expired()) {
                auto key = ttlQueue.top()->key;
                auto entry = db.LookUp(std::move(key));
                entry.DeleteEntry();
                ttlQueue.pop();
            }
        }

        if (!dataQueue.empty()) {
            std::lock_guard lock(queueMutex);
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

            TargetType targetType;
            std::string targetValue;
            if (type != OPType::GET && type != OPType::DELETE && type != OPType::INSERT) {
                auto [targetTypeP, targetValueP] = ParseTarget(view);
                targetType = targetTypeP;
                targetValue = std::move(targetValueP);

                if (targetTypeP == TargetType::INVALID) {
                    outstream << "Invalid target" << std::endl;
                    continue;
                }
            }


            std::unique_ptr<Node<T> > toExecute;
            auto resMsg = ParseOperationRes::VALID;


            if constexpr (get_db_type<T>::value == DBTypes::INTEGER) {
                if (type == OPType::GET) {
                    auto [msg, ptr] = ParseOneKeyOP<T, GetNode<T> >(view, outstream, db);
                    if (msg == ParseOperationRes::VALID) {
                        ptr.get()->Execute();
                        continue;
                    }
                    resMsg = msg;
                } else if (type == OPType::DELETE) {
                    auto [msg, ptr] = ParseDelete<T>(view, db);
                    if (msg == ParseOperationRes::VALID) {
                        ptr.get()->Execute();
                        continue;
                    }
                    resMsg = msg;
                } else if (type == OPType::INSERT) {
                    auto [msg, ptr] = ParseInsert<T>(view, db, ttl, ttlQueue);
                    if (msg == ParseOperationRes::VALID) {
                        ptr.get()->Execute();
                        continue;
                    }
                    resMsg = msg;
                } else if (type == OPType::ADD) {
                    auto [msg, ptr] = ParseIntNode<DBType<T>, AddOp<Integer> >(view, db);
                    if (msg == ParseOperationRes::VALID) {
                        toExecute = std::move(ptr);
                    } else {
                        resMsg = msg;
                    }
                } else if (type == OPType::SUBTRACT) {
                    auto [msg, ptr] = ParseIntNode<DBType<T>, SubtractOp<Integer> >(view, db);
                    if (msg == ParseOperationRes::VALID) {
                        toExecute = std::move(ptr);
                    } else {
                        resMsg = msg;
                    }
                } else if (type == OPType::MULTIPLY) {
                    auto [msg, ptr] = ParseIntNode<DBType<T>, MultiplyOp<Integer> >(view, db);
                    if (msg == ParseOperationRes::VALID) {
                        toExecute = std::move(ptr);
                    } else {
                        resMsg = msg;
                    }
                } else if (type == OPType::DIVIDE) {
                    auto [msg, ptr] = ParseIntNode<DBType<T>, DivideOp<Integer> >(view, db);
                    if (msg == ParseOperationRes::VALID) {
                        toExecute = std::move(ptr);
                    } else {
                        resMsg = msg;
                    }
                } else {
                    resMsg = ParseOperationRes::INVALID_ARGS;
                }
            }

            if constexpr (get_db_type<T>::value == DBTypes::FLOAT) {
                if (type == OPType::GET) {
                    auto [msg, ptr] = ParseOneKeyOP<T, GetNode<T> >(view, outstream, db);
                    if (msg == ParseOperationRes::VALID) {
                        ptr.get()->Execute();
                        continue;
                    }
                    resMsg = msg;
                } else if (type == OPType::DELETE) {
                    auto [msg, ptr] = ParseDelete<T>(view, db);
                    if (msg == ParseOperationRes::VALID) {
                        ptr.get()->Execute();
                        continue;
                    }
                    resMsg = msg;
                } else if (type == OPType::INSERT) {
                    auto [msg, ptr] = ParseInsert<T>(view, db, ttl, ttlQueue);
                    if (msg == ParseOperationRes::VALID) {
                        ptr.get()->Execute();
                        continue;
                    }
                } else if (type == OPType::FADD) {
                    auto [msg, ptr] = ParseFloatNode<DBType<T>, AddOp<Float> >(view, db);
                    if (msg == ParseOperationRes::VALID) {
                        toExecute = std::move(ptr);
                    } else {
                        resMsg = msg;
                    }
                } else if (type == OPType::FSUBTRACT) {
                    auto [msg, ptr] = ParseFloatNode<DBType<T>, SubtractOp<Float> >(view, db);
                    if (msg == ParseOperationRes::VALID) {
                        toExecute = std::move(ptr);
                    } else {
                        resMsg = msg;
                    }
                } else if (type == OPType::FMULTIPLY) {
                    auto [msg, ptr] = ParseFloatNode<DBType<T>, MultiplyOp<Float> >(view, db);
                    if (msg == ParseOperationRes::VALID) {
                        toExecute = std::move(ptr);
                    } else {
                        resMsg = msg;
                    }
                } else if (type == OPType::FDIVIDE) {
                    auto [msg, ptr] = ParseFloatNode<DBType<T>, DivideOp<Float> >(view, db);
                    if (msg == ParseOperationRes::VALID) {
                        toExecute = std::move(ptr);
                    } else {
                        resMsg = msg;
                    }
                } else {
                    resMsg = ParseOperationRes::INVALID_ARGS;
                }
            }

            if constexpr (get_db_type<T>::value == DBTypes::STRING) {
                if (type == OPType::GET) {
                    auto [msg, ptr] = ParseOneKeyOP<T, GetNode<T> >(view, outstream, db);
                    if (msg == ParseOperationRes::VALID) {
                        ptr.get()->Execute();
                        continue;
                    }
                    resMsg = msg;
                } else if (type == OPType::DELETE) {
                    auto [msg, ptr] = ParseDelete<T>(view, db);
                    if (msg == ParseOperationRes::VALID) {
                        ptr.get()->Execute();
                        continue;
                    }
                    resMsg = msg;
                } else if (type == OPType::INSERT) {
                    auto [msg, ptr] = ParseInsert<T>(view, db, ttl, ttlQueue);
                    if (msg == ParseOperationRes::VALID) {
                        ptr.get()->Execute();
                        continue;
                    }
                } else if (type == OPType::CONCAT) {
                    auto [msg, ptr] = ParseStringOpStringString<DBType<T>, ConcatStrings>(view, db);
                    if (msg == ParseOperationRes::VALID) {
                        toExecute = std::move(ptr);
                    } else {
                        resMsg = msg;
                    }
                } else if (type == OPType::SUFFIX) {
                    auto [msg, ptr] = ParseStringOpStringNum<DBType<T>, SuffixNode>(view, db);
                    if (msg == ParseOperationRes::VALID) {
                        toExecute = std::move(ptr);
                    } else {
                        resMsg = msg;
                    }
                } else if (type == OPType::PREFIX) {
                    auto [msg, ptr] = ParseStringOpStringNum<DBType<T>, PrefixNode>(view, db);
                    if (msg == ParseOperationRes::VALID) {
                        toExecute = std::move(ptr);
                    } else {
                        resMsg = msg;
                    }
                } else if (type == OPType::AT) {
                    auto [msg, ptr] = ParseStringOpStringNum<DBType<T>, AtNode>(view, db);
                    if (msg == ParseOperationRes::VALID) {
                        toExecute = std::move(ptr);
                    } else {
                        resMsg = msg;
                    }
                } else if (type == OPType::FIND) {
                    if (TargetType::KEY == targetType) {
                        outstream << "Invalid target" << std::endl;
                        continue;
                    }
                    auto [msg, ptr] = ParseStringOpStringString<DBType<T>, FindNode>(view, db);
                    if (std::ofstream outFile(targetValue); !outFile.is_open()) {
                        resMsg = ParseOperationRes::COULD_NOT_OPEN_FILE;
                    } else if (msg == ParseOperationRes::VALID) {
                        std::unique_ptr<ToStreamNode<T, Vector<Integer> > > toExec = std::make_unique<ToStreamNode<T,
                            Vector<Integer> > >(outFile, std::move(ptr), db);
                        toExec->Execute();
                        continue;
                    } else {
                        resMsg = msg;
                    }
                } else {
                    resMsg = ParseOperationRes::INVALID_ARGS;
                }
            }

            if constexpr (get_db_type<T>::value == DBTypes::VECTOR_FLOAT) {
                if (type == OPType::GET) {
                    auto [msg, ptr] = ParseOneKeyOP<T, GetNode<T> >(view, outstream, db);
                    if (msg == ParseOperationRes::VALID) {
                        ptr.get()->Execute();
                        continue;
                    }
                    resMsg = msg;
                } else if (type == OPType::DELETE) {
                    auto [msg, ptr] = ParseDelete<T>(view, db);
                    if (msg == ParseOperationRes::VALID) {
                        ptr.get()->Execute();
                        continue;
                    }
                    resMsg = msg;
                } else if (type == OPType::INSERT) {
                    auto [msg, ptr] = ParseInsert<T>(view, db, ttl, ttlQueue);
                    if (msg == ParseOperationRes::VALID) {
                        ptr.get()->Execute();
                        continue;
                    }
                    resMsg = msg;
                } else if (type == OPType::APPEND) {
                    auto [msg, ptr] = ParseVectOpVectVal<DBType<T>, AppendNode<Float>, Vector<Float> >(view, db);
                    if (msg == ParseOperationRes::VALID) {
                        toExecute = std::move(ptr);
                    } else {
                        resMsg = msg;
                    }
                } else if (type == OPType::POP) {
                    auto [msg, ptr] = ParseVectOpVect<DBType<T>, PopNode<Float>, Vector<Float> >(view, db);
                    if (msg == ParseOperationRes::VALID) {
                        toExecute = std::move(ptr);
                    } else {
                        resMsg = msg;
                    }
                } else if (type == OPType::VINSERT) {
                    auto [msg, ptr] = ParseVectOpVectNumVal<DBType<T>, VInsertNode<Float>, Vector<Float> >(view, db);
                    if (msg == ParseOperationRes::VALID) {
                        toExecute = std::move(ptr);
                    } else {
                        resMsg = msg;
                    }
                } else if (type == OPType::REMOVE) {
                    auto [msg, ptr] = ParseVectOpVectNum<DBType<T>, RemoveNode<Float>, Vector<Float> >(view, db);
                    if (msg == ParseOperationRes::VALID) {
                        toExecute = std::move(ptr);
                    } else {
                        resMsg = msg;
                    }
                } else if (type == OPType::SET) {
                    auto [msg, ptr] = ParseVectOpVectNumVal<DBType<T>, SetNode<Float>, Vector<Float> >(view, db);
                    if (msg == ParseOperationRes::VALID) {
                        toExecute = std::move(ptr);
                    } else {
                        resMsg = msg;
                    }
                } else if (type == OPType::VGET) {
                    std::unique_ptr<Node<Float> > resGet;
                    auto msgGet = ParseOperationRes::INVALID_ARGS;
                    auto [msg, ptr] = ParseVectOpVectNum<DBType<T>, VgetNode<Float>, Vector<Float> >(view, db);
                    if (msg == ParseOperationRes::VALID) {
                        resGet = std::move(ptr);
                        msgGet = msg;
                    } else {
                        resMsg = msg;
                    }

                    if (TargetType::KEY == targetType) {
                        outstream << "Invalid target" << std::endl;
                        continue;
                    }
                    if (std::ofstream outFile(targetValue); !outFile.is_open()) {
                        resMsg = ParseOperationRes::COULD_NOT_OPEN_FILE;
                    } else if (msgGet == ParseOperationRes::VALID) {
                        std::unique_ptr<ToStreamNode<T, Float> > toExec =
                                std::make_unique<ToStreamNode<T, Float> >(outFile, std::move(resGet), db);
                        toExec->Execute();
                        continue;
                    }
                } else if (type == OPType::SIZE) {
                    std::unique_ptr<Node<Integer> > resSize;
                    auto msgSize = ParseOperationRes::INVALID_ARGS;
                    auto [msg, ptr] = ParseVectOpVect<DBType<T>, SizeNode<Float>, Vector<Float> >(view, db);
                    if (msg == ParseOperationRes::VALID) {
                        resSize = std::move(ptr);
                        msgSize = msg;
                    } else {
                        resMsg = msg;
                    }

                    if (TargetType::KEY == targetType) {
                        outstream << "Invalid target" << std::endl;
                        continue;
                    }
                    if (std::ofstream outFile(targetValue); !outFile.is_open()) {
                        resMsg = ParseOperationRes::COULD_NOT_OPEN_FILE;
                    } else if (msgSize == ParseOperationRes::VALID) {
                        std::unique_ptr<ToStreamNode<T, Integer> > toExec =
                                std::make_unique<ToStreamNode<T, Integer> >(outFile, std::move(resSize), db);
                        toExec->Execute();
                        continue;
                    }
                } else {
                    resMsg = ParseOperationRes::INVALID_ARGS;
                }
            } else if constexpr (get_db_type<T>::value == DBTypes::VECTOR_INT) {
                if (type == OPType::GET) {
                    auto [msg, ptr] = ParseOneKeyOP<T, GetNode<T> >(view, outstream, db);
                    if (msg == ParseOperationRes::VALID) {
                        ptr.get()->Execute();
                        continue;
                    }
                    resMsg = msg;
                } else if (type == OPType::DELETE) {
                    auto [msg, ptr] = ParseDelete<T>(view, db);
                    if (msg == ParseOperationRes::VALID) {
                        ptr.get()->Execute();
                        continue;
                    }
                    resMsg = msg;
                } else if (type == OPType::INSERT) {
                    auto [msg, ptr] = ParseInsert<T>(view, db, ttl, ttlQueue);
                    if (msg == ParseOperationRes::VALID) {
                        ptr.get()->Execute();
                        continue;
                    }
                    resMsg = msg;
                } else if (type == OPType::APPEND) {
                    auto [msg, ptr] = ParseVectOpVectVal<DBType<T>, AppendNode<Integer>, Vector<Integer> >(view, db);
                    if (msg == ParseOperationRes::VALID) {
                        toExecute = std::move(ptr);
                    } else {
                        resMsg = msg;
                    }
                } else if (type == OPType::POP) {
                    auto [msg, ptr] = ParseVectOpVect<DBType<T>, PopNode<Integer>, Vector<Integer> >(view, db);
                    if (msg == ParseOperationRes::VALID) {
                        toExecute = std::move(ptr);
                    } else {
                        resMsg = msg;
                    }
                } else if (type == OPType::VINSERT) {
                    auto [msg, ptr] = ParseVectOpVectNumVal<DBType<T>, VInsertNode<Integer>, Vector<
                        Integer> >(view, db);
                    if (msg == ParseOperationRes::VALID) {
                        toExecute = std::move(ptr);
                    } else {
                        resMsg = msg;
                    }
                } else if (type == OPType::REMOVE) {
                    auto [msg, ptr] = ParseVectOpVectNum<DBType<T>, RemoveNode<Integer>, Vector<Integer> >(view, db);
                    if (msg == ParseOperationRes::VALID) {
                        toExecute = std::move(ptr);
                    } else {
                        resMsg = msg;
                    }
                } else if (type == OPType::SET) {
                    auto [msg, ptr] = ParseVectOpVectNumVal<DBType<T>, SetNode<Integer>, Vector<Integer> >(view, db);
                    if (msg == ParseOperationRes::VALID) {
                        toExecute = std::move(ptr);
                    } else {
                        resMsg = msg;
                    }
                } else if (type == OPType::VGET) {
                    std::unique_ptr<Node<Integer> > resGet;
                    auto msgGet = ParseOperationRes::INVALID_ARGS;
                    auto [msg, ptr] = ParseVectOpVectNum<DBType<T>, VgetNode<Integer>, Vector<Integer> >(view, db);
                    if (msg == ParseOperationRes::VALID) {
                        resGet = std::move(ptr);
                        msgGet = msg;
                    } else {
                        resMsg = msg;
                    }

                    if (TargetType::KEY == targetType) {
                        outstream << "Invalid target" << std::endl;
                        continue;
                    }
                    if (std::ofstream outFile(targetValue); !outFile.is_open()) {
                        resMsg = ParseOperationRes::COULD_NOT_OPEN_FILE;
                    } else if (msgGet == ParseOperationRes::VALID) {
                        std::unique_ptr<ToStreamNode<T, Integer> > toExec =
                                std::make_unique<ToStreamNode<T, Integer> >(outFile, std::move(resGet), db);
                        toExec->Execute();
                        continue;
                    }
                } else if (type == OPType::SIZE) {
                    std::unique_ptr<Node<Integer> > resSize;
                    auto msgSize = ParseOperationRes::INVALID_ARGS;
                    auto [msg, ptr] = ParseVectOpVect<DBType<T>, SizeNode<Integer>, Vector<Integer> >(view, db);
                    if (msg == ParseOperationRes::VALID) {
                        resSize = std::move(ptr);
                        msgSize = msg;
                    } else {
                        resMsg = msg;
                    }

                    if (TargetType::KEY == targetType) {
                        outstream << "Invalid target" << std::endl;
                        continue;
                    }
                    if (std::ofstream outFile(targetValue); !outFile.is_open()) {
                        resMsg = ParseOperationRes::COULD_NOT_OPEN_FILE;
                    } else if (msgSize == ParseOperationRes::VALID) {
                        std::unique_ptr<ToStreamNode<T, Integer> > toExec =
                                std::make_unique<ToStreamNode<T, Integer> >(outFile, std::move(resSize), db);
                        toExec->Execute();
                        continue;
                    }
                } else {
                    resMsg = ParseOperationRes::INVALID_ARGS;
                }
            } else if constexpr (get_db_type<T>::value == DBTypes::VECTOR_STRING) {
                if (type == OPType::GET) {
                    auto [msg, ptr] = ParseOneKeyOP<T, GetNode<T> >(view, outstream, db);
                    if (msg == ParseOperationRes::VALID) {
                        ptr.get()->Execute();
                        continue;
                    }
                    resMsg = msg;
                } else if (type == OPType::DELETE) {
                    auto [msg, ptr] = ParseDelete<T>(view, db);
                    if (msg == ParseOperationRes::VALID) {
                        ptr.get()->Execute();
                        continue;
                    }
                    resMsg = msg;
                } else if (type == OPType::INSERT) {
                    auto [msg, ptr] = ParseInsert<T>(view, db, ttl, ttlQueue);
                    if (msg == ParseOperationRes::VALID) {
                        ptr.get()->Execute();
                        continue;
                    }
                    resMsg = msg;
                } else if (type == OPType::APPEND) {
                    auto [msg, ptr] = ParseVectOpVectVal<DBType<T>, AppendNode<String>, Vector<String> >(view, db);
                    if (msg == ParseOperationRes::VALID) {
                        toExecute = std::move(ptr);
                    } else {
                        resMsg = msg;
                    }
                } else if (type == OPType::POP) {
                    auto [msg, ptr] = ParseVectOpVect<DBType<T>, PopNode<String>, Vector<String> >(view, db);
                    if (msg == ParseOperationRes::VALID) {
                        toExecute = std::move(ptr);
                    } else {
                        resMsg = msg;
                    }
                } else if (type == OPType::VINSERT) {
                    auto [msg, ptr] = ParseVectOpVectNumVal<DBType<T>, VInsertNode<String>, Vector<String> >(view, db);
                    if (msg == ParseOperationRes::VALID) {
                        toExecute = std::move(ptr);
                    } else {
                        resMsg = msg;
                    }
                } else if (type == OPType::REMOVE) {
                    auto [msg, ptr] = ParseVectOpVectNum<DBType<T>, RemoveNode<String>, Vector<String> >(view, db);
                    if (msg == ParseOperationRes::VALID) {
                        toExecute = std::move(ptr);
                    } else {
                        resMsg = msg;
                    }
                } else if (type == OPType::SET) {
                    auto [msg, ptr] = ParseVectOpVectNumVal<DBType<T>, SetNode<String>, Vector<String> >(view, db);
                    if (msg == ParseOperationRes::VALID) {
                        toExecute = std::move(ptr);
                    } else {
                        resMsg = msg;
                    }
                } else if (type == OPType::VGET) {
                    std::unique_ptr<Node<String> > resGet;
                    auto msgGet = ParseOperationRes::INVALID_ARGS;
                    auto [msg, ptr] = ParseVectOpVectNum<DBType<T>, VgetNode<String>, Vector<String> >(view, db);
                    if (msg == ParseOperationRes::VALID) {
                        resGet = std::move(ptr);
                        msgGet = msg;
                    } else {
                        resMsg = msg;
                    }

                    if (TargetType::KEY == targetType) {
                        outstream << "Invalid target" << std::endl;
                        continue;
                    }
                    if (std::ofstream outFile(targetValue); !outFile.is_open()) {
                        resMsg = ParseOperationRes::COULD_NOT_OPEN_FILE;
                    } else if (msgGet == ParseOperationRes::VALID) {
                        std::unique_ptr<ToStreamNode<T, String> > toExec =
                                std::make_unique<ToStreamNode<T, String> >(outFile, std::move(resGet), db);
                        toExec->Execute();
                        continue;
                    }
                } else if (type == OPType::SIZE) {
                    std::unique_ptr<Node<Integer> > resSize;
                    auto msgSize = ParseOperationRes::INVALID_ARGS;
                    auto [msg, ptr] = ParseVectOpVect<DBType<T>, SizeNode<String>, Vector<String> >(view, db);
                    if (msg == ParseOperationRes::VALID) {
                        resSize = std::move(ptr);
                        msgSize = msg;
                    } else {
                        resMsg = msg;
                    }

                    if (TargetType::KEY == targetType) {
                        outstream << "Invalid target" << std::endl;
                        continue;
                    }
                    if (std::ofstream outFile(targetValue); !outFile.is_open()) {
                        resMsg = ParseOperationRes::COULD_NOT_OPEN_FILE;
                    } else if (msgSize == ParseOperationRes::VALID) {
                        std::unique_ptr<ToStreamNode<T, Integer> > toExec =
                                std::make_unique<ToStreamNode<T, Integer> >(outFile, std::move(resSize), db);
                        toExec->Execute();
                        continue;
                    }
                } else {
                    resMsg = ParseOperationRes::INVALID_ARGS;
                }
            } else if constexpr (get_db_type<T>::value == DBTypes::OBJECT) {
                if (type == OPType::GET) {
                    auto [msg, ptr] = ParseOneKeyOP<T, GetNode<T> >(view, outstream, db);
                    if (msg == ParseOperationRes::VALID) {
                        ptr.get()->Execute();
                        continue;
                    }
                    resMsg = msg;
                } else if (type == OPType::DELETE) {
                    auto [msg, ptr] = ParseDelete<T>(view, db);
                    if (msg == ParseOperationRes::VALID) {
                        ptr.get()->Execute();
                        continue;
                    }
                    resMsg = msg;
                } else if (type == OPType::INSERT) {
                    auto [msg, ptr] = ParseInsert(view, db, ttl, ttlQueue, reference, referenceObject);
                    if (msg == ParseOperationRes::VALID) {
                        ptr.get()->Execute();
                        continue;
                    }
                    resMsg = msg;
                } else if (type == OPType::OGET) {
                    if (targetType == TargetType::KEY || targetType == TargetType::INVALID) {
                        resMsg = ParseOperationRes::INVALID_ARGS;
                    } else if (std::ofstream outFile(targetValue); !outFile.is_open()) {
                        resMsg = ParseOperationRes::COULD_NOT_OPEN_FILE;
                    } else {
                        auto [msg, ptr] = ParseObjectGet<DBType<T> >(view, db, reference, outFile);
                        if (msg == ParseOperationRes::VALID) {
                            ptr.get()->Execute();
                            continue;
                        }
                        resMsg = msg;
                    }
                } else if (type == OPType::OSET) {
                    auto [resMsgParse, ptrObject, fieldT, fieldN] = ParseObjectSet(view, db, reference);
                    auto parseValue = ParseArg(view);
                    std::unique_ptr<DBopsNodeBase> ptrOp;

                    if (resMsgParse != ParseOperationRes::VALID) {
                        resMsg = resMsgParse;
                    } else if (fieldT == ObjectFieldTypes::INTEGER) {
                        auto [valMsg, valPtr] = getInteger(db, std::move(parseValue));

                        if (valMsg != GetArgsReturnTypes::VALID) {
                            resMsg = ParseOperationRes::INVALID_ARGS;
                        } else if (targetType == TargetType::KEY) {
                            auto keyBytes = stringBytes(std::move(targetValue));
                            auto ptrEval = std::make_unique<ObjectSetNode<Integer> >(
                                std::move(ptrObject), fieldN, std::move(valPtr));
                            if (ttl.second == TTLTypes::IMMORTAL) {
                                ptrOp = std::make_unique<InsertNode<T, ImmortalSentinel, ttlQueueType> >(
                                    db, std::move(keyBytes), std::move(ptrEval), std::move(ImmortalSentinel()),
                                    ttlQueue);
                                ptrOp.get()->Execute();
                                continue;
                            }
                            if (ttl.second == TTLTypes::MILLISECONDS) {
                                ptrOp = std::make_unique<InsertNode<T, std::chrono::milliseconds, ttlQueueType> >(
                                    db, std::move(keyBytes), std::move(ptrEval),
                                    std::move(std::get<std::chrono::milliseconds>(ttl.first)), ttlQueue);
                                ptrOp.get()->Execute();
                                continue;
                            }
                            if (ttl.second == TTLTypes::SECONDS) {
                                ptrOp = std::make_unique<InsertNode<T, std::chrono::seconds, ttlQueueType> >(
                                    db, std::move(keyBytes), std::move(ptrEval),
                                    std::move(std::get<std::chrono::seconds>(ttl.first)), ttlQueue);
                                ptrOp.get()->Execute();
                                continue;
                            }
                            if (ttl.second == TTLTypes::MINUTES) {
                                ptrOp = std::make_unique<InsertNode<T, std::chrono::minutes, ttlQueueType> >(
                                    db, std::move(keyBytes), std::move(ptrEval),
                                    std::move(std::get<std::chrono::minutes>(ttl.first)), ttlQueue);
                                ptrOp.get()->Execute();
                                continue;
                            }
                        } else if (targetType == TargetType::FILE) {
                            if (std::ofstream outFile(targetValue); !outFile.is_open()) {
                                resMsg = ParseOperationRes::COULD_NOT_OPEN_FILE;
                            } else {
                                auto ptrEval = std::make_unique<ObjectSetNode<Integer> >(
                                    std::move(ptrObject), fieldN, std::move(valPtr));

                                ptrOp = std::make_unique<ToStreamNode<T, Object> >(
                                    outFile, std::move(ptrEval), db);

                                ptrOp.get()->Execute();
                                continue;
                            }
                        }
                    }
                } else {
                    resMsg = ParseOperationRes::INVALID_ARGS;
                }
            }


            std::unique_ptr<DBopsNodeBase> opNode;

            if (resMsg != ParseOperationRes::VALID) {
                opNode = GetError(resMsg, GetArgCount(type), view.size(), outstream, "");
            } else if (ValidateOPType(type, dbType) && targetType == TargetType::KEY) {
                auto key = stringBytes(std::move(targetValue));
                if (ttl.second == TTLTypes::MILLISECONDS) {
                    opNode = std::make_unique<InsertNode<T, std::chrono::milliseconds, ttlQueueType> >(
                        db, std::move(key), std::move(toExecute),
                        std::get<std::chrono::milliseconds>(std::move(ttl.first)), ttlQueue);
                } else if (ttl.second == TTLTypes::SECONDS) {
                    opNode = std::make_unique<InsertNode<T, std::chrono::seconds, ttlQueueType> >(
                        db, std::move(key), std::move(toExecute), std::get<std::chrono::seconds>(std::move(ttl.first)),
                        ttlQueue);
                } else if (ttl.second == TTLTypes::MINUTES) {
                    opNode = std::make_unique<InsertNode<T, std::chrono::minutes, ttlQueueType> >(
                        db, std::move(key), std::move(toExecute), std::get<std::chrono::minutes>(std::move(ttl.first)),
                        ttlQueue);
                } else {
                    opNode = std::make_unique<InsertNode<T, ImmortalSentinel, ttlQueueType> >(
                        db, std::move(key), std::move(toExecute), ImmortalSentinel(), ttlQueue);
                }
            } else if (targetType == TargetType::FILE) {
                if (std::ofstream outFile(targetValue); !outFile.is_open()) {
                    std::vector<std::string> err = {"Could not open file: ", targetValue};
                    opNode = std::make_unique<ErrorNode>(outstream, std::move(err));
                } else {
                    opNode = std::make_unique<ToStreamNode<T, T> >(outFile, std::move(toExecute), db);
                }
            } else {
                std::vector<std::string> err = {"Error"};
                opNode = std::make_unique<ErrorNode>(outstream, std::move(err));
            }
            opNode->Execute();
        }
    }
}
#endif //PROJECT_OPERATIONTYPE_HPP
