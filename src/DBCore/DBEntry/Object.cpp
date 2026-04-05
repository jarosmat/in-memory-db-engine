#include "Object.hpp"

Object::Object(const TypeForConstructIt first, const TypeForConstructIt last) : members(first, last) {
}

auto Object::has_member(const std::string &name) const -> bool {
    return members.contains(name);
}
