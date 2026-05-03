#include "Null.hpp"

std::ostream & operator<<(std::ostream &os, const Null &) {
    os << "null";
    return os;
}
