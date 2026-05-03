#ifndef PROJECT_NULL_HPP
#define PROJECT_NULL_HPP
#include <spanstream>

struct Null {
};


std::ostream &operator<<(std::ostream &os, const Null&);

#endif //PROJECT_NULL_HPP
