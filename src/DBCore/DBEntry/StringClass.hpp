#ifndef STRINGCLASS_HPP
#define STRINGCLASS_HPP
#include <set>
#include <string>


class String {
    std::string val;
    class PrefixAut;

public:
    explicit String(const std::string &val);

    explicit String(std::string &&val);

    String();

    static String Concatenate(const std::string &valLeft, const std::string &valRight);

    static String Concatenate(std::string &&valLeft, std::string &&valRight);

    char operator[](size_t i) const;

    void RemovePrefix(size_t n);

    void RemoveSuffix(size_t n);

    [[nodiscard]] std::set<size_t> Find(const String &valToFind) const;

    bool operator==(const String &other) const;

    [[nodiscard]] size_t Size() const;
};

#endif // STRINGCLASS_HPP
