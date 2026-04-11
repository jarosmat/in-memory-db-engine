#include <catch2/catch_test_macros.hpp>
#include "Timepoint.hpp"
#include <thread>

using namespace std;
using namespace std::chrono;


TEST_CASE("Timepoint constructor initializes to current time", "[Timepoint]") {
    const Timepoint tp;
    constexpr size_t toTest = 1;
    REQUIRE_FALSE(tp.Elapsed<milliseconds>(milliseconds(toTest)));
    REQUIRE_FALSE(tp.Elapsed<seconds>(seconds(toTest)));
    REQUIRE_FALSE(tp.Elapsed<minutes>(minutes(toTest)));
}

TEST_CASE("ElapsedMilliseconds returns true after specified milliseconds", "[Timepoint]") {
    const Timepoint tp;
    constexpr size_t dur1 = 80;
    constexpr size_t dur2 = 40;
    constexpr size_t testDur1 = 100;
    constexpr size_t testDur2 = 200;
    this_thread::sleep_for(milliseconds(dur1));
    REQUIRE_FALSE(tp.Elapsed<milliseconds>(milliseconds(testDur1)));
    this_thread::sleep_for(milliseconds(dur2));
    REQUIRE(tp.Elapsed<milliseconds>(milliseconds(testDur1)));
    REQUIRE_FALSE(tp.Elapsed<milliseconds>(milliseconds(testDur2)));
}

TEST_CASE("ElapsedSeconds returns true after specified seconds", "[Timepoint]") {
    const Timepoint tp;
    constexpr size_t dur1 = 900;
    constexpr size_t dur2 = 300;
    constexpr size_t testDur1 = 1;
    constexpr size_t testDur2 = 2;
    this_thread::sleep_for(milliseconds(dur1));
    REQUIRE_FALSE(tp.Elapsed<seconds>(seconds(testDur1)));
    this_thread::sleep_for(milliseconds(dur2));
    REQUIRE(tp.Elapsed<seconds>(seconds(testDur1)));
    REQUIRE_FALSE(tp.Elapsed<seconds>(seconds(testDur2)));
}

TEST_CASE("ElapsedMinutes returns true after specified minutes", "[Timepoint]") {
    const Timepoint tp;
    constexpr size_t dur1 = 59;
    constexpr size_t dur2 = 2;
    constexpr size_t testDur1 = 1;
    constexpr size_t testDur2 = 2;
    this_thread::sleep_for(seconds(dur1));
    REQUIRE_FALSE(tp.Elapsed<minutes>(minutes(testDur1)));
    this_thread::sleep_for(seconds(dur2));
    REQUIRE(tp.Elapsed<minutes>(minutes(testDur1)));
    REQUIRE_FALSE(tp.Elapsed<minutes>(minutes(testDur2)));
}
