#include "Vector.hpp"

using namespace std;

template<typename T>
bool Vector<T>::Full() const {
    return count == size;
}

template<typename T>
void Vector<T>::EnsureNotEmpty() {
    if (Full()) {
        Expand();
    }
}

template<typename T>
Vector<T>::Vector(): size(initialSize), data(std::make_unique<T[initialSize]>()), count(0) {
}


template<typename T>
Vector<T>::Vector(const size_t size) : size(size), data(std::make_unique<T[size]>()), count(0) {
}

template<typename T>
Vector<T>::Vector(const size_t size, T &initialVal): size(size * resizeCoef), data(std::make_unique<T[this->size]>()), count(size){
    for (size_t i = 0; i < size; i++) {
        data.get()[i] = initialVal;
    }
}

template<typename T>
void Vector<T>::Append(const T& val) {
    EnsureNotEmpty();
    data.get()[count++] = val;
}

template<typename T>
void Vector<T>::Append(T &&val) {
    EnsureNotEmpty();
    data.get()[count++] = std::move(val);
}

template<typename T>
const T & Vector<T>::CLastEntry() const {
    return data.get()[size - 1];
}

template<typename T>
T & Vector<T>::LastEntry() {
    return data.get()[size - 1];
}

template<typename T>
const T & Vector<T>::operator[](const size_t i) const {
    if (i < count) {
        return data.get()[i];
    }
    throw "Index out of Bounds Error";
}

template<typename T>
T & Vector<T>::operator[](const size_t i) {
    if (i < count) {
        return data.get()[i];
    }
    throw "Index out of Bounds Error";
}

template<typename T>
size_t Vector<T>::Count() const {
    return count;
}
