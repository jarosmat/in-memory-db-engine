#ifndef PROJECT_OBJECT_H
#define PROJECT_OBJECT_H
#include <unordered_map>
#include "ValueTypes.hpp"


template<typename... Ts>
struct ValueType;

template<typename... Ts>
struct ValueType<std::tuple<Ts...> > {
    using type = std::variant<Ts...>;
};

using Value = ValueType<PrimitiveValuePack>::type;
using Key = std::string;
using TypeForConstruct = std::vector<std::pair<Key, Value> >;
using TypeForConstructIt = TypeForConstruct::iterator;

class Object {
    std::unordered_map<Key, Value> members;

public:
    Object(TypeForConstructIt first, TypeForConstructIt last);

    template<typename T>
    auto get_member(const Key &name) const -> T {
        return std::get<T>(members.at(name));
    }

    template<typename T>
    void set_value(const Key &name, const T &value) {
        members[name] = value;
    }

    template<typename T>
    void set_value(const Key &name, std::remove_reference_t<T> &&value) {
        members[name] = std::move(value);
    }

    auto has_member(const Key &name) const -> bool;
};


#endif //PROJECT_OBJECT_H
