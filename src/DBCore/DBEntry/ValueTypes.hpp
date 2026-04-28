#ifndef PROJECT_VALUETYPES_HPP
#define PROJECT_VALUETYPES_HPP
#include <variant>

#include "String/String.hpp"
#include "Object/Object.hpp"
#include "Vector.hpp"

using String = String;
using Integer = std::int64_t;
using Float = double;
template<typename T>
using VectorEntry = std::vector<T>;
using PrimitiveValuePack = std::tuple<Integer, Float, String>;
using AllValuePack = std::variant<Integer, Float, String, Object, Vector<Integer>, Vector<String>, Vector<Float> >;


#endif //PROJECT_VALUETYPES_HPP
