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

auto GetOptionToken(view_t &args) -> StartupTokens {
    const auto val = args.front();
    args.advance(1);
    if (val == "--value") {
        return StartupTokens::VALUE;
    }
    if (val == "--ttl") {
        return StartupTokens::TTL;
    }

    return StartupTokens::INVALID;
}

auto GetDBType(view_t &args) -> std::pair<DBTypes, ttl_t> {
    std::pair res = {DBTypes::INVALID, ttl_t(ImmortalSentinel(), TTLTypes::IMMORTAL)};

    while (!args.empty()) {
        const StartupTokens val = GetOptionToken(args);
        if (val == StartupTokens::INVALID) {
            return {DBTypes::INVALID, ttl_t{}};
        }
        if (val == StartupTokens::VALUE) {
            res.first = GetType(args);
        }
        if (val == StartupTokens::TTL) {
            res.second = GetTTL(args);
        }
    }
    return res;
}

auto GetTTL(view_t &args) -> ttl_t {
    if (args.empty()) {
        return {variant_t{}, TTLTypes::INVALID};
    }

    std::string val = args.front();
    args.advance(1);

    if (const auto size = val.size(); size < 2) {
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

std::tuple<std::string, ObjectFieldTypes, bool> ParseFieldStartup(std::string_view view) {
    RemoveTrailingSpaces(view);
    SkipSpaces(view);
    auto nameIt = view.begin();

    while (*nameIt != ':' && !std::isspace(*nameIt) && nameIt != view.end()) {
        ++nameIt;
    }

    if (nameIt == view.end()) {
        return {{}, ObjectFieldTypes::INVALID_FIELD, false};
    }

    std::string name(view.begin(), nameIt);

    while (nameIt != view.begin()) {
        view.remove_prefix(1);
    }

    SkipSpaces(view);

    if (view.front() != ':') {
        return {{}, ObjectFieldTypes::INVALID_FIELD, false};
    }
    view.remove_prefix(1);

    SkipSpaces(view);

    auto typeIt = view.begin();

    while (*typeIt != '-' && !std::isspace(*typeIt) && typeIt != view.end()) {
        ++typeIt;
    }

    const std::string typeVal(view.begin(), typeIt);

    while (view.begin() != typeIt) {
        view.remove_prefix(1);
    }

    ObjectFieldTypes resType;

    if (typeVal == "int") {
        resType = ObjectFieldTypes::INTEGER;
    } else if (typeVal == "float") {
        resType = ObjectFieldTypes::FLOAT;
    } else if (typeVal == "string") {
        resType = ObjectFieldTypes::STRING;
    } else {
        return {{}, ObjectFieldTypes::INVALID_TYPE, false};
    }

    if (view.empty()) {
        return {std::move(name), resType, false};
    }

    if (view.size() == 2 && view.front() == '-' && view.back() == 'O') {
        return {std::move(name), resType, true};
    }

    return {{}, ObjectFieldTypes::INVALID_TYPE, false};
}

std::pair<SplitFieldRes, std::vector<std::string> > SplitToFields(std::string_view view) {
    SkipSpaces(view);
    RemoveTrailingSpaces(view);

    if (view.empty()) {
        return {SplitFieldRes::EMPTY_STRING, {}};
    }

    if (view.front() != '{' || view.back() != '}') {
        return {SplitFieldRes::INVALID_PARENTHESIS, {}};
    }


    view.remove_prefix(1);
    view.remove_suffix(1);

    SkipSpaces(view);
    RemoveTrailingSpaces(view);

    if (view.empty()) {
        return {SplitFieldRes::VALID, {}};
    }


    auto endIt = view.begin();
    auto startIt = view.begin();
    const auto end = view.end();

    std::vector<std::string> res;

    while (endIt != end) {
        if (*endIt == ',') {
            res.emplace_back(startIt, endIt);
            ++endIt;
            startIt = endIt;
        } else {
            ++endIt;
        }
    }

    res.emplace_back(startIt, endIt);

    return {SplitFieldRes::VALID, std::move(res)};
}

std::tuple<ObjectParseRes, Object, object_reference> ParseObjectStructure(view_t &viewIn) {
    if (viewIn.empty()) {
        return {ObjectParseRes::INVALID_STRUCTURE, {}, {}};
    }

    const std::string_view view(viewIn.front().begin(), viewIn.front().end());
    auto [msg, res] = SplitToFields(view);

    if (msg == SplitFieldRes::INVALID_PARENTHESIS || msg == SplitFieldRes::EMPTY_STRING) {
        return {ObjectParseRes::INVALID_STRUCTURE, {}, {}};
    }

    Object refObject;
    object_reference reference;

    for (auto &&a: res) {
        const std::string_view fieldView(a.begin(), a.end());

        if (auto [name, fieldMsg, optional] = ParseFieldStartup(fieldView); fieldMsg == ObjectFieldTypes::FLOAT) {
            if (optional) {
                refObject.set_value(name, Null());
            }
            reference.insert({std::move(name), ObjectFieldTypes::FLOAT});
        } else if (fieldMsg == ObjectFieldTypes::INTEGER) {
            if (optional) {
                refObject.set_value(name, Null());
            }
            reference.insert({std::move(name), ObjectFieldTypes::INTEGER});
        } else if (fieldMsg == ObjectFieldTypes::STRING) {
            if (optional) {
                refObject.set_value(name, Null());
            }
            reference.insert({std::move(name), ObjectFieldTypes::STRING});
        } else if (fieldMsg == ObjectFieldTypes::INVALID_TYPE) {
            return {ObjectParseRes::INVALID_TYPE, std::move(refObject), std::move(reference)};
        } else {
            return {ObjectParseRes::INVALID_FIELD, std::move(refObject), std::move(reference)};
        }
    }

    return {ObjectParseRes::VALID, std::move(refObject), std::move(reference)};
}
