#include "DBType.hpp"

#include <complex>


auto ParseUnsignedTTL(std::string &val) -> ttl_t {
    const auto size = val.size();

    if (!std::isdigit(static_cast<unsigned char>(val.at(0)))) {
        return {variant_t{}, TTLTypes::INVALID};
    }

    std::size_t digLen = 1;
    for (std::size_t i = 1; i < size && std::isdigit(static_cast<unsigned char>(val.at(i))); ++i) {
        ++digLen;
    }

    if (digLen == size - 2 && val.at(digLen) == API_CONSTANTS::MILLISECONDS[0] && val.at(digLen + 1) ==
        API_CONSTANTS::MILLISECONDS[1]) {
        val.pop_back();
        val.pop_back();
        return {variant_t{std::chrono::milliseconds(std::stoll(val))}, TTLTypes::MILLISECONDS};
    }

    if (digLen == size - 1 && val.at(digLen) == API_CONSTANTS::MINUTES_CH) {
        return {variant_t{std::chrono::minutes(std::stoll(val))}, TTLTypes::MINUTES};
    }

    if (digLen == size - 1 && val.at(digLen) == API_CONSTANTS::SECONDS_CH) {
        return {variant_t{std::chrono::seconds(std::stoll(val))}, TTLTypes::SECONDS};
    }

    return {variant_t{}, TTLTypes::INVALID};
}

auto ParseSignedTTL(std::string &val) -> ttl_t {
    const auto size = val.size();

    if (!std::isdigit(static_cast<unsigned char>(val.at(1)))) {
        return {variant_t{}, TTLTypes::INVALID};
    }

    std::size_t digLen = 1;
    for (std::size_t i = 1; i < size && std::isdigit(static_cast<unsigned char>(val.at(i))); ++i) {
        ++digLen;
    }

    if (digLen == size - 2 && val.at(digLen) == API_CONSTANTS::MILLISECONDS[0] && val.at(digLen + 1) ==
        API_CONSTANTS::MILLISECONDS[1]) {
        val.pop_back();
        val.pop_back();
        return {variant_t{std::chrono::milliseconds(std::stoll(val))}, TTLTypes::MILLISECONDS};
    }

    if (digLen == size - 1 && val.at(digLen) == API_CONSTANTS::MINUTES_CH) {
        return {variant_t{std::chrono::minutes(std::stoll(val))}, TTLTypes::MINUTES};
    }

    if (digLen == size - 1 && val.at(digLen) == API_CONSTANTS::SECONDS_CH) {
        return {variant_t{std::chrono::seconds(std::stoll(val))}, TTLTypes::SECONDS};
    }

    return {variant_t{}, TTLTypes::INVALID};
}

auto GetOptionToken(view_t &args) -> StartupTokens {
    const auto val = args.front();
    args.advance(1);
    if (val == API_CONSTANTS::VALUE_O) {
        return StartupTokens::VALUE;
    }
    if (val == API_CONSTANTS::TTL_O) {
        return StartupTokens::TTL;
    }
    if (val == API_CONSTANTS::STRUCT_O) {
        return StartupTokens::STRUCT;
    }

    return StartupTokens::INVALID;
}

auto GetDBType(view_t &args) -> std::tuple<DBTypes, ttl_t, Object, object_reference> {
    auto resType = DBTypes::INVALID;
    auto resTTL = ttl_t(ImmortalSentinel(), TTLTypes::IMMORTAL);
    Object resObject;
    object_reference resRef;

    bool gotStruct = false;
    bool gotValue = false;
    bool gotTTL = false;


    while (!args.empty()) {
        const StartupTokens val = GetOptionToken(args);
        if (val == StartupTokens::INVALID) {
            return {DBTypes::INVALID, ttl_t{}, {}, {}};
        }
        if (val == StartupTokens::VALUE) {
            if (gotValue) {
                return {DBTypes::INVALID, ttl_t{}, {}, {}};
            }

            const auto res = GetType(args);
            if (gotStruct && res != DBTypes::OBJECT) {
                return {DBTypes::INVALID, ttl_t{}, {}, {}};
            }

            resType = res;
            gotValue = true;
        }
        if (val == StartupTokens::TTL) {
            if (gotTTL) {
                return {DBTypes::INVALID, ttl_t{}, {}, {}};
            }
            resTTL = GetTTL(args);
            gotTTL = true;
        }

        if (val == StartupTokens::STRUCT) {
            if (gotValue && resType != DBTypes::OBJECT) {
                return {DBTypes::INVALID, ttl_t{}, {}, {}};
            }

            if (gotStruct) {
                return {DBTypes::INVALID, ttl_t{}, {}, {}};
            }
            auto [msg, obj, ref] = ParseObjectStructure(args);
            if (msg != ObjectParseRes::VALID) {
                return {DBTypes::INVALID, ttl_t{}, {}, {}};
            }

            gotStruct = true;

            resType = DBTypes::OBJECT;
            resObject = std::move(obj);
            resRef = std::move(ref);
        }
    }
    return {resType, resTTL, std::move(resObject), std::move(resRef)};
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

    return ParseUnsignedTTL(val);
}

