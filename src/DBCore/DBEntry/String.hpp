#ifndef PROJECT_STRING_HPP
#define PROJECT_STRING_HPP
#include <set>
#include <string>

using namespace std;

class String {
    string val;
public:
    String(const string& val);

    String(string&& val);

    static String Concatenate(const string& valLeft, const string& valRight);

    static String Concatenate(string&& valLeft, string&& valRight);

    char operator[](const size_t i) const;

    void RemovePrefix(const size_t n);

    void RemoveSuffix(const size_t n);

    set<size_t> Find(const String& val) const;

    set<size_t> Find(string&& val) const ;

    bool operator==(const String &other) const;
};


#endif //PROJECT_STRING_HPP