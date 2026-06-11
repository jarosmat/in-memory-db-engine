#include "OperationsParser.hpp"

args_parsed ParseArg(view_t &args) {
    if (args.empty()) {
        return {ArgsType::EMPTY, ""};
    }

    auto val = args[0];
    args.advance(1);

    if (val.empty()) {
        return {ArgsType::CONSTANT, ""};
    }

    if (val[0] == '\\') {
        return {ArgsType::CONSTANT, std::string(val.begin() + 1, val.end())};
    }

    if (val[0] == API_CONSTANTS::FROM_DB_ID) {
        return {ArgsType::KEY_FROM_CONSTANT, std::string(val.begin() + 1, val.end())};
    }

    if (val[0] == API_CONSTANTS::FILE_ID) {
        return {ArgsType::KEY_FROM_FILE, std::string(val.begin() + 1, val.end())};
    }

    return {ArgsType::CONSTANT, std::move(val)};
}


std::tuple<std::string, ObjectFieldTypes, std::string> ParseField(std::string_view view) {
    RemoveTrailingSpaces(view);
    SkipSpaces(view);
    auto nameIt = view.begin();

    while (nameIt != view.end() && *nameIt != ':' && !std::isspace(*nameIt)) {
        ++nameIt;
    }

    if (nameIt == view.end()) {
        return {{}, ObjectFieldTypes::INVALID_FIELD, std::string()};
    }

    std::string name(view.begin(), nameIt);

    std::size_t mv = 0;
    auto beg = view.begin();
    const auto end = view.end();
    while (beg != end && nameIt != beg) {
        ++mv;
        ++beg;
    }
    view.remove_prefix(mv);


    SkipSpaces(view);

    if (view.front() != ':') {
        return {{}, ObjectFieldTypes::INVALID_FIELD, std::string()};
    }
    view.remove_prefix(1);

    SkipSpaces(view);

    auto valIt = view.begin();

    while (valIt != view.end() && !std::isspace(*valIt)) {
        ++valIt;
    }

    std::string value(view.begin(), valIt);


    std::size_t mvVal = 0;
    auto begVal = view.begin();
    const auto endVal = view.end();
    while (endVal != begVal && begVal != valIt) {
        ++mvVal;
        ++begVal;
    }
    view.remove_prefix(mvVal);

    if (view.empty()) {
        return {std::move(name), ObjectFieldTypes::VALID, std::move(value)};
    }

    return {{}, ObjectFieldTypes::INVALID_TYPE, std::string()};
}

parse_res<String> ParseString(const std::string_view input) {
    if (input.length() < 2) {
        return {false, String()};
    }
    if (input.front() != '\'' || input.back() != '\'') {
        return {false, String()};
    }

    std::string res(input.begin() + 1, input.end() - 1);
    return {true, String(std::move(res))};
}

parse_res<Integer> ParseInt(const std::string_view input) {
    if (input.empty()) {
        return {false, 0};
    }

    Integer result = 0;

    auto [ptr, ec] = std::from_chars(input.data(), input.data() + input.size(), result);

    bool success = ec == std::errc{} && ptr == input.data() + input.size();

    return {success, result};
}

parse_res<Float> ParseFloat(const std::string_view input) {
    if (input.empty()) {
        return {false, 0.0};
    }

    Float result = 0.0;

    auto [ptr, ec] = std::from_chars(input.data(), input.data() + input.size(), result);

    bool success = ec == std::errc{} && ptr == input.data() + input.size();

    return {success, result};
}

