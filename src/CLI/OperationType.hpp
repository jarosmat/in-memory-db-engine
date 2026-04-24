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


#endif //PROJECT_OPERATIONTYPE_HPP
