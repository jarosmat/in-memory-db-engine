#include <variant>

#include "String/String.hpp"
#include "Object/Object.hpp"
#include "Vector.hpp"

using String = String;
using Integer = std::size_t;
using Float = double;
template<typename T>
using VectorEntry = std::vector<T>;
using PrimitiveValuePack = std::tuple<Integer, Float, String>;


#ifndef PROJECT_VALUETYPES_HPP
#define PROJECT_VALUETYPES_HPP

#endif //PROJECT_VALUETYPES_HPP