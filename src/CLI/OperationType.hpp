#ifndef PROJECT_OPERATIONTYPE_HPP
#define PROJECT_OPERATIONTYPE_HPP
#include <functional>
#include <ranges>
#include <vector>
#include "Nodes.hpp"
#include "DBType.hpp"
#include "Vector.hpp"
#include "../DBCore/DBEntry/ValueTypes.hpp"

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

using view_t = std::ranges::subrange<std::vector<std::string>::iterator>;

OPType GetOPType(view_t &args);

using args_parsed = std::pair<ArgsType, std::string>;

args_parsed ParseArg(view_t &args);

template<typename T>
using parse_res = std::pair<bool, T>;

parse_res<String> ParseString(std::string_view input);

parse_res<Integer> ParseInt(std::string_view input);

parse_res<Float> ParseFloat(std::string_view input);

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

    std::vector<T> result;

    while (it < end_boundary) {
        while (it < end_boundary && std::isspace(static_cast<unsigned char>(*it))) {
            ++it;
        }

        if (it == end_boundary) {
            break;
        }

        auto token_end = it;
        while (token_end < end_boundary && *token_end != ',') {
            ++token_end;
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

        result.push_back(value);

        it = token_end + 1;
        // if (it < end_boundary && *it == ',') {
        //     ++it;
        // }
    }

    return {VectorParsingRes::VALID, result};
}


#endif //PROJECT_OPERATIONTYPE_HPP
