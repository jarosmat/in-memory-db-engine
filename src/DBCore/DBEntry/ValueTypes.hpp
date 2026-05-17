#ifndef PROJECT_VALUETYPES_HPP
#define PROJECT_VALUETYPES_HPP

#include <variant>
#include "Null.hpp"
#include "String/String.hpp"
#include "Object/Object.hpp"
#include "Vector.hpp"

using String = String;
using Integer = std::int64_t;
using Float = double;
template<typename T>
using VectorEntry = std::vector<T>;
using PrimitiveValuePack = std::tuple<Integer, Float, String, Null>;
// using AllValuePack = std::variant<Integer, Float, String, Object, Vector<Integer>, Vector<String>, Vector<Float> >;


template<typename... Ts>
struct ValueType;

template<typename... Ts>
struct ValueType<std::tuple<Ts...> > {
    using type = std::variant<Ts..., Vector<Ts>...>;
};

using Value = ValueType<PrimitiveValuePack>::type;
using Key = std::string;
using TypeForConstruct = std::vector<std::pair<Key, Value> >;
using TypeForConstructIt = TypeForConstruct::iterator;

class Object {
    std::unordered_map<Key, Value> members;

public:
    Object(TypeForConstructIt first, TypeForConstructIt last);

    Object() = default;

    template<typename T>
    auto get_member(const Key &name) const -> T {
        return std::get<T>(members.at(name));
    }

    void dump_to_stream(std::ostream& stream, const Key &key) const {
        std::visit([&stream](auto&&arg){stream << arg;}, members.at(key));
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

    std::size_t MemberCount() const;


    friend std::ostream &operator<<(std::ostream &os, const Object &obj) {
        auto visitor = [&os](const auto &value) {
            os << value;
        };
        if (obj.MemberCount() == 0) {
            os << "{}";
            return os;
        }
        os << '{';
        auto iter = obj.members.begin();
        os << iter->first << ": ";
        std::visit(visitor, iter->second);
        ++iter;
        for (; iter != obj.members.end(); ++iter) {
            os << ", " << iter->first << ": ";
            std::visit(visitor, iter->second);
        }
        os << '}';
        return os;
    }
};

#endif //PROJECT_VALUETYPES_HPP
