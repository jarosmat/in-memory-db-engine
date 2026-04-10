#ifndef PROJECT_CLOCK_H
#define PROJECT_CLOCK_H
#include <chrono>

using clock_type = std::chrono::steady_clock;
using time_delta = std::chrono::time_point<std::chrono::steady_clock>;

class Timepoint {
    time_delta time;

public:
    template<typename T>
    [[nodiscard]] auto Elapsed(std::size_t val) const -> bool {
        return std::chrono::duration_cast<T>(clock_type::now() - time) >= T(val);
    }

    explicit Timepoint();
};


#endif //PROJECT_CLOCK_H