parse_res<Object> ParseObject(std::string_view input, Object referenceObject, const object_reference &reference) {
    RemoveTrailingSpaces(input);
    SkipSpaces(input);
    if (input.size() < 2 || input.front() != '{' || input.back() != '}') {
        return {false, {}};
    }

    input.remove_prefix(1);
    input.remove_suffix(1);

    while (!input.empty()) {
        auto itNext = input.begin();
        while (itNext != input.end() && *itNext != ',') {
            ++itNext;
        }

        const std::string_view fieldView(input.begin(), itNext);

        auto [name, type, value] = ParseField(fieldView);

        if (type == ObjectFieldTypes::INVALID_FIELD || type == ObjectFieldTypes::INVALID_TYPE) {
            return {false, {}};
        }

        auto lookUp = reference.find(name);

        if (lookUp == reference.end()) {
            return {false, {}};
        }

        const std::string_view valView(value.begin(), value.end());
        if (lookUp->second == ObjectFieldTypes::INTEGER) {
            auto [resMsg, valueParsed] = ParseInt(valView);
            if (!resMsg) {
                return {false, {}};
            }
            referenceObject.set_value(name, valueParsed);
        }

        if (lookUp->second == ObjectFieldTypes::FLOAT) {
            auto [resMsg, valueParsed] = ParseFloat(valView);
            if (!resMsg) {
                return {false, {}};
            }
            referenceObject.set_value(name, valueParsed);
        }

        if (lookUp->second == ObjectFieldTypes::STRING) {
            auto [resMsg, valueParsed] = ParseString(valView);
            if (!resMsg) {
                return {false, {}};
            }
            referenceObject.set_value(name, valueParsed);
        }

        std::size_t mv = 0;
        auto beg = input.begin();
        const auto end = input.end();
        while (beg != end && itNext != beg) {
            ++mv;
            ++beg;
        }
        input.remove_prefix(mv);


        if (!input.empty()) {
            input.remove_prefix(1);
        }
    }

    if (referenceObject.MemberCount() != reference.size()) {
        return {false, {}};
    }

    return {true, std::move(referenceObject)};
}

key_parse ParseKey(view_t &view) {
    if (view.empty()) {
        return {KeyType::INVALID, {}};
    }

    auto val = view.front();
    view.advance(1);

    if (!val.empty() && val[0] == API_CONSTANTS::FILE_ID) {
        return {KeyType::FILE_PATH, std::string(val.begin() + 1, val.end())};
    }

    return {KeyType::CONSTANT, std::move(val)};
}

target_parse ParseTarget(view_t &view) {
    if (view.empty()) {
        return {TargetType::INVALID, {}};
    }

    auto val = view.front();
    view.advance(1);

    if (!val.empty() && val[0] == API_CONSTANTS::FILE_ID) {
        return {TargetType::FILE, std::string(val.begin() + 1, val.end())};
    }

    return {TargetType::KEY, std::move(val)};
}

ttl_t ParseTTL(view_t &view, ttl_t defaultTTL) {
    if (view.empty()) {
        return defaultTTL;
    }

    if (const auto val = view.front(); val != API_CONSTANTS::TTL_O) {
        return defaultTTL;
    }

    view.advance(1);

    return GetTTL(view);
}

parse_op_r<DBopsNode<Object> > ParseInsert(view_t &view, DBType<Object> &db, const ttl_t &ttl, ttlQueueType &queue,
                                           const object_reference &reference, const Object &referenceObject) {
    return ParseInsertWithParser<Object>(
        view, db, [&reference, &referenceObject](DBType<Object> &dbRef, args_parsed &&args) {
            return getObject(dbRef, std::move(args), referenceObject, reference);
        }, ttl, queue);
}

std::unique_ptr<ErrorNode> GetError(const ParseOperationRes res, const std::size_t expected, const std::size_t actual,
                                    std::ostream &outstream, const std::string &file) {
    if (res == ParseOperationRes::INVALID_ARG_C) {
        auto node = std::make_unique<ErrorNode>(outstream, std::move(InvalidArgsCount(expected, actual)));
        return node;
    }
    if (res == ParseOperationRes::INVALID_ARGS) {
        auto node = std::make_unique<ErrorNode>(outstream, std::move(InvalidArgs()));
        return node;
    }
    if (res == ParseOperationRes::COULD_NOT_OPEN_FILE) {
        auto node = std::make_unique<ErrorNode>(outstream, std::move(CouldNotOpenFileErr(file)));
        return node;
    }
    if (res == ParseOperationRes::COULD_NOT_READ_FILE) {
        auto node = std::make_unique<ErrorNode>(outstream, std::move(CouldNotReadFileErr(file)));
        return node;
    }
    return {};
}



bool HasTarget(const OPType op) {
    return op != OPType::INVALID && op != OPType::INSERT && op != OPType::GET && op != OPType::TERMINATE && op !=
           OPType::DELETE;
}
