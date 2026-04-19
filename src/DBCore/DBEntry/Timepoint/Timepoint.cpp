#include "Timepoint.hpp"

using namespace std::chrono;

Timepoint::Timepoint():time(clock_type::now()) {
}
