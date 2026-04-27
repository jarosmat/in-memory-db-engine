#include "OperationType.hpp"


err_t CouldNotOpenFileErr(const std::string &file) {
    return {"Could not open file: ", file};
}

err_t CouldNotReadFileErr(const std::string &file) {
    return {"Could not read file: ", file};
}

err_t InvalidArgs() {
    return {"Invalid arguments: "};
}

err_t InvalidArgsCount(const std::size_t expected, const std::size_t actual) {
    return {"Invalid number of arguments, expected: ", std::to_string(expected), ", got: ", std::to_string(actual)};
}

err_t InvalidOperation() {
    return {"Invalid operation"};
}

read_from_file_r readFileBytes(const std::string &filePath) {
    std::ifstream file(filePath, std::ios::binary | std::ios::ate);

    if (!file.is_open()) {
        return {BytesFromFileRes::COULD_NOT_OPEN, {}};
    }

    std::streamsize size = file.tellg();

    if (size <= 0) {
        return {BytesFromFileRes::VALID, {}};
    }

    file.seekg(0, std::ios::beg);

    std::vector<uint8_t> buffer(size);

    if (file.read(reinterpret_cast<char *>(buffer.data()), size)) {
        return {BytesFromFileRes::VALID, buffer};
    }

    return {BytesFromFileRes::COULD_NOT_READ, {}};
}

std::vector<uint8_t> stringBytes(std::string &&str) {
    std::vector<uint8_t> vec(str.begin(), str.end());
    return vec;
}

OPType GetOPType(view_t &args) {
    if (args.empty()) {
        return OPType::INVALID;
    }

    auto val = args[0];
    args.advance(1);

    if (val == "INSERT") {
        return OPType::INSERT;
    }
    if (val == "GET") {
        return OPType::GET;
    }
    if (val == "DELETE") {
        return OPType::DELETE;
    }
    if (val == "TERMINATE") {
        return OPType::TERMINATE;
    }
    if (val == "ADD") {
        return OPType::ADD;
    }
    if (val == "SUBTRACT") {
        return OPType::SUBTRACT;
    }
    if (val == "DIVIDE") {
        return OPType::DIVIDE;
    }
    if (val == "MULTIPLY") {
        return OPType::MULTIPLY;
    }
    if (val == "FADD") {
        return OPType::FADD;
    }
    if (val == "FSUBTRACT") {
        return OPType::FSUBTRACT;
    }
    if (val == "FDIVIDE") {
        return OPType::FDIVIDE;
    }
    if (val == "FMULTIPLY") {
        return OPType::FMULTIPLY;
    }
    if (val == "CONCAT") {
        return OPType::CONCAT;
    }
    if (val == "SUFFIX") {
        return OPType::SUFFIX;
    }
    if (val == "PREFIX") {
        return OPType::PREFIX;
    }
    if (val == "AT") {
        return OPType::AT;
    }
    if (val == "FIND") {
        return OPType::FIND;
    }
    if (val == "APPEND") {
        return OPType::APPEND;
    }
    if (val == "POP") {
        return OPType::POP;
    }
    if (val == "VINSERT") {
        return OPType::VINSERT;
    }
    if (val == "REMOVE") {
        return OPType::REMOVE;
    }
    if (val == "SET") {
        return OPType::SET;
    }
    if (val == "VGET") {
        return OPType::VGET;
    }
    if (val == "SIZE") {
        return OPType::SIZE;
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

    if (val[0] == '$') {
        return {ArgsType::KEY_FROM_CONSTANT, std::string(val.begin() + 1, val.end())};
    }

    if (val[0] == '@') {
        return {ArgsType::KEY_FROM_FILE, std::string(val.begin() + 1, val.end())};
    }

    return {ArgsType::CONSTANT, std::move(val)};
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

    bool success = (ec == std::errc{}) && (ptr == input.data() + input.size());

    return {success, result};
}

parse_res<Float> ParseFloat(std::string_view input) {
    if (input.empty()) {
        return {false, 0.0};
    }

    Float result = 0.0;

    auto [ptr, ec] = std::from_chars(input.data(), input.data() + input.size(), result);

    bool success = (ec == std::errc{}) && (ptr == input.data() + input.size());

    return {success, result};
}

key_parse ParseKey(view_t &view) {
    if (view.empty()) {
        return {KeyType::INVALID, {}};
    }

    auto val = view.front();
    view.advance(1);

    if (!val.empty() && val[0] == '@') {
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

    if (!val.empty() && val[0] == '@') {
        return {TargetType::FILE, std::string(val.begin() + 1, val.end())};
    }

    return {TargetType::KEY, std::move(val)};
}

ttl_t ParseTTL(view_t &view, ttl_t defaultTTL) {
    if (view.empty()) {
        return defaultTTL;
    }

    auto val = view.front();

    if (val != "--ttl") {
        return defaultTTL;
    }

    view.advance(1);

    return GetTTL(view);
}

std::pair<uint64_t, uint64_t> sampleKey(const uint64_t seed) {
    std::mt19937_64 gen(seed);
    std::uniform_int_distribution<uint64_t> dis;
    return {dis(gen), dis(gen)};
}

std::pair<bool, std::string> parseQuote(std::string_view &view) {
    std::string res;
    while (!view.empty() && view.front() != '"') {
        res.push_back(view.front());
        view.remove_prefix(1);
    }

    if (view.empty()) {
        return {false, {}};
    }


    view.remove_prefix(1);

    return {true, std::move(res)};
}

std::string parseUnquote(std::string_view &view) {
    std::string res;
    while (!view.empty() && !std::isspace(view.front())) {
        res.push_back(view.front());
        view.remove_prefix(1);
    }

    return res;
}

void SkipSpaces(std::string_view &view) {
    while (!view.empty() && std::isspace(view.front())) {
        view.remove_prefix(1);
    }
}

std::pair<bool, std::vector<std::string> > parseLine(const std::string &line) {
    std::vector<std::string> tokens;
    std::string_view view(line.begin(), line.end());

    SkipSpaces(view);
    while (!view.empty()) {
        auto ch = view.front();
        if (ch == '"') {
            view.remove_prefix(1);
            auto [res,val] = parseQuote(view);
            if (!res) {
                return {false, {}};
            }
            tokens.push_back(std::move(val));
        } else {
            tokens.push_back(std::move(parseUnquote(view)));
        }
        SkipSpaces(view);
    }

    return {true, tokens};
}

void readThread(std::stop_token stoken, std::istream &stream, std::mutex &queueMutex,
                std::queue<std::string> &dataQueue) {
    std::string line;
    while (!stoken.stop_requested()) {
        if (std::getline(stream, line)) {
            std::lock_guard<std::mutex> lock(queueMutex);
            dataQueue.push(line);
        }
    }
}
