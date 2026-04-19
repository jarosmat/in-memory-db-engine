#include "DBType.hpp"

#include <complex>


auto ParseUnsignedTTL(std::string &val) -> ttl_t {
    const auto size = val.size();

    if (!std::isdigit(val.at(0))) {
        return {variant_t{}, TTLTypes::INVALID};
    }

    std::size_t digLen = 1;
    for (std::size_t i = 1; i < size && std::isdigit(val.at(i)); ++i) {
        ++digLen;
    }

    if (digLen == size - 2 && val.at(digLen) == 'm' && val.at(digLen + 1) == 's') {
        val.pop_back();
        val.pop_back();
        return {variant_t{std::chrono::milliseconds(std::stoll(val))}, TTLTypes::MILLISECONDS};
    }

    if (digLen == size - 1 && val.at(digLen) == 'm') {
        return {variant_t{std::chrono::minutes(std::stoll(val))}, TTLTypes::MINUTES};
    }

    if (digLen == size - 1 && val.at(digLen) == 's') {
        return {variant_t{std::chrono::seconds(std::stoll(val))}, TTLTypes::SECONDS};
    }

    return {variant_t{}, TTLTypes::INVALID};
}

auto ParseSignedTTL(std::string &val) -> ttl_t {
    const auto size = val.size();

    if (!std::isdigit(val.at(1))) {
        return {variant_t{}, TTLTypes::INVALID};
    }

    std::size_t digLen = 1;
    for (std::size_t i = 1; i < size && std::isdigit(val.at(i)); ++i) {
        ++digLen;
    }

    if (digLen == size - 2 && val.at(digLen) == 'm' && val.at(digLen + 1) == 's') {
        val.pop_back();
        val.pop_back();
        return {variant_t{std::chrono::milliseconds(std::stoll(val))}, TTLTypes::MILLISECONDS};
    }

    if (digLen == size - 1 && val.at(digLen) == 'm') {
        return {variant_t{std::chrono::minutes(std::stoll(val))}, TTLTypes::MINUTES};
    }

    if (digLen == size - 1 && val.at(digLen) == 's') {
        return {variant_t{std::chrono::seconds(std::stoll(val))}, TTLTypes::SECONDS};
    }

    return {variant_t{}, TTLTypes::INVALID};
}

auto GetTTL(view_t &args) -> ttl_t {
    if (args.empty()) {
        return {variant_t{}, TTLTypes::INVALID};
    }

    auto val = args.front();
    args.advance(1);
    const auto size = val.size();

    if (size < 3) {
        return {variant_t{}, TTLTypes::INVALID};
    }

    if (val.at(0) == '-') {
        return ParseSignedTTL(val);
    }
    return ParseUnsignedTTL(val);
}

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
