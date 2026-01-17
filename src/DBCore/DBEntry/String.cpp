#include "String.hpp"

String::String(const string &val): val(val) {
}

String::String(string &&val): val(std::move(val)) {
}

String String::Concatenate(const string &valLeft, const string &valRight) {
    string result = valLeft + valRight;

    return {std::move(result)};
}

String String::Concatenate(string &&valLeft, string &&valRight) {
    string result(std::move(valLeft));
    result += valRight;

    return {std::move(result)};
}

char String::operator[](const size_t i) const {
    if (i < val.size())
        return val[i];

    throw "Index out of Bounds";
}

void String::RemovePrefix(const size_t n) {
}

void String::RemoveSuffix(const size_t n) {
}

set<size_t> String::Find(const String &val) const {
}

set<size_t> String::Find(string &&val) const {
}

bool String::operator==(const String &other) const {
    return this->val == other.val;
}
