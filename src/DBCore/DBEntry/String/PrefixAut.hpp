#ifndef PROJECT_PREFIXAUT_HPP
#define PROJECT_PREFIXAUT_HPP
#include "StringClass.hpp"
#include <vector>

class String::PrefixAut {
public:
    explicit PrefixAut(const String& toFind);

    [[nodiscard]] size_t GetNewState(char newChar, size_t pos) const;

    [[nodiscard]] bool FoundPattern(size_t pos) const;
private:
    const String& word;
    size_t statesCount;
    std::vector<size_t> backEdges;
};

#endif //PROJECT_PREFIXAUT_HPP