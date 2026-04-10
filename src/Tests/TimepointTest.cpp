#include <catch2/catch_test_macros.hpp>
#include "Timepoint.hpp"
#include <thread>

using namespace std;
using namespace std::chrono;


TEST_CASE("Timepoint constructor initializes to current time", "[Timepoint]") {
    const Timepoint tp;
    REQUIRE_FALSE(tp.Elapsed<milliseconds>(1));
    REQUIRE_FALSE(tp.Elapsed<seconds>(1));
    REQUIRE_FALSE(tp.Elapsed<minutes>(1));
}

TEST_CASE("ElapsedMilliseconds returns true after specified milliseconds", "[Timepoint]") {
    const Timepoint tp;
    constexpr size_t dur1 = 80;
    constexpr size_t dur2 = 40;
    this_thread::sleep_for(milliseconds(dur1));
    REQUIRE_FALSE(tp.Elapsed<milliseconds>(100));
    this_thread::sleep_for(milliseconds(dur2));
    REQUIRE(tp.Elapsed<milliseconds>(100));
    REQUIRE_FALSE(tp.Elapsed<milliseconds>(200));
}

TEST_CASE("ElapsedSeconds returns true after specified seconds", "[Timepoint]") {
    const Timepoint tp;
    constexpr size_t dur1 = 900;
    constexpr size_t dur2 = 300;
    this_thread::sleep_for(milliseconds(dur1));
    REQUIRE_FALSE(tp.Elapsed<seconds>(1));
    this_thread::sleep_for(milliseconds(dur2));
    REQUIRE(tp.Elapsed<seconds>(1));
    REQUIRE_FALSE(tp.Elapsed<seconds>(2));
}

TEST_CASE("ElapsedMinutes returns true after specified minutes", "[Timepoint]") {
    const Timepoint tp;
    constexpr size_t dur1 = 59;
    constexpr size_t dur2 = 2;
    this_thread::sleep_for(seconds(dur1));
    REQUIRE_FALSE(tp.Elapsed<minutes>(1));
    this_thread::sleep_for(seconds(dur2));
    REQUIRE(tp.Elapsed<minutes>(1));
    REQUIRE_FALSE(tp.Elapsed<minutes>(2));
}
