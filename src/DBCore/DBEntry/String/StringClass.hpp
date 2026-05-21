#ifndef STRINGCLASS_HPP
#define STRINGCLASS_HPP
#include <set>
#include <string>
#include <iostream>


class String {
    std::string val;
    class PrefixAut;

public:
    explicit String(const std::string &val);

    explicit String(std::string &&val);

    explicit String(char ch);

    String();

    static String Concatenate(const std::string &valLeft, const std::string &valRight);

    static String Concatenate(std::string &&valLeft, std::string &&valRight);

    static String Concatenate(String &&valLeft, String &&valRight);

    char operator[](size_t i) const;

    void RemovePrefix(size_t n);

    void RemoveSuffix(size_t n);

    [[nodiscard]] std::set<size_t> Find(const String &valToFind) const;

    bool operator==(const String &other) const;

    [[nodiscard]] size_t Size() const;

    explicit operator std::string();

    friend std::ostream &operator<<(std::ostream &out, const String &value) {
        return (out << value.val);
    }
};

#endif // STRINGCLASS_HPP
