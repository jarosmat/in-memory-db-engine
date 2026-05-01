#ifndef PROJECT_ARRAY_HPP
#define PROJECT_ARRAY_HPP
#include <memory>

template<typename T>
class Vector {
    static constexpr size_t initialSize = 4;
    static constexpr size_t resizeCoef = 2;
    std::size_t size;
    std::unique_ptr<T[]> data;
    std::size_t count;

    [[nodiscard]] bool Full() const;

    void EnsureNotEmpty();

    void Expand();

public:
    ~Vector() = default;

    Vector(const Vector &other);

    Vector(Vector &&other) = default;

    Vector &operator=(const Vector &other);

    Vector &operator=(Vector &&other) noexcept = default;

    Vector();

    explicit Vector(std::size_t size);

    Vector(std::size_t size, const T &initialVal);

    void Append(const T &val);

    void Append(T &&val);

    void Remove(size_t i);

    void Insert(size_t i, T &&val);

    void Insert(size_t i, const T& val);

    const T &CLastEntry() const;

    T &LastEntry();

    const T &operator[](std::size_t i) const;

    T &operator[](std::size_t i);

    [[nodiscard]] size_t Count() const;
};

template<typename T>
std::ostream &operator<<(std::ostream &os, const Vector<T> &obj) {
    if (!obj.Count() == 0) {
        os << "{" << obj[0];
        for (std::size_t i = 1; i < obj.Count(); ++i) {
            os << ", " << obj[i];
        }
        os << "}";
    } else {
        os << "{}";
    }
    os << std::endl;
    return os;
}


#endif //PROJECT_ARRAY_HPP
