#include "String.hpp"

using namespace std;

String::String(const string &val) : val(val) {
}

String::String(string &&val) : val(std::move(val)) {
}

String::String(const char ch) {
    val.push_back(ch);
}

String::String() {
}

String String::Concatenate(const string &valLeft, const string &valRight) {
    string result = valLeft + valRight;

    return String(std::move(result));
}

String String::Concatenate(string &&valLeft, string &&valRight) {
    string result(std::move(valLeft));
    result += valRight;

    return String(std::move(result));
}

String String::Concatenate(String &&valLeft, String &&valRight) {
    string result(std::move(valLeft.val));

    result += valRight.val;

    return String(std::move(result));
}

char String::operator[](const size_t i) const {
    if (i < val.size())
        return val[i];

    throw "Index out of Bounds";
}

void String::RemovePrefix(const size_t n) {
    if (n >= val.size()) {
        val.clear();
    } else {
        val.erase(val.begin(), val.begin() + n);
    }
}

void String::RemoveSuffix(const size_t n) {
    if (n >= val.size()) {
        val.clear();
    } else {
        val.erase(val.end() - n, val.end());
    }
}

set<size_t> get_num(const size_t num) {
   set<size_t> res;
    for (auto i{0uz}; i < num; ++i) {
        res.insert(i);
    }

    return res;
}

set<size_t> String::Find(const String &valToFind) const {
    if (valToFind.Size() == 0) {
        return get_num(Size());
    }
    set<size_t> result;
    auto &thisVal = this->val;
    const PrefixAut aut(valToFind);
    const size_t findValSize = valToFind.Size();
    size_t pos = 0;

    for (size_t i = 0; i < thisVal.size(); i++) {
        pos = aut.GetNewState(thisVal[i], pos);

        if (aut.FoundPattern(pos)) {
            result.insert(i - findValSize + 1);
        }
    }
    return result;
}

bool String::operator==(const String &other) const {
    return this->val == other.val;
}

size_t String::Size() const {
    return val.size();
}

String::operator string() {
    return val;
}
