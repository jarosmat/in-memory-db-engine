#include "PrefixAut.hpp"

using namespace std;

String::PrefixAut::PrefixAut(const String& toFind) : word(toFind) {
    statesCount = word.Size();
    backEdges = vector<size_t>(statesCount, 0);

    for (size_t i = 1; i < statesCount - 1; i++) {
        backEdges[i + 1] = GetNewState(word[i], backEdges[i]);
    }
}

size_t String::PrefixAut::GetNewState(const char newChar, size_t pos) const {
    if (pos == statesCount) {
        pos = backEdges[pos];
    }
    while (pos > 0 && word[pos] != newChar) {
        pos = backEdges[pos];
    }
    if (newChar == word[pos]) {
        return pos + 1;
    }
    return pos;
}

bool String::PrefixAut::FoundPattern(const size_t pos) const {
    return pos == statesCount;
}
