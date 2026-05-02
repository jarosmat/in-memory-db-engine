#ifndef VECTOR_TPP
#define VECTOR_TPP
#include "VectorClass.hpp"


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
void Vector<T>::Expand() {
    const size_t newSize = size * resizeCoef;
    std::unique_ptr<T[]> newData = std::make_unique<T[]>(newSize);
    auto newDataVal = newData.get();
    auto dataVal = data.get();

    for (size_t i = 0; i < count; i++) {
        newDataVal[i] = std::move(dataVal[i]);
    }

    data.swap(newData);
    size = newSize;
}

template<typename T>
Vector<T>::Vector(const Vector &other) : size(other.size), data(std::make_unique<T[]>(size)), count(other.count) {
    for (std::size_t i = 0; i < count; ++i) {
        data.get()[i] = other.data.get()[i];
    }
}

template<typename T>
Vector<T> &Vector<T>::operator=(const Vector &other) {
    size = other.size;
    data = std::make_unique<T[]>(size);
    count = other.count;
    for (std::size_t i = 0; i < count; ++i) {
        data.get()[i] = other.data.get()[i];
    }
    return *this;
}

template<typename T>
Vector<T>::Vector() : size(initialSize), data(std::make_unique<T[]>(this->size)), count(0) {
}


template<typename T>
Vector<T>::Vector(const size_t size) : size(size), data(std::make_unique<T[]>(this->size)), count(0) {
}

template<typename T>
Vector<T>::Vector(const size_t size, const T &initialVal) : size(size * resizeCoef),
                                                            data(std::make_unique<T[]>(this->size)),
                                                            count(size) {
    for (size_t i = 0; i < size; i++) {
        data.get()[i] = initialVal;
    }
}

template<typename T>
void Vector<T>::Append(const T &val) {
    EnsureNotEmpty();
    data.get()[count++] = val;
}

template<typename T>
void Vector<T>::Append(T &&val) {
    EnsureNotEmpty();
    data.get()[count++] = std::move(val);
}

template<typename T>
void Vector<T>::Remove(const size_t i) {
    count--;
    auto dataVal = data.get();
    for (size_t j = i; j < count; j++) {
        dataVal[j] = dataVal[j + 1];
    }
}

template<typename T>
void Vector<T>::Insert(size_t i, T &&val) {
    EnsureNotEmpty();
    if (i > Count()) {
        throw "Index out of bounds Error";
    }
    if (i == Count()) {
        Append(std::move(val));
    }
    else {
        Append(std::move(data.get()[Count() - 1]));
        for (size_t j = Count() - 1; i < j; --j) {
           data.get()[j] = std::move(data.get()[j - 1]);
        }
        data.get()[i] = std::move(val);
    }
}

template<typename T>
void Vector<T>::Insert(size_t i, const T &val) {
    EnsureNotEmpty();
    if (i > Count()) {
        throw "Index out of bounds Error";
    }
    if (i == Count()) {
        Append(val);
    }
    else {
        Append(std::move(data.get()[Count() - 1]));
        for (size_t j = Count() - 1; i < j; --j) {
            data.get()[j] = std::move(data.get()[j - 1]);
        }
        data.get()[i] = val;
    }
}

template<typename T>
const T &Vector<T>::CLastEntry() const {
    return data.get()[count - 1];
}

template<typename T>
T &Vector<T>::LastEntry() {
    return data.get()[count - 1];
}

template<typename T>
const T &Vector<T>::operator[](const size_t i) const {
    if (i < count) {
        return data.get()[i];
    }
    throw "Index out of Bounds Error";
}

template<typename T>
T &Vector<T>::operator[](const size_t i) {
    if (i < count) {
        return data.get()[i];
    }
    throw "Index out of Bounds Error";
}

template<typename T>
size_t Vector<T>::Count() const {
    return count;
}

template<typename T>
std::ostream & operator<<(std::ostream &os, const Vector<T> &obj) {
    if (!obj.Count() == 0) {
        os << "{" << obj[0];
        for (std::size_t i = 1; i < obj.Count(); ++i) {
            os << ", " << obj[i];
        }
        os << "}";
    } else {
        os << "{}";
    }
    return os;
}
#endif // VECTOR_TPP
