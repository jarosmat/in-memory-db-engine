#include "Utils.hpp"


#include <cctype>
#include <fstream>
#include <random>


void SkipSpaces(std::string_view &view) {
    while (!view.empty() && std::isspace(view.front())) {
        view.remove_prefix(1);
    }
}

void RemoveTrailingSpaces(std::string_view &view) {
    while (std::isspace(view.back()) && !view.empty()) {
        view.remove_suffix(1);
    }
}

read_from_file_r readFileBytes(const std::string &filePath) {
    std::ifstream file(filePath, std::ios::binary | std::ios::ate);

    if (!file.is_open()) {
        return {BytesFromFileRes::COULD_NOT_OPEN, {}};
    }

    const std::streamsize size = file.tellg();

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
