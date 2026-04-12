#ifndef PROJECT_CLOCK_H
#define PROJECT_CLOCK_H
#include <chrono>

using clock_type = std::chrono::steady_clock;
using time_delta = std::chrono::time_point<std::chrono::steady_clock>;

class Timepoint {
    time_delta time;

public:
    template<typename T>
    [[nodiscard]] auto Elapsed(T &&val) const -> bool {
        return std::chrono::duration_cast<std::decay_t<T> >(clock_type::now() - time) >= std::forward<T>(val);
    }

    template<typename T>
    auto RemainingTime(T &&val)const -> std::decay_t<T> {
        if (Elapsed(std::forward<T>(val))) {
            constexpr std::size_t deadVal = 0;
            return T(deadVal);
        }
        return std::forward<T>(val) - std::chrono::duration_cast<std::decay_t<T>>(clock_type::now() - time);
    }

    explicit Timepoint();
};


#endif //PROJECT_CLOCK_H
