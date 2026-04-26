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

using read_from_file_r = std::pair<BytesFromFileRes, std::vector<uint8_t> >;

read_from_file_r readFileBytes(const std::string &filePath);

std::vector<uint8_t> stringBytes(std::string &&str);

using view_t = std::ranges::subrange<std::vector<std::string>::iterator>;

OPType GetOPType(view_t &args);

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
std::pair<GetArgsReturnTypes, StringPtr> getString(DBTypes dbType, DB &db, args_parsed &&args) {
    auto [type, val] = std::move(args);
    auto stringVal = String(std::move(val));
    if (type == ArgsType::CONSTANT) {
        auto res = std::make_unique<ConstantNode<String> >(std::move(stringVal));
        return {GetArgsReturnTypes::VALID, std::move(res)};
    }
    if (dbType != DBTypes::STRING) {
        return {GetArgsReturnTypes::INVALID, {}};
    }
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
    return {GetArgsReturnTypes::INVALID, {}};
}

parse_res<Integer> ParseInt(std::string_view input);

template<typename DB>
    requires (!std::is_reference_v<DB>)
std::pair<GetArgsReturnTypes, NumberPtr<Integer> > getInteger(DBTypes dbType, DB &db, args_parsed &&args) {
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
    if (dbType != DBTypes::INTEGER) {
        return {GetArgsReturnTypes::INVALID, {}};
    }
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
    if (dbType != DBTypes::FLOAT) {
        return {GetArgsReturnTypes::INVALID, {}};
    }
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
    DBTypes expectedDb = DBVectType<T>::type;
    if (dbType != expectedDb) {
        return {GetArgsReturnTypes::INVALID, {}};
    }

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
parse_op_r<N> ParseIntNode(view_t &view, DBTypes dbType, DB &db) {
    if (view.size() != 2) {
        return {ParseOperationRes::INVALID_ARG_C, {}};
    }
    auto parse1 = ParseArg(view);
    auto [type1, ptr1] = getInteger(dbType, db, std::move(parse1));
    auto parse2 = ParseArg(view);
    auto [type2, ptr2] = getInteger(dbType, db, std::move(parse2));


    if (GetArgsReturnTypes::INVALID == type1 || GetArgsReturnTypes::INVALID == type2) {
        return {ParseOperationRes::INVALID_ARGS, {}};
    }
    auto res = std::make_unique<N>(std::move(ptr1), std::move(ptr2));
    return {ParseOperationRes::VALID, std::move(res)};
}

template<typename DB, typename N>
parse_op_r<N> ParseFloatNode(view_t &view, DBTypes dbType, DB &db) {
    if (view.size() != 2) {
        return {ParseOperationRes::INVALID_ARG_C, {}};
    }
    auto parse1 = ParseArg(view);
    auto [type1, ptr1] = getFloat(dbType, db, std::move(parse1));
    auto parse2 = ParseArg(view);
    auto [type2, ptr2] = getFloat(dbType, db, std::move(parse2));


    if (GetArgsReturnTypes::INVALID == type1 || GetArgsReturnTypes::INVALID == type2) {
        return {ParseOperationRes::INVALID_ARGS, {}};
    }
    auto res = std::make_unique<N>(std::move(ptr1), std::move(ptr2));
    return {ParseOperationRes::VALID, std::move(res)};
}

template<typename DB, typename N>
parse_op_r<N> ParseStringOpStringNum(view_t &view, DBTypes dbType, DB &db) {
    if (view.size() != 2) {
        return {ParseOperationRes::INVALID_ARG_C, {}};
    }
    auto parse1 = ParseArg(view);
    auto [typeS, ptrS] = getString(dbType, db, std::move(parse1));
    auto parse2 = ParseArg(view);
    auto [typeI, ptrI] = getInteger(dbType, db, std::move(parse2));

    if (GetArgsReturnTypes::INVALID == typeS || GetArgsReturnTypes::INVALID == typeI) {
        return {ParseOperationRes::INVALID_ARGS, {}};
    }

    auto res = std::make_unique<N>(std::move(ptrS), std::move(ptrI));

    return {ParseOperationRes::VALID, std::move(res)};
}

template<typename DB, typename N>
parse_op_r<N> ParseStringOpStringString(view_t &view, DBTypes dbType, DB &db) {
    if (view.size() != 2) {
        return {ParseOperationRes::INVALID_ARG_C, {}};
    }
    auto parse1 = ParseArg(view);
    auto [type1, ptr1] = getString(dbType, db, std::move(parse1));
    auto parse2 = ParseArg(view);
    auto [type2, ptr2] = getString(dbType, db, std::move(parse2));

    if (GetArgsReturnTypes::INVALID == type1 || GetArgsReturnTypes::INVALID == type2) {
        return {ParseOperationRes::INVALID_ARGS, {}};
    }

    auto res = std::make_unique<N>(std::move(ptr1), std::move(ptr2));

    return {ParseOperationRes::VALID, std::move(res)};
}

template<typename DB, typename OP, typename T>
parse_op_r<OP> ParseVectOpVectVal(view_t &view, DBTypes dbType, DB &db,
                                  std::function<std::pair<bool, T>(std::string_view)>
                                  converter,
                                  std::function<std::pair<GetArgsReturnTypes, Node<T> >(DBTypes, DB &, args_parsed &&)>
                                  valParser) {
    if (view.size() != 2) {
        return {ParseOperationRes::INVALID_ARG_C, {}};
    }

    auto parse = ParseArg(view);
    auto [type, ptr] = getVector(dbType, db, std::move(parse), converter);
    auto parseVal = ParseArg(view);
    auto [typeVal, ptrVal] = valParser(dbType, db, std::move(parseVal));

    if (type == GetArgsReturnTypes::INVALID || typeVal == GetArgsReturnTypes::INVALID) {
        return {ParseOperationRes::INVALID_ARGS, {}};
    }

    auto res = std::make_unique<OP>(std::move(ptr), std::move(ptrVal));
    return {ParseOperationRes::VALID, std::move(res)};
}

template<typename DB, typename OP, typename T>
parse_op_r<OP> ParseVectOpVect(view_t &view, DBTypes dbType, DB &db,
                               std::function<std::pair<bool, T>(std::string_view)>
                               converter) {
    if (view.size() != 1) {
        return {ParseOperationRes::INVALID_ARG_C, {}};
    }

    auto parse = ParseArg(view);
    auto [type, ptr] = getVector(dbType, db, std::move(parse), converter);

    if (type == GetArgsReturnTypes::INVALID) {
        return {ParseOperationRes::INVALID_ARGS, {}};
    }

    auto res = std::make_unique<OP>(std::move(ptr));
    return {ParseOperationRes::VALID, std::move(res)};
}

template<typename DB, typename OP, typename T>
parse_op_r<OP> ParseVectOpVectNumVal(view_t &view, DBTypes dbType, DB &db,
                                     std::function<std::pair<bool, T>(std::string_view)>
                                     converter,
                                     std::function<std::pair<GetArgsReturnTypes, Node<T> >(
                                         DBTypes, DB &, args_parsed &&)>
                                     valParser) {
    if (view.size() != 3) {
        return {ParseOperationRes::INVALID_ARG_C, {}};
    }

    auto parse = ParseArg(view);
    auto [type, ptr] = getVector(dbType, db, std::move(parse), converter);
    auto parseNum = ParseArg(view);
    auto [typeNum, ptrNum] = getInteger(dbType, db, std::move(parseNum));
    auto parseVal = ParseArg(view);
    auto [typeVal, ptrVal] = valParser(dbType, db, std::move(parseVal));

    if (type == GetArgsReturnTypes::INVALID || typeVal == GetArgsReturnTypes::INVALID || typeNum ==
        GetArgsReturnTypes::INVALID) {
        return {ParseOperationRes::INVALID_ARGS, {}};
    }

    auto res = std::make_unique<OP>(std::move(ptr), std::move(ptrNum), std::move(ptrVal));
    return {ParseOperationRes::VALID, std::move(res)};
}

template<typename DB, typename OP, typename T>
parse_op_r<OP> ParseVectOpVectNum(view_t &view, DBTypes dbType, DB &db,
                                  std::function<std::pair<bool, T>(std::string_view)>
                                  converter) {
    if (view.size() != 2) {
        return {ParseOperationRes::INVALID_ARG_C, {}};
    }

    auto parse = ParseArg(view);
    auto [type, ptr] = getVector(dbType, db, std::move(parse), converter);
    auto parseNum = ParseArg(view);
    auto [typeNum, ptrNum] = getInteger(dbType, db, std::move(parseNum));

    if (type == GetArgsReturnTypes::INVALID || typeNum == GetArgsReturnTypes::INVALID) {
        return {ParseOperationRes::INVALID_ARGS, {}};
    }

    auto res = std::make_unique<OP>(std::move(ptr), std::move(ptrNum));
    return {ParseOperationRes::VALID, std::move(res)};
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

ttl_t ParseTTL(view_t &view, variant_t defaultTTL);

std::pair<uint64_t, uint64_t> sampleKey(const uint64_t seed);


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
};

struct CompareQueueEntry {
    bool operator()(const std::unique_ptr<QueueEntry> &a, const std::unique_ptr<QueueEntry> &b) const {
        const auto a_entry = a.get();
        const auto b_entry = b.get();
        if (!a_entry || !b_entry) {
            return false;
        }
        return a_entry->RemainingTime() > b_entry->RemainingTime();
    }
};

std::pair<bool, std::string> parseQuote(std::string_view &view);

std::string parseUnquote(std::string_view &view);

void SkipSpaces(std::string_view &view);

std::pair<bool, std::vector<std::string> > parseLine(const std::string &line);

void readThread(std::stop_token stoken, std::istream &stream, std::mutex &queueMutex,
                std::queue<std::string> &dataQueue);


#endif //PROJECT_OPERATIONTYPE_HPP
