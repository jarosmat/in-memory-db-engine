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

TEST_CASE("Timepoint: RemainingTime with std::chrono::milliseconds", "[timepoint][remaining][milliseconds]") {
    SECTION("Time remaining (not expired)") {
        const Timepoint tp;
        auto remaining = tp.RemainingTime(100ms);
        REQUIRE(remaining >= 95ms);
        REQUIRE(remaining <= 100ms);
    }

    SECTION("Expired (zero remaining)") {
        const Timepoint tp;
        std::this_thread::sleep_for(150ms);
        auto remaining = tp.RemainingTime(100ms);
        REQUIRE(remaining == 0ms);
    }
}

TEST_CASE("Timepoint: RemainingTime with std::chrono::seconds", "[timepoint][remaining][seconds]") {
    SECTION("Time remaining (not expired)") {
        const Timepoint tp;
        auto remaining = tp.RemainingTime(2s);
        REQUIRE(remaining >= 1900ms);
        REQUIRE(remaining <= 2000ms);
    }

    SECTION("Expired (zero remaining)") {
        const Timepoint tp;
        std::this_thread::sleep_for(2100ms);
        auto remaining = tp.RemainingTime(2s);
        REQUIRE(remaining == 0s);
    }
}

TEST_CASE("Timepoint: RemainingTime with std::chrono::minutes", "[timepoint][remaining][minutes]") {
    SECTION("Time remaining (not expired)") {
        const Timepoint tp;
        auto remaining = tp.RemainingTime(1min);
        REQUIRE(remaining >= 59s);
        REQUIRE(remaining <= 60s);
    }

    SECTION("Expired (zero remaining)") {
        const Timepoint tp;
        std::this_thread::sleep_for(61s);
        auto remaining = tp.RemainingTime(1min);
        REQUIRE(remaining == 0min);
    }
}