auto GetType(view_t &view) -> DBTypes {
    if (view.empty()) {
        return DBTypes::INVALID;
    }

    const auto val = view.front();
    view.advance(1);
    if (val == API_CONSTANTS::INT) {
        return DBTypes::INTEGER;
    }
    if (val == API_CONSTANTS::FLOAT) {
        return DBTypes::FLOAT;
    }
    if (val == API_CONSTANTS::STRING) {
        return DBTypes::STRING;
    }
    if (val == API_CONSTANTS::OBJECT) {
        return DBTypes::OBJECT;
    }
    if (val == API_CONSTANTS::VECT_INT) {
        return DBTypes::VECTOR_INT;
    }
    if (val == API_CONSTANTS::VECT_FLOAT) {
        return DBTypes::VECTOR_FLOAT;
    }
    if (val == API_CONSTANTS::VECT_STRING) {
        return DBTypes::VECTOR_STRING;
    }
    return DBTypes::INVALID;
}

std::tuple<std::string, ObjectFieldTypes, bool> ParseFieldStartup(std::string_view view) {
    RemoveTrailingSpaces(view);
    SkipSpaces(view);
    auto nameIt = view.begin();

    while (nameIt != view.end() && *nameIt != ':' && !std::isspace(static_cast<unsigned char>(*nameIt))) {
        ++nameIt;
    }

    if (nameIt == view.end()) {
        return {{}, ObjectFieldTypes::INVALID_FIELD, false};
    }

    std::string name(view.begin(), nameIt);

    std::size_t mv = 0;
    auto beg = view.begin();
    while (nameIt != beg) {
        ++mv;
        ++beg;
    }

    view.remove_prefix(mv);


    SkipSpaces(view);

    if (view.front() != ':') {
        return {{}, ObjectFieldTypes::INVALID_FIELD, false};
    }
    view.remove_prefix(1);

    SkipSpaces(view);

    auto typeIt = view.begin();

    while (typeIt != view.end() && *typeIt != '-' && !std::isspace(static_cast<unsigned char>(*typeIt))) {
        ++typeIt;
    }

    const std::string typeVal(view.begin(), typeIt);


    std::size_t mvTp = 0;
    auto begTp = view.begin();
    auto endTp = view.end();
    while (endTp != begTp && begTp != typeIt) {
        ++mvTp;
        ++begTp;
    }
    view.remove_prefix(mvTp);


    ObjectFieldTypes resType;

    if (typeVal == API_CONSTANTS::INT) {
        resType = ObjectFieldTypes::INTEGER;
    } else if (typeVal == API_CONSTANTS::FLOAT) {
        resType = ObjectFieldTypes::FLOAT;
    } else if (typeVal == API_CONSTANTS::STRING) {
        resType = ObjectFieldTypes::STRING;
    } else {
        return {{}, ObjectFieldTypes::INVALID_TYPE, false};
    }

    if (view.empty()) {
        return {std::move(name), resType, false};
    }

    if (view.size() == 2 && view.front() == API_CONSTANTS::OPTIONAL_IS[0] && view.back() == API_CONSTANTS::OPTIONAL_IS[1]) {
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
    auto val = viewIn.front();
    viewIn.advance(1);
    const std::string_view view(val.begin(), val.end());
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
