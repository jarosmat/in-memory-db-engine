#ifndef PROJECT_UTILS_HPP
#define PROJECT_UTILS_HPP
#include <cstdint>
#include <string_view>
#include <vector>

void SkipSpaces(std::string_view &view);

void RemoveTrailingSpaces(std::string_view &view);

enum class BytesFromFileRes {
    VALID,
    COULD_NOT_OPEN,
    COULD_NOT_READ
};

using read_from_file_r = std::pair<BytesFromFileRes, std::vector<uint8_t> >;

read_from_file_r readFileBytes(const std::string &filePath);

std::vector<uint8_t> stringBytes(std::string &&str);

std::pair<uint64_t, uint64_t> sampleKey(uint64_t seed);

std::pair<bool, std::string> parseQuote(std::string_view &view);

std::string parseUnquote(std::string_view &view);


std::pair<bool, std::vector<std::string> > parseLine(const std::string &line);


#endif //PROJECT_UTILS_HPP
