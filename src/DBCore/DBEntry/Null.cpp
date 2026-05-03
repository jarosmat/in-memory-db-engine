#include "Null.hpp"

bool operator==(const Null &, const Null &) {
    return true;
}

std::ostream & operator<<(std::ostream &os, const Null &) {
    os << "null";
    return os;
}
